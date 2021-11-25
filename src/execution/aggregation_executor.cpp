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
  //将子执行器中满足条件的元组，进行分组，加入a_hashtable
  // （keys列的值相同的为一组），提取avals
  Tuple child_tuple{};
  RID rid{};
  while(child_->Next(&child_tuple,&rid)){
    auto akeys=MakeKey(&child_tuple);
    auto avals=MakeVal(&child_tuple);
    aht_.InsertCombine(akeys,avals);
  }
  aht_iterator_=aht_.Begin();
}

bool AggregationExecutor::Next(Tuple *tuple, RID *rid) {
  if(aht_iterator_!=aht_.End()){
    auto key=aht_iterator_.Key();
    auto val=aht_iterator_.Val();
    ++aht_iterator_;
    if(plan_->GetHaving()== nullptr||plan_->GetHaving()->EvaluateAggregate(key.group_bys_,val.aggregates_).GetAs<bool>()){
      std::vector<Value> vals;
      for (const Column &col : GetOutputSchema()->GetColumns()) {
        vals.push_back(col.GetExpr()->EvaluateAggregate(key.group_bys_,val.aggregates_));
      }
      *tuple={vals,GetOutputSchema()};
      *rid=tuple->GetRid();
      return true;
    }
  }
  return false;
}

}  // namespace bustub
