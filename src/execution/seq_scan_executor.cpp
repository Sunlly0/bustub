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

SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan) : AbstractExecutor(exec_ctx) {
  plan_=plan;
}

void SeqScanExecutor::Init() {
  table_heap=exec_ctx_->GetCatalog()->GetTable(plan_->GetTableOid())->;

}

bool SeqScanExecutor::Next(Tuple *tuple, RID *rid) {
  for (auto iterator = table_heap->Begin(exec_ctx_->GetTransaction()); iterator != table_heap->end(); ++iterator) {
//    auto location = (*iterator).second;
//    EXPECT_EQ(location.GetPageId(), 0);
//    EXPECT_EQ(location.GetSlotNum(), current_key);
//    current_key = current_key + 1;
  }
  return false; }

}  // namespace bustub
