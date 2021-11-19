//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// aggregation_executor.cpp
//
// Identification: src/execution/aggregation_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <memory>
#include <vector>

#include "execution/executors/aggregation_executor.h"

namespace bustub {

AggregationExecutor::AggregationExecutor(ExecutorContext *exec_ctx, const AggregationPlanNode *plan,
                                         std::unique_ptr<AbstractExecutor> &&child)
    : AbstractExecutor(exec_ctx), plan_(plan),aht_(plan_->GetAggregates(),plan_->GetAggregateTypes()),
      aht_iterator_(aht_.Begin()){
  child_=std::move(child);
}

const AbstractExecutor *AggregationExecutor::GetChildExecutor() const {
  return child_.get();
}

void AggregationExecutor::Init() {
  child_->Init();


}

bool AggregationExecutor::Next(Tuple *tuple, RID *rid) {
  Tuple *child_tuple=new Tuple();
  while(child_->Next(child_tuple,rid)){
    auto akeys=MakeKey(child_tuple);
    auto avals=MakeVal(child_tuple);
    aht_.InsertCombine(akeys,avals);
  }

  return true;

  return false;
}

}  // namespace bustub
