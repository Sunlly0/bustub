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

//SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan) : AbstractExecutor(exec_ctx),iter(nullptr,RID(), nullptr),table_info_(exec_ctx_->GetCatalog()->GetTable(plan_->GetTableOid())),plan_(plan){
//};

SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan) : AbstractExecutor(exec_ctx){
  plan_=plan;
};

void SeqScanExecutor::Init() {
  table_info_ = exec_ctx_->GetCatalog()->GetTable(plan_->GetTableOid());
  table_heap_=table_info_->table_.get();
  iter= table_heap_->Begin(exec_ctx_->GetTransaction());
}

bool SeqScanExecutor::Next(Tuple *tuple, RID *rid) {
//  Schema &schema = table_info_->schema_;
//表迭代器从开始往后遍历，如果到了结尾则返回false
  //注意：此处找到满足谓词条件元组会返回true，暂时跳出循环，在executor_eniginer中把结果添加至结果集
  // 如果元组不满足谓词条件，则不会return，往下继续遍历,直到找到满足谓词的元素或扫描到结尾
  while(iter != table_heap_->End()) {
    //*iter的内容就是元组
    *tuple = *iter;
    *rid = tuple->GetRid();
    //是否有谓词限制，
    //1. 没有谓词限定，直接返回true
    if(plan_->GetPredicate()==nullptr){
      iter++;
      return true;
    }
    else{
      // 2. 判断元素是否满足谓词要求
      // 此处是否能在schema直接用outputschema?
      Value res = plan_->GetPredicate()->Evaluate(tuple, plan_->OutputSchema());
      iter++;
      // 将res转换为bool，如果满足谓词要求，则返回引用结果和return true

      if (res.GetAs<bool>()) {
        return true;
      }
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
