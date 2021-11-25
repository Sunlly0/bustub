//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// limit_executor.cpp
//
// Identification: src/execution/limit_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/limit_executor.h"

namespace bustub {

LimitExecutor::LimitExecutor(ExecutorContext *exec_ctx, const LimitPlanNode *plan,
                             std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {
  plan_=plan;
  child_executor_=std::move(child_executor);
}

void LimitExecutor::Init() {
  child_executor_->Init();
  //初始阶段，跳过offset个单位
  Tuple *tuple=new Tuple();
  RID rid{};
  for(size_t i=0;i<plan_->GetOffset();i++){
    child_executor_->Next(tuple,&rid);
  }
  num=0;
}

bool LimitExecutor::Next(Tuple *tuple, RID *rid) {
  //Next阶段，limit数量没有超过num，就会调用Next，每调用一次num++
  if(num<plan_->GetLimit()) {
    num++;
    if(child_executor_->Next(tuple, rid)){
      return true;
    }
  }
  return false;
}

}  // namespace bustub
