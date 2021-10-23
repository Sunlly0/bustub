/**
 * index_iterator.cpp
 */
#include <cassert>

#include "storage/index/index_iterator.h"

namespace bustub {

/*
 * NOTE: you can change the destructor/constructor method here
 * set your own input parameters
 */
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::IndexIterator(B_PLUS_TREE_LEAF_PAGE_TYPE  *leftmost_leaf, int idx, BufferPoolManager *buffer_pool_manager):
  leaf(leftmost_leaf),index(idx), bpm(buffer_pool_manager){};

INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::~IndexIterator() = default;

INDEX_TEMPLATE_ARGUMENTS
bool INDEXITERATOR_TYPE::isEnd() {
  if(leaf->GetNextPageId()==INVALID_PAGE_ID&& index>=leaf->GetSize()-1) return true;
  return false;
}

INDEX_TEMPLATE_ARGUMENTS
const MappingType &INDEXITERATOR_TYPE::operator*() {
  return leaf->GetItem(index);
}

INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE &INDEXITERATOR_TYPE::operator++() {
  index++;
  //1. 当index超过了叶节点最大时，跳转到下一个叶节点的开头
  if(index>=leaf->GetSize()&&leaf->GetNextPageId()!=INVALID_PAGE_ID){
    B_PLUS_TREE_LEAF_PAGE_TYPE *nextleaf=reinterpret_cast<B_PLUS_TREE_LEAF_PAGE_TYPE *>(bpm->FetchPage(leaf->GetNextPageId()));
    leaf=nextleaf;
    index=0;
    bpm->UnpinPage(leaf->GetPageId(), false);
  }
  //2. 如果index 没有超过叶节点最大，还是在该叶节点上，index+=1
  return *this;
}

template class IndexIterator<GenericKey<4>, RID, GenericComparator<4>>;

template class IndexIterator<GenericKey<8>, RID, GenericComparator<8>>;

template class IndexIterator<GenericKey<16>, RID, GenericComparator<16>>;

template class IndexIterator<GenericKey<32>, RID, GenericComparator<32>>;

template class IndexIterator<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
