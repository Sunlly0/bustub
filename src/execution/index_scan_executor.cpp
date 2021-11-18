//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// index_scan_executor.cpp
//
// Identification: src/execution/index_scan_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include "execution/executors/index_scan_executor.h"

namespace bustub {
IndexScanExecutor::IndexScanExecutor(ExecutorContext *exec_ctx, const IndexScanPlanNode *plan)
    : AbstractExecutor(exec_ctx) {
  plan_=plan;
}

void IndexScanExecutor::Init() {
  index_=static_cast<BPlusTreeIndex<GenericKey<8>, RID, GenericComparator<8>> *>(exec_ctx_->GetCatalog()->GetIndex(plan_->GetIndexOid())->index_.get());
  table_info_=exec_ctx_->GetCatalog()->GetTable(index_->GetMetadata()->GetTableName());
  table_heap_=table_info_->table_.get();
  index_iter_=index_->GetBeginIterator();

}

bool IndexScanExecutor::Next(Tuple *tuple, RID *rid) {
//  Schema &schema = table_info_->schema_;
  //遍历索引
  if(index_iter_ != index_->GetEndIterator()){
    // 从索引迭代器中获取rid，再从表中获取元组
    //*index_iter的内容是索引的叶子结点
    table_heap_->GetTuple((*index_iter_).second,tuple,exec_ctx_->GetTransaction());

    // 判断元素是否满足谓词要求
    // 此处是否能在schema直接用outputschema?
    Value res=plan_->GetPredicate()->Evaluate(tuple,GetOutputSchema());
    // 将res转换为bool，如果满足谓词要求，则返回引用结果和return true
    //Q:是否是因为索引有序？
    if(res.GetAs<bool>()){
      *rid=(*index_iter_).second;
      ++index_iter_;
      return true;
    }
    else{
      ++index_iter_;
      return false;
    }
  }
  return false;
}


}  // namespace bustub
