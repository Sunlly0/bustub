//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// insert_executor.cpp
//
// Identification: src/execution/insert_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <memory>

#include "execution/executors/insert_executor.h"

namespace bustub {

InsertExecutor::InsertExecutor(ExecutorContext *exec_ctx, const InsertPlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {
  plan_=plan;
  index_=0;
  child_executor_=std::move(child_executor);
}

void InsertExecutor::Init() {
  // 插入：暂时不考虑索引
  table_info_ = exec_ctx_->GetCatalog()->GetTable(plan_->TableOid());
  table_heap_=table_info_->table_.get();
  //Q:若空表上没有索引，理论上应该返回空的all_index_info_
  all_index_info_=exec_ctx_->GetCatalog()->GetTableIndexes(table_info_->name_);
  //Q: 如果忘记对子执行器初始化，后续会出错！
  if(child_executor_!=nullptr){
    child_executor_->Init();
  }

}

bool InsertExecutor::Next([[maybe_unused]] Tuple *tuple, RID *rid) {
  //插入值，修改索引
  //1. 如果是带值的插入
  if(plan_->IsRawInsert()){
//    if(index_<plan_->RawValues().size()){
    while(index_<plan_->RawValues().size()){
      //新建元组并插入表
      Tuple insert_tuple(plan_->RawValuesAt(index_),&table_info_->schema_);
      table_heap_->InsertTuple(insert_tuple,rid,exec_ctx_->GetTransaction());
      //更新索引
      for(auto &index_info:all_index_info_){
        //Q:不能声明index?
        //A:与unique_ptr有关
//        auto index=index_info->index_;
        auto index_key=insert_tuple.KeyFromTuple(table_info_->schema_,index_info->key_schema_,index_info->index_->GetKeyAttrs());
        index_info->index_->InsertEntry(index_key,*rid,exec_ctx_->GetTransaction());
      }
      index_++;
//      return true;
    }
  }
  // 2. 如果是不带值的插入，要从子执行器中的result_set中获取值
  else{
//    if (child_executor_->Next(tuple, rid)) {
    while (child_executor_->Next(tuple, rid)) {
      //从子执行器中获取满足条件的元组并插入
      table_heap_->InsertTuple(*tuple, rid,exec_ctx_->GetTransaction());

      //更新索引
      for(auto &index_info:all_index_info_){
        auto index_key=tuple->KeyFromTuple(table_info_->schema_,index_info->key_schema_,index_info->index_->GetKeyAttrs());
        index_info->index_->InsertEntry(index_key,*rid,exec_ctx_->GetTransaction());
      }
//      return true;
    }
  }
  return false;
}

}  // namespace bustub
