//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lock_manager.cpp
//
// Identification: src/concurrency/lock_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "concurrency/lock_manager.h"

#include <utility>
#include <vector>

namespace bustub {

bool LockManager::LockShared(Transaction *txn, const RID &rid) {
  //处理事务的增长和收缩阶段
  //如何考虑隔离级别？
  //某个事务申请rid元组上的共享锁
  //0. 判断是否为增长阶段(GROWING)，如果不是，不能获取锁。状态：Abort
  if(txn->GetState()==TransactionState::SHRINKING){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(),AbortReason::LOCK_ON_SHRINKING);
  }
  if(txn->GetState()==TransactionState::GROWING){
    std::unique_lock<std::mutex> latch(latch_);
    //1. 先从lock_table 中获取该元组对应的lockquestqueue;
    LockRequest req(txn->GetTransactionId(),LockMode::SHARED);
    bool block=false;
    //1.1 lock_table中的rid对应该项不为空，则不是第一次请求
    if(lock_table_.find(rid)!=lock_table_.end()){
      //2.检查是否有更新(upgrading==true)或其他等待排他锁状态的请求
      if(lock_table_[rid].upgrading_==true) block=true;
      else {
        if (!lock_table_[rid].request_queue_.empty()) {
          for (auto iter = lock_table_[rid].request_queue_.begin(); iter != lock_table_[rid].request_queue_.end();
               iter++) {
            if (iter->lock_mode_ == LockMode::EXCLUSIVE && iter->granted_ == true) {
              block = true;
            }
          }
        }
      }
    }
    //1.2 lock_table中的rid对应该项为空，是第一次请求，前面一定没有别的请求，直接获得锁
    else{
      //初始化
      lock_table_[rid].request_queue_.clear();
    }
    //2.1 如果是，则将请求加入队列，grant=false,并设置cv成等待,收到通知后继续操作
    //2.1.2 收到通知，grant=true
    if(block) {
      req.granted_=false;
      lock_table_[rid].request_queue_.push_back(req);
      lock_table_[rid].cv_.wait(latch);
      for (auto iter = lock_table_[rid].request_queue_.begin(); iter != lock_table_[rid].request_queue_.end();
           iter++) {
        if (iter->txn_id_==txn->GetTransactionId()) {
          iter->granted_= true;
        }
      }
      block = false;
    }
    //2.2 如果不是，则该事务跳过等待,grant=true
    else{
      req.granted_=true;
      lock_table_[rid].request_queue_.push_back(req);
    }

    //3. 顺利拿到共享锁
    txn->GetSharedLockSet()->emplace(rid);
    return true;
  }
  return false;
}

bool LockManager::LockExclusive(Transaction *txn, const RID &rid) {
  //某个事务申请rid元组上的排他锁
  //0. 判断是否为增长阶段(GROWING)，如果不是，不能获取锁。状态：Abort
  if(txn->GetState()==TransactionState::SHRINKING){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(),AbortReason::LOCK_ON_SHRINKING);
  }
  if(txn->GetState()==TransactionState::GROWING){
    std::unique_lock<std::mutex> latch(latch_);
    //1. 先从lock_table 中获取该元组对应的lockquestqueue;
    LockRequest req(txn->GetTransactionId(),LockMode::EXCLUSIVE);
    bool block=false;
    //1.1 lock_table中的rid对应该项不为空，则不是第一次请求
    if(lock_table_.find(rid)!=lock_table_.end()){
      //2.检查是否有更新(upgrading==true)或其他等待共享锁或排他锁状态的请求（queue>1）,granted==true
      if(lock_table_[rid].upgrading_==true) block=true;
      else {
        if (!lock_table_[rid].request_queue_.empty()) {
          for (auto iter = lock_table_[rid].request_queue_.begin(); iter != lock_table_[rid].request_queue_.end();
               iter++) {
            if (iter->granted_ == true) {
              block = true;
            }
          }
        }
      }
    }
    //1.2 lock_table中的rid对应该项为空，是第一次请求，前面一定没有别的请求，直接获得锁
    else{
      //初始化
      lock_table_[rid].request_queue_.clear();
    }
    //2.1 如果是，则将请求加入队列，grant=false,并设置cv成等待,收到通知后继续操作
    //2.1.2 收到通知，grant=true
    if(block) {
      req.granted_=false;
      lock_table_[rid].request_queue_.push_back(req);
      lock_table_[rid].cv_.wait(latch);
      for (auto iter = lock_table_[rid].request_queue_.begin(); iter != lock_table_[rid].request_queue_.end();
           iter++) {
        if (iter->txn_id_==txn->GetTransactionId()) {
          iter=lock_table_[rid].request_queue_.erase(iter);
        }
      }

    }
    //2.2 如果不是，则该事务跳过等待,grant=true
    req.granted_=true;
    lock_table_[rid].request_queue_.push_back(req);
    //3. 顺利拿到排他锁
    txn->GetExclusiveLockSet()->emplace(rid);
    return true;
  }
  return false;
}

bool LockManager::LockUpgrade(Transaction *txn, const RID &rid) {
  //某个事务申请rid元组上的升级（更新锁）
  //0. 判断是否为增长阶段(GROWING)，如果不是，不能获取锁。状态：Abort
  if(txn->GetState()==TransactionState::SHRINKING){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(),AbortReason::LOCK_ON_SHRINKING);
  }
  if(txn->GetState()==TransactionState::GROWING){
    std::unique_lock<std::mutex> latch(latch_);
    //1. 先从lock_table 中获取该元组对应的lockquestqueue;
    bool block=false;
    //2.检查是否有更新(upgrading==true)或其他等待共享锁或排他锁状态的请求（queue>1）,granted==true
    if(lock_table_[rid].upgrading_==true) block=true;
    else {
      if (!lock_table_[rid].request_queue_.empty()) {
        for (auto iter = lock_table_[rid].request_queue_.begin(); iter != lock_table_[rid].request_queue_.end();
             iter++) {
          //排除自己
          if (iter->txn_id_!=txn->GetTransactionId()&&iter->granted_ == true) {
            block = true;
          }
        }
      }
    }
    //2.1 如果是，则将请求加入队列，grant=false,并设置cv成等待,收到通知后继续操作
    //2.1.2 收到通知，grant=true
    if(block) {
      lock_table_[rid].cv_.wait(latch);
    }
    //3. 更新锁
    //3.1.queue标记为更新状态
    lock_table_[rid].upgrading_= true;
    //3.2找到相应的请求并更新lockmode为排他锁
    for (auto iter = lock_table_[rid].request_queue_.begin(); iter != lock_table_[rid].request_queue_.end();
         iter++) {
      if (iter->txn_id_==txn->GetTransactionId()) {
        iter->lock_mode_=LockMode::EXCLUSIVE;

        //4.事务锁列表更新
        txn->GetSharedLockSet()->erase(rid);
        txn->GetExclusiveLockSet()->emplace(rid);

        //5. 取消queue的更新状态
        lock_table_[rid].upgrading_= false;
        return true;
      }
    }
  }
  return false;
}

bool LockManager::Unlock(Transaction *txn, const RID &rid) {
  //0. 如果是增长阶段(GROWING)，修改成为收缩阶段(SHRINKING)
  if(txn->GetState()==TransactionState::GROWING) {
    txn->SetState(TransactionState::SHRINKING);
  }
//  //0.1 对于强2pl策略，应该在释放时将所有锁释放（不会一个一个释放），释放完所有锁就提交
//  if(txn->GetState()==TransactionState::SHRINKING){
//    txn->SetState(TransactionState::ABORTED);
//    throw TransactionAbortException(txn->GetTransactionId(),AbortReason::UNLOCK_ON_SHRINKING);
//  }
  //1.从事务的锁列表中，释放该锁
  for (auto iter = lock_table_[rid].request_queue_.begin(); iter != lock_table_[rid].request_queue_.end();
       iter++) {
    if (iter->txn_id_==txn->GetTransactionId()) {
      if(iter->lock_mode_==LockMode::SHARED){
        txn->GetSharedLockSet()->erase(rid);
      }
      else if(iter->lock_mode_==LockMode::EXCLUSIVE){
        txn->GetExclusiveLockSet()->erase(rid);
      }
      lock_table_[rid].request_queue_.erase(iter);

      //2.锁表中清除该事务的请求，并通过cv其他事务
      lock_table_[rid].cv_.notify_one();
      return true;
    }
  }
  return false;
}

void LockManager::AddEdge(txn_id_t t1, txn_id_t t2) {}

void LockManager::RemoveEdge(txn_id_t t1, txn_id_t t2) {}

bool LockManager::HasCycle(txn_id_t *txn_id) { return false; }

std::vector<std::pair<txn_id_t, txn_id_t>> LockManager::GetEdgeList() { return {}; }

void LockManager::RunCycleDetection() {
  while (enable_cycle_detection_) {
    std::this_thread::sleep_for(cycle_detection_interval);
    {
      std::unique_lock<std::mutex> l(latch_);
      // TODO(student): remove the continue and add your cycle detection and abort code here
      continue;
    }
  }
}

}  // namespace bustub
