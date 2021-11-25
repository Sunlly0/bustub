//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// delete_executor.cpp
//
// Identification: src/execution/delete_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <memory>

#include "execution/executors/delete_executor.h"

namespace bustub {

DeleteExecutor::DeleteExecutor(ExecutorContext *exec_ctx, const DeletePlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {
  plan_=plan;
  //Q: 为什么要用std::move？
  child_executor_=std::move(child_executor);
}

void DeleteExecutor::Init() {
    table_info_=exec_ctx_->GetCatalog()->GetTable(plan_->TableOid());
    all_index_info_ = exec_ctx_->GetCatalog()->GetTableIndexes(table_info_->name_);
    table_heap_=table_info_->table_.get();
    //Q: 如果忘记对子执行器初始化，后续会出错！
    if(child_executor_!=nullptr){
      child_executor_->Init();
    }
}

bool DeleteExecutor::Next([[maybe_unused]] Tuple *tuple, RID *rid) {
  //从子执行器中获取元组并删除
  if (child_executor_->Next(tuple, rid)) {
    //对表中的元组进行删除
    //此处用的MarkDelete，在后续事务提交时才会真正删除
    if(table_heap_->MarkDelete(*rid, exec_ctx_->GetTransaction())){
      //Q:由于上面是MarkDelete，此处是否需要删除索引？
      for(auto &index_info:all_index_info_){
        auto index_key=tuple->KeyFromTuple(table_info_->schema_,index_info->key_schema_,index_info->index_->GetKeyAttrs());
        index_info->index_->DeleteEntry(index_key,*rid,exec_ctx_->GetTransaction());
      }
      return true;
    }
  }
  return false;

}

}  // namespace bustub
