//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// update_executor.cpp
//
// Identification: src/execution/update_executor.cpp
//
// Copyright (c) 2015-20, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <memory>

#include "execution/executors/update_executor.h"

namespace bustub {

UpdateExecutor::UpdateExecutor(ExecutorContext *exec_ctx, const UpdatePlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {
  plan_=plan;
  //Q: 为什么要用std::move？
  child_executor_=std::move(child_executor);
}

void UpdateExecutor::Init() {
  table_info_ = exec_ctx_->GetCatalog()->GetTable(plan_->TableOid());
  table_heap_=table_info_->table_.get();
  all_index_info_ = exec_ctx_->GetCatalog()->GetTableIndexes(table_info_->name_);

  //Q: 如果忘记对子执行器初始化，后续会出错！
  if(child_executor_!=nullptr){
    child_executor_->Init();
  }

}

bool UpdateExecutor::Next([[maybe_unused]] Tuple *tuple, RID *rid) {
  //从子执行器中获取元组并更新
  if (child_executor_->Next(tuple, rid)) {
    //创建更新后的元组
    //Q:此处的更新调用项目已经实现的函数GenerateUpdatedTuple，具体做了啥操作？
    Tuple updated_tuple=GenerateUpdatedTuple(*tuple);
    //对表中的元素进行更新
    table_heap_->UpdateTuple(updated_tuple,*rid, exec_ctx_->GetTransaction());

//    //Q: 是否需要重建索引？
    for(auto &index_info:all_index_info_){
      auto index=index_info->index_.get();
      auto index_key=tuple->KeyFromTuple(table_info_->schema_,index_info->key_schema_,index->GetKeyAttrs());
      index->DeleteEntry(index_key,*rid,exec_ctx_->GetTransaction());
      index->InsertEntry(index_key,*rid,exec_ctx_->GetTransaction());
    }

    return true;
  }
  return false;
}
}  // namespace bustub
