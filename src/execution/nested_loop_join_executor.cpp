//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// nested_loop_join_executor.cpp
//
// Identification: src/execution/nested_loop_join_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/nested_loop_join_executor.h"

namespace bustub {

NestedLoopJoinExecutor::NestedLoopJoinExecutor(ExecutorContext *exec_ctx, const NestedLoopJoinPlanNode *plan,
                                               std::unique_ptr<AbstractExecutor> &&left_executor,
                                               std::unique_ptr<AbstractExecutor> &&right_executor)
    : AbstractExecutor(exec_ctx) {
  plan_=plan;
  left_executor_=std::move(left_executor);
  right_executor_=std::move(right_executor);
}

void NestedLoopJoinExecutor::Init() {
  left_executor_->Init();
  right_executor_->Init();
  left_schema=plan_->GetLeftPlan()->OutputSchema();
  right_schema=plan_->GetRightPlan()->OutputSchema();
}

bool NestedLoopJoinExecutor::Next(Tuple *tuple, RID *rid) {
  Tuple *left_tuple=new Tuple();
  Tuple *right_tuple=new Tuple();
  while(left_executor_->Next(left_tuple,rid)){
    while(right_executor_->Next(right_tuple,rid)){
      if(plan_->Predicate()== nullptr){
        std::vector<Value> left_values = GetAllValues(left_tuple,left_schema);
        std::vector<Value> right_values = GetAllValues(right_tuple,right_schema);
        left_values.insert(left_values.end(), right_values.begin(), right_values.end());
        *tuple=Tuple(left_values, plan_->OutputSchema());
        return true;
      }
      else{
        Value res = plan_->Predicate()->EvaluateJoin(left_tuple,left_schema,right_tuple,right_schema);
        if(res.GetAs<bool>()){
          //拼接元组
          std::vector<Value> left_values = GetAllValues(left_tuple,left_schema);
          std::vector<Value> right_values = GetAllValues(right_tuple,right_schema);
          left_values.insert(left_values.end(), right_values.begin(), right_values.end());
          *tuple=Tuple(left_values, plan_->OutputSchema());
          *rid=tuple->GetRid();
          return true;
        }
      }
    }
  }
  return false;
}

std::vector<Value> NestedLoopJoinExecutor::GetAllValues(const Tuple *tuple,const Schema *schema){
  std::vector<Value> vals;
  for (const Column &col : schema->GetColumns()) {
    Value val = tuple->GetValue(schema, schema->GetColIdx(col.GetName()));
    vals.push_back(val);
  }
  return vals;
}


}  // namespace bustub
