//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// nested_index_join_executor.cpp
//
// Identification: src/execution/nested_index_join_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/nested_index_join_executor.h"

namespace bustub {

NestIndexJoinExecutor::NestIndexJoinExecutor(ExecutorContext *exec_ctx, const NestedIndexJoinPlanNode *plan,
                                             std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {
  plan_=plan;
  child_executor_=std::move(child_executor);
}

void NestIndexJoinExecutor::Init() {
  TableMetadata *table_info=exec_ctx_->GetCatalog()->GetTable(plan_->GetInnerTableOid());
  index_info_=exec_ctx_->GetCatalog()->GetIndex(plan_->GetIndexName(),table_info->name_);
  index_=static_cast<BPlusTreeIndex<GenericKey<8>, RID, GenericComparator<8>> *>(index_info_->index_.get());
  table_heap_=exec_ctx_->GetCatalog()->GetTable(plan_->GetInnerTableOid())->table_.get();
  child_executor_->Init();
}

bool NestIndexJoinExecutor::Next(Tuple *tuple, RID *rid) {
  Tuple left_tuple;
  Tuple right_tuple;
  while (child_executor_->Next(&left_tuple,rid)) {
    std::vector<RID> rid_index;
    auto index_key =
        left_tuple.KeyFromTuple(*(plan_->OuterTableSchema()), index_info_->key_schema_, index_->GetKeyAttrs());
    index_->ScanKey(index_key, &rid_index, exec_ctx_->GetTransaction());
    if (rid_index.size() > 0) {
      bool res = table_heap_->GetTuple(rid_index.at(0), &right_tuple, exec_ctx_->GetTransaction());
      std::vector<Value> left_values = GetAllValues(&left_tuple, plan_->OuterTableSchema());
      std::vector<Value> right_values = GetAllValues(&right_tuple, plan_->InnerTableSchema());
      left_values.insert(left_values.end(), right_values.begin(), right_values.end());
      *tuple = Tuple(left_values, plan_->OutputSchema());
      *rid = tuple->GetRid();
      return true;
    }
  }
  return false;
}

std::vector<Value> NestIndexJoinExecutor::GetAllValues(const Tuple *tuple,const Schema *schema){
  std::vector<Value> vals;
  for (const Column &col : schema->GetColumns()) {
    Value val = tuple->GetValue(schema, schema->GetColIdx(col.GetName()));
    vals.push_back(val);
  }
  return vals;
}

}  // namespace bustub
