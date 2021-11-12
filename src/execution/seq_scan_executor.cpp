//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// seq_scan_executor.cpp
//
// Identification: src/execution/seq_scan_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include "execution/executors/seq_scan_executor.h"

namespace bustub {

//SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan) : AbstractExecutor(exec_ctx),table_info_(exec_ctx_->GetCatalog()->GetTable(plan_->GetTableOid())),plan_(plan),table_heap_(table_info_->table_.get()),iter(table_heap_->Begin(exec_ctx_->GetTransaction())) {

SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan) : AbstractExecutor(exec_ctx),iter(nullptr,RID(), nullptr),table_info_(exec_ctx_->GetCatalog()->GetTable(plan_->GetTableOid())),plan_(plan){
};

void SeqScanExecutor::Init() {
  table_info_ = exec_ctx_->GetCatalog()->GetTable(plan_->GetTableOid());
  table_heap_=table_info_->table_.get();
  iter= table_heap_->Begin(exec_ctx_->GetTransaction());
}

bool SeqScanExecutor::Next(Tuple *tuple, RID *rid) {

  Schema &schema = table_info_->schema_;
  if(iter != table_heap_->End()){
//    Tuple *tuple_=&(*iter);
    Value res=plan_->GetPredicate()->Evaluate(&(*iter),GetOutputSchema());
//    Value res=plan_->GetPredicate()->Evaluate(&(*iter),&schema);
    if(res.GetAs<bool>()){
//      *tuple= GetOutputTuple(*iter,&schema);
      *tuple= *iter;
      *rid=iter->GetRid();
      iter++;
      return true;
    }
    else{
      iter++;
      return false;
    }

  }
  return false;
}
//
//Tuple SeqScanExecutor::GetOutputTuple(const Tuple tuple, const Schema *schema){
//  std::vector<Value> vals;
//  for (const Column &col : schema->GetColumns()) {
//    Value val = tuple.GetValue(schema, schema->GetColIdx(col.GetName()));
//    vals.push_back(val);
//  }
//  return Tuple(vals,schema);
//}

//}

}  // namespace bustub
