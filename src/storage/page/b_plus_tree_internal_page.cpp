//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/page/b_plus_tree_internal_page.cpp
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <sstream>

#include "common/exception.h"
#include "storage/page/b_plus_tree_internal_page.h"

namespace bustub {
/*****************************************************************************
 * HELPER METHODS AND UTILITIES
 *****************************************************************************/
/*
 * Init method after creating a new internal page
 * Including set page type, set current size, set page id, set parent id and set
 * max page size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::Init(page_id_t page_id, page_id_t parent_id, int max_size) {
  page_type_ = IndexPageType::INTERNAL_PAGE;
  size_ = 0;
  max_size_ = max_size;
  parent_page_id_ = parent_id;
  page_id_ = page_id;
}
/*
 * Helper method to get/set the key associated with input "index"(a.k.a
 * array offset)
 */
INDEX_TEMPLATE_ARGUMENTS
KeyType B_PLUS_TREE_INTERNAL_PAGE_TYPE::KeyAt(int index) const {
  // replace with your own code
  //Q:是否可能产生数组越界？ by Sunlly0
  //A:使用C语言风格的数组，可以越界访问数组
  //可以不考虑超过size和max_size的问题，因为是辅助函数
  KeyType key{};
  auto pair = arrar[index];
  key = std::get<0>(pair);
  return key;
}

INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::SetKeyAt(int index, const KeyType &key) {
  //Q:key变化之后，value是否变化？
  //A:不会，指向的页面不变，只有关键字变化
    auto pair = arrar[index];
    std::get<0>(pair) = key;
}

/*
 * Helper method to find and return array index(or offset), so that its value
 * equals to input "value"
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_INTERNAL_PAGE_TYPE::ValueIndex(const ValueType &value) const { 
  for(int i=0;i<size;i++){
    auto pair = arrar[i];
    if(std::get<1>(pair)==value) return i;
  }
  // return 0; 
}

/*
 * Helper method to get the value associated with input "index"(a.k.a array
 * offset)
 */
INDEX_TEMPLATE_ARGUMENTS
ValueType B_PLUS_TREE_INTERNAL_PAGE_TYPE::ValueAt(int index) const {
  ValueType value{};
  auto pair = arrar[index];
  value = std::get<1>(pair);
  return value;
}

/*****************************************************************************
 * LOOKUP
 *****************************************************************************/
/*
 * Find and return the child pointer(page_id) which points to the child page
 * that contains input "key"
 * Start the search from the second key(the first key should always be invalid)
 */
INDEX_TEMPLATE_ARGUMENTS
ValueType B_PLUS_TREE_INTERNAL_PAGE_TYPE::Lookup(const KeyType &key, const KeyComparator &comparator) const {
  //遍历所有键进行查找
  for(int index=1;index<size_;index++){
    auto pagekey=KeyAt(index);
    //找到恰好比key大的值后
    if(comparator(pagekey,key<0)) return ValueAt(index-1);
  }
  //遍历完也没有找到比key大的值，则进入最右边的分支
  return ValueAt(size_);
  // return INVALID_PAGE_ID;
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/*
 * Populate new root page with old_value + new_key & new_value
 * When the insertion cause overflow from leaf page all the way upto the root
 * page, you should create a new root page and populate its elements.
 * NOTE: This method is only called within InsertIntoParent()(b_plus_tree.cpp)
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::PopulateNewRoot(const ValueType &old_value, const KeyType &new_key,
                                                     const ValueType &new_value) {
  //仅在作为叶子节点的根节点满的时候，分解成新的内部节点的时候调用
  auto pair=std::make_pair(new_key,new_value);
  std::get<1>(arrar[0])=value;
  arrar[1]=pair;
  this->SetSize(1);
}
/*
 * Insert new_key & new_value pair right after the pair with its value ==
 * old_value
 * @return:  new size after insertion
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_INTERNAL_PAGE_TYPE::InsertNodeAfter(const ValueType &old_value, const KeyType &new_key,
                                                    const ValueType &new_value) {
  auto index=ValueIndex(old_value);
  auto pair=std:make_pair(new_key,new_value);
  if(size_<max_size_){
    //插入键值对，对其后的所有键进行移位，保证有序
    for(int i=index+1;i<size_;i++){
      arrar[i+1]=arrar[i];
    }
    arrar[index+1]=pair;
    this->IncreaseSize(1);
  }
  //Q:如果插入后size==max_size_，是否要调用分裂函数？可能是树的实现要考虑的事情
  return size_;
}

/*****************************************************************************
 * SPLIT
 *****************************************************************************/
/*
 * Remove half of key & value pairs from this page to "recipient" page
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::MoveHalfTo(BPlusTreeInternalPage *recipient,
                                                BufferPoolManager *buffer_pool_manager) {
  int index=this->GetMinSize();
  keyType newkey=KeyAt(index);
  Valuetype newvalue=ValueAt(index);
  for(int i=index;i<size_;i++){
    recipient->arrar{i-index}=arrar{index};
    // arrar{index}=0;
  }
  recipient->SetSize(size_-index);
  this->Setsize(index);
}

/* Copy entries into me, starting from {items} and copy {size} entries.
 * Since it is an internal page, for all entries (pages) moved, their parents page now changes to me.
 * So I need to 'adopt' them by changing their parent page id, which needs to be persisted with BufferPoolManger
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::CopyNFrom(MappingType *items, int size, BufferPoolManager *buffer_pool_manager) {}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/*
 * Remove the key & value pair in internal page according to input index(a.k.a
 * array offset)
 * NOTE: store key&value pair continuously after deletion
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::Remove(int index) {
  for(int i=index;i<size_;i++){
    //从后往前移位
    arrar[i]=arrar[i+1];
    // if(i==size_) arrar[i]{};
  }
  // arrar[size_]
  //将最后一位清空
  size_--;
}

/*
 * Remove the only key & value pair in internal page and return the value
 * NOTE: only call this method within AdjustRoot()(in b_plus_tree.cpp)
 */
INDEX_TEMPLATE_ARGUMENTS
ValueType B_PLUS_TREE_INTERNAL_PAGE_TYPE::RemoveAndReturnOnlyChild() { 
  //只有一个键值对的时候（没有key，只有value）删除该页面
  if(size_==1){
    arrar{size_}=0;
    return ValueAt(0);
  }
  return INVALID_PAGE_ID; 
}
/*****************************************************************************
 * MERGE
 *****************************************************************************/
/*
 * Remove all of key & value pairs from this page to "recipient" page.
 * The middle_key is the separation key you should get from the parent. You need
 * to make sure the middle key is added to the recipient to maintain the invariant.
 * You also need to use BufferPoolManager to persist changes to the parent page id for those
 * pages that are moved to the recipient
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::MoveAllTo(BPlusTreeInternalPage *recipient, const KeyType &middle_key,
                                               BufferPoolManager *buffer_pool_manager) {
  auto index=KeyAt(middle_key);
  for(int i=0;i<recipient->size_;i++){
    arrar{index+0}=recipient->arrar{index};
  }
  this->IncreaseSize(recipient->size_);
  //Q:如何处理recipient数组的清除和添加middle_key?
  buffer_pool_manager->DeletePageImpl(recipinent->page_id);
}

/*****************************************************************************
 * REDISTRIBUTE
 *****************************************************************************/
/*
 * Remove the first key & value pair from this page to tail of "recipient" page.
 *
 * The middle_key is the separation key you should get from the parent. You need
 * to make sure the middle key is added to the recipient to maintain the invariant.
 * You also need to use BufferPoolManager to persist changes to the parent page id for those
 * pages that are moved to the recipient
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::MoveFirstToEndOf(BPlusTreeInternalPage *recipient, const KeyType &middle_key,
                                                      BufferPoolManager *buffer_pool_manager) {
  //Q:被旁边的节点借键值对的情况？
  auto pair=array{0};
  //将第一个键值对加在recipient页之后
  recipient->IncreaseSize(1);
  recipient->array{size_}=pair;
  //对于自己，数组往前移动一位
  for(int index=0;index<size_;index++){
    arrar[index]=arrar[index+1];
  }
  size_--;
  //更新middle_key并且将父节点写出到磁盘
  middle_key=std::get<0>(pair);
  buffer_pool_manager->FlushPageImpl(parent_page_id_);
}

/* Append an entry at the end.
 * Since it is an internal page, the moved entry(page)'s parent needs to be updated.
 * So I need to 'adopt' it by changing its parent page id, which needs to be persisted with BufferPoolManger
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::CopyLastFrom(const MappingType &pair, BufferPoolManager *buffer_pool_manager) {

  buffer_pool_manager->FlushPageImpl(page_id_);
}

/*
 * Remove the last key & value pair from this page to head of "recipient" page.
 * You need to handle the original dummy key properly, e.g. updating recipient’s array to position the middle_key at the
 * right place.
 * You also need to use BufferPoolManager to persist changes to the parent page id for those pages that are
 * moved to the recipient
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::MoveLastToFrontOf(BPlusTreeInternalPage *recipient, const KeyType &middle_key,
                                                       BufferPoolManager *buffer_pool_manager) {
  //Q:被旁边的节点借键值对的情况？
  auto pair=array{size_};
  //数组往后移动一位
  for(int index=0;index<recipient->size_;index++){
    recipient->arrar[index+1]=recipient->arrar[index];
  }
  //将第一个键值对加在recipient页之后
  recipient->IncreaseSize(1);
  recipient->array{0}=pair;
  size_--;
  //更新middle_key并且将父节点写出到磁盘
  middle_key=std::get<0>(pair);
  buffer_pool_manager->FlushPageImpl(parent_page_id_);
}

/* Append an entry at the beginning.
 * Since it is an internal page, the moved entry(page)'s parent needs to be updated.
 * So I need to 'adopt' it by changing its parent page id, which needs to be persisted with BufferPoolManger
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::CopyFirstFrom(const MappingType &pair, BufferPoolManager *buffer_pool_manager) {}

// valuetype for internalNode should be page id_t
template class BPlusTreeInternalPage<GenericKey<4>, page_id_t, GenericComparator<4>>;
template class BPlusTreeInternalPage<GenericKey<8>, page_id_t, GenericComparator<8>>;
template class BPlusTreeInternalPage<GenericKey<16>, page_id_t, GenericComparator<16>>;
template class BPlusTreeInternalPage<GenericKey<32>, page_id_t, GenericComparator<32>>;
template class BPlusTreeInternalPage<GenericKey<64>, page_id_t, GenericComparator<64>>;
}  // namespace bustub
