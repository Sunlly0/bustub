//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/page/b_plus_tree_leaf_page.cpp
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <sstream>

#include "common/exception.h"
#include "common/rid.h"
#include "storage/page/b_plus_tree_leaf_page.h"

namespace bustub {

/*****************************************************************************
 * HELPER METHODS AND UTILITIES
 *****************************************************************************/

/**
 * Init method after creating a new leaf page
 * Including set page type, set current size to zero, set page id/parent id, set
 * next page id and set max size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::Init(page_id_t page_id, page_id_t parent_id, int max_size) {
  SetPageType(IndexPageType::LEAF_PAGE);
  SetSize(0);
  SetParentPageId(parent_id);
  SetPageId(page_id);
  SetNextPageId(INVALID_PAGE_ID);
  SetMaxSize(max_size);
}

/**
 * Helper methods to set/get next page id
 */
INDEX_TEMPLATE_ARGUMENTS
page_id_t B_PLUS_TREE_LEAF_PAGE_TYPE::GetNextPageId() const { return next_page_id_; }

INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::SetNextPageId(page_id_t next_page_id) { next_page_id_ = next_page_id; }

/**
 * Helper method to find the first index i so that array[i].first >= key
 * NOTE: This method is only used when generating index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_LEAF_PAGE_TYPE::KeyIndex(const KeyType &key, const KeyComparator &comparator) const {
  // //使用二分查找，对时间进一步优化
  // int left=0;
  // int right=size_
  // keyType key_mid=KeyAt(mid);
  // while(left<right){
  //   int mid=(right-left)/2;
  //   key_mid=KeyAt(mid);
  //   if(comparator(key_mid,key)<0){
  //     left=mid;
  //   }
  //   else{
  //     right=mid;
  //   }
  //   int mid=(right-left)/2;
  // }
  // return right;

  //使用顺序查找
  for (int index = 0; index < GetSize(); index++) {
    if (comparator(array[index].first, key) >= 0) return index;
  }
  return GetSize();
}

/*
 * Helper method to find and return the key associated with input "index"(a.k.a
 * array offset)
 */
INDEX_TEMPLATE_ARGUMENTS
KeyType B_PLUS_TREE_LEAF_PAGE_TYPE::KeyAt(int index) const {
  // replace with your own code
  KeyType key = array[index].first;
  return key;
}

/*
 * Helper method to find and return the key & value pair associated with input
 * "index"(a.k.a array offset)
 */
INDEX_TEMPLATE_ARGUMENTS
const MappingType &B_PLUS_TREE_LEAF_PAGE_TYPE::GetItem(int index) {
  // replace with your own code
  return array[index];
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/*
 * Insert key & value pair into leaf page ordered by key
 * @return  page size after insertion
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_LEAF_PAGE_TYPE::Insert(const KeyType &key, const ValueType &value, const KeyComparator &comparator) {
  //在调用insert之前做了判断，保证插入的key是唯一的
  //找到恰好比待插入的key更大的Key'的位置，就是插入后key所在的位置
  auto index = KeyIndex(key,comparator);
  //插入键值对，对其后的所有键进行移位，保证有序
  for (int i = GetSize()-1; i >= index; i--) {
    array[i + 1] = array[i];
  }
  array[index].first = key;
  array[index].second = value;
  // size加1并返回
  this->IncreaseSize(1);
  return GetSize();
}

/*****************************************************************************
 * SPLIT
 *****************************************************************************/
/*
 * Remove half of key & value pairs from this page to "recipient" page
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::MoveHalfTo(BPlusTreeLeafPage *recipient) {
  //叶子节点分裂的时候调用
  auto num = GetSize() - GetMinSize();
  //此处是recipient将元素copy到自己,recipient从原节点（this）的num开始，复制剩下一半的页
//  recipient->CopyNFrom(&(this->array[num]), num);
//  recipient->CopyNFrom(&(array[GetMinSize()]), num);
  recipient->CopyNFrom(&array[GetMinSize()], num);
  this->SetSize( GetMinSize());
  recipient->SetSize(num);
}

/*
 * Copy starting from items, and copy {size} number of elements into me.
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::CopyNFrom(MappingType *items, int size) {
  for (int i = 0; i < size; i++) {
    // array[size_ + i] = items[i];
    array[i+this->GetSize()]=*(items+i);
  }
}

/*****************************************************************************
 * LOOKUP
 *****************************************************************************/
/*
 * For the given key, check to see whether it exists in the leaf page. If it
 * does, then store its corresponding value in input "value" and return true.
 * If the key does not exist, then return false
 */
INDEX_TEMPLATE_ARGUMENTS
bool B_PLUS_TREE_LEAF_PAGE_TYPE::Lookup(const KeyType &key, ValueType *value, const KeyComparator &comparator) const {
  //找有序数组中大于等于key的第一个index
  auto index = KeyIndex(key,comparator);
  // 1.如果index所属的key==key,查找成功，返回true;
  if (comparator(KeyAt(index), key) == 0) {
    *value = array[index].second;
    return true;
  }
  // 2. 查找不成功，返回false
  value = nullptr;
  return false;
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/*
 * First look through leaf page to see whether delete key exist or not. If
 * exist, perform deletion, otherwise return immediately.
 * NOTE: store key&value pair continuously after deletion
 * @return   page size after deletion
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_LEAF_PAGE_TYPE::RemoveAndDeleteRecord(const KeyType &key, const KeyComparator &comparator) {
  //找有序数组中大于等于key的第一个index
  auto index = KeyIndex(key,comparator);
  // 1.如果index所属的key==key,key存在，做删除操作;
  if (comparator(KeyAt(index), key) == 0) {
    //1.2其他情况，依次将数组向前移动一位
    for (int i = index; i < GetSize()-1; i++) {
      array[i] = array[i+1];
    }
    this->IncreaseSize(-1);
  }
  // 2. key不存在，删除失败
  //返回删除后的size
  return GetSize();
}

/*****************************************************************************
 * MERGE
 *****************************************************************************/
/*
 * Remove all of key & value pairs from this page to "recipient" page. Don't forget
 * to update the next_page id in the sibling page
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::MoveAllTo(BPlusTreeLeafPage *recipient) {
  //lsibling和node合并，node全部元素挪至lsibling并即将删除本节点。
  //为了避免找前驱的情况，总是将右边的元素挪到左边
  recipient->CopyNFrom(&(this->array[0]),this->GetSize());
//
//  //recipient向后挪动size位
//  for(int i=this->GetSize()+recipient->GetSize()-1;i>this->GetSize();i--){
//    recipient->array[i]=recipient->array[i-this->GetSize()];
//  }
//  //recipient的前size位从本节点中复制补全
//  for(int i=0;i<this->GetSize()-1;i++){
//    recipient->array[i]=this->array[i];
//  }
  //修改大小
  this->SetSize(0);
  recipient->IncreaseSize(this->GetSize());
  recipient->SetNextPageId(this->GetNextPageId());
}

/*****************************************************************************
 * REDISTRIBUTE
 *****************************************************************************/
/*
 * Remove the first key & value pair from this page to "recipient" page.
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::MoveFirstToEndOf(BPlusTreeLeafPage *recipient) {
  recipient->CopyLastFrom(this->array[0]);
  for (int i = 0; i <GetSize(); i++) {
    array[i] = array[i+1];
  }
  this->IncreaseSize(-1);
}

/*
 * Copy the item into the end of my item list. (Append item to my array)
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::CopyLastFrom(const MappingType &item) {
  //将item插入所有元素之后，不用移位
  array[GetSize()]=item;
  this->IncreaseSize(1);
}

/*
 * Remove the last key & value pair from this page to "recipient" page.
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::MoveLastToFrontOf(BPlusTreeLeafPage *recipient) {
    recipient->CopyFirstFrom(this->array[GetSize()-1]);
    this->IncreaseSize(-1);
}

/*
 * Insert item at the front of my items. Move items accordingly.
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::CopyFirstFrom(const MappingType &item) {
  //将item插入所有元素之前，对其后的所有键进行移位，保证有序
  for (int i = GetSize()-1; i >0; i--) {
    array[i + 1] = array[i];
  }
  array[0]=item;
  this->IncreaseSize(1);
}

template class BPlusTreeLeafPage<GenericKey<4>, RID, GenericComparator<4>>;
template class BPlusTreeLeafPage<GenericKey<8>, RID, GenericComparator<8>>;
template class BPlusTreeLeafPage<GenericKey<16>, RID, GenericComparator<16>>;
template class BPlusTreeLeafPage<GenericKey<32>, RID, GenericComparator<32>>;
template class BPlusTreeLeafPage<GenericKey<64>, RID, GenericComparator<64>>;
}  // namespace bustub
