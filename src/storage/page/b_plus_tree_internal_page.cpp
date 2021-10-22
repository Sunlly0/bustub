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
  SetPageType(IndexPageType::INTERNAL_PAGE);
  SetSize(0);
  SetMaxSize(max_size);
  SetParentPageId(parent_id);
  SetPageId(page_id);
}
/*
 * Helper method to get/set the key associated with input "index"(a.k.a
 * array offset)
 */
INDEX_TEMPLATE_ARGUMENTS
KeyType B_PLUS_TREE_INTERNAL_PAGE_TYPE::KeyAt(int index) const {
  // replace with your own code
  // Q:是否可能产生数组越界？ by Sunlly0
  // A:使用C语言风格的数组，可以越界访问数组
  //可以不考虑超过size和max_size的问题，因为是辅助函数
  // KeyType key{};
  KeyType key = array[index].first;
  return key;
}

INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::SetKeyAt(int index, const KeyType &key) {
  // Q:key变化之后，value是否变化？
  // A:不会，指向的页面不变，只有关键字变化
  array[index].first = key;
}

/*
 * Helper method to find and return array index(or offset), so that its value
 * equals to input "value"
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_INTERNAL_PAGE_TYPE::ValueIndex(const ValueType &value) const {
  for (int i = 0; i < GetSize(); i++) {
    ValueType v = array[i].second;
    if (v == value) return i;
  }
  return 0;
}

/*
 * Helper method to get the value associated with input "index"(a.k.a array
 * offset)
 */
INDEX_TEMPLATE_ARGUMENTS
ValueType B_PLUS_TREE_INTERNAL_PAGE_TYPE::ValueAt(int index) const {
  ValueType value = array[index].second;
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
  for (int index = 1; index < GetSize(); index++) {
    auto pagekey = KeyAt(index);
    // 1. 找到恰好比key大的值后，进入该值左边的分支
    if (comparator(pagekey, key) > 0) return ValueAt(index - 1);
  }
  // 2. 遍历完也没有找到比key大的值，则进入最右边的分支
  return ValueAt(GetSize() - 1);
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
  array[0].second=old_value;
  array[1].first=new_key;
  array[1].second=new_value;
  this->SetSize(2);
}
/*
 * Insert new_key & new_value pair right after the pair with its value ==
 * old_value
 * @return:  new size after insertion
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_INTERNAL_PAGE_TYPE::InsertNodeAfter(const ValueType &old_value, const KeyType &new_key,
                                                    const ValueType &new_value) {
  //在子节点分裂，将新的节点page_id插入父节点时调用 
  //找到old_node_id页位置，紧挨其后插入key和new_node_id                                                 
  auto index = ValueIndex(old_value);
  //插入键值对，对其后的所有键进行移位，保证有序
  for (int i = this->GetSize(); i > index + 1; i--) {
    array[i + 1] = array[i];
  }
  array[index].first = new_key;
  array[index].second = new_value;
  this->IncreaseSize(1);
  // Q:如果插入后size==max_size_，是否要调用分裂函数？
  // A:树的实现过程会考虑，本函数仅做插入
  return this->GetSize();
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
  //内部节点分裂时调用，recipient是新建的内部页
  int index = this->GetMinSize();
  //recipient从原节点（this）的index+1开始，复制剩下一半的页
  //Q: 这里的数量，有点混淆。对于内部节点，minsize到底是啥？
  recipient->CopyNFrom(&(this->array[index+1]),this->GetSize()-index,buffer_pool_manager);
  // KeyType newkey = KeyAt(index);
  // ValueType newvalue = ValueAt(index);
  // for (int i = index; i < size_; i++) {
  //   recipient->array{i - index} = array{index};
  //   // array{index}=0;
  // }

  recipient->SetSize(this->GetSize()- index);
  this->SetSize(index);
}

/* Copy entries into me, starting from {items} and copy {size} entries.
 * Since it is an internal page, for all entries (pages) moved, their parents page now changes to me.
 * So I need to 'adopt' them by changing their parent page id, which needs to be persisted with BufferPoolManger
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::CopyNFrom(MappingType *items, int size, BufferPoolManager *buffer_pool_manager) {
  for (int i = 0; i < size; i++) {
    // array[size_ + i] = items;
    array[i] = *items;
    //索引改变后，要做持久化
    //子节点的父亲节点改变
    auto cpage=reinterpret_cast<B_PLUS_TREE_INTERNAL_PAGE_TYPE *>(buffer_pool_manager->FetchPage(items->second));
    cpage->SetParentPageId(this->GetPageId());
     buffer_pool_manager->UnpinPage(items->second,true);
     buffer_pool_manager->FlushPage(items->second);
    items++;
  }
}

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
   for (int i = index; i < GetSize(); i++) {
     //从后往前移位
     array[i] = array[i + 1];
   }
   //Q: 是否需要将删除位清空？
   IncreaseSize(-1);
}

/*
 * Remove the only key & value pair in internal page and return the value
 * NOTE: only call this method within AdjustRoot()(in b_plus_tree.cpp)
 */
INDEX_TEMPLATE_ARGUMENTS
ValueType B_PLUS_TREE_INTERNAL_PAGE_TYPE::RemoveAndReturnOnlyChild() {
  //只有一个键值对的时候（没有key，只有value）删除该页面
  // if (GetSize() == 1) {
  //   array[size_] = 0;
  //   return ValueAt(0);
  // }
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
  for(int i=this->GetSize()+recipient->GetSize()-1;i>this->GetSize();i--){
    recipient->array[i]=recipient->array[i-this->GetSize()];
  }
  //recipient的前size位从本节点中复制补全
  for(int i=0;i<this->GetSize()-1;i++){
    recipient->array[i]=this->array[i];
  }

  //增加middle_key
  recipient->SetKeyAt(this->GetSize(),middle_key);
  //对于待删除节点的子节点，均修改其父亲节点指针为recipient。并且写出到磁盘做持久化
  for(int i=0;i<this->GetSize();i++){
    BPlusTreePage *cpage=reinterpret_cast<BPlusTreePage *>(buffer_pool_manager->FetchPage(this->ValueAt(i)));
    cpage->SetParentPageId(recipient->GetPageId());
    buffer_pool_manager->UnpinPage(cpage->GetParentPageId(),true);
    buffer_pool_manager->FlushPage(cpage->GetParentPageId());
  }
  //修改大小
  this->SetSize(0);
  recipient->IncreaseSize(this->GetSize());
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
  //自己是右兄弟节点，将自己首元素借给左边节点末尾
  recipient->CopyLastFrom(this->array[0],buffer_pool_manager);
  for (int i = 0; i <GetSize(); i++) {
    array[i] = array[i+1];
  }
  this->IncreaseSize(-1);
  recipient->SetKeyAt(recipient->GetSize()-1,middle_key);
  //更新父节点中对应的middle_key并且写出到磁盘
  BPlusTreeInternalPage *ppage = reinterpret_cast<BPlusTreeInternalPage *>(buffer_pool_manager->FetchPage(this->GetParentPageId()));
  ppage->SetKeyAt(ValueIndex(this->GetPageId()),this->KeyAt(0));
  buffer_pool_manager->UnpinPage(this->GetParentPageId(),true);
  buffer_pool_manager->FlushPage(this->GetParentPageId());
}

/* Append an entry at the end.
 * Since it is an internal page, the moved entry(page)'s parent needs to be updated.
 * So I need to 'adopt' it by changing its parent page id, which needs to be persisted with BufferPoolManger
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::CopyLastFrom(const MappingType &pair, BufferPoolManager *buffer_pool_manager) {
  //将item插入所有元素之后，不用移位
  array[GetSize()]=pair;
  this->IncreaseSize(1);
  //传入键值对的子节点的父亲节点改变，做持久化
  BPlusTreePage *cpage=reinterpret_cast<BPlusTreePage *>(buffer_pool_manager->FetchPage(pair.second));
  cpage->SetParentPageId(this->GetPageId());
  buffer_pool_manager->UnpinPage(pair.second,true);
  buffer_pool_manager->FlushPage(pair.second);
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
  //自己是左兄弟节点，将自己末尾元素借给右边节点首
  recipient->CopyFirstFrom(this->array[GetSize()-1],buffer_pool_manager);
  this->IncreaseSize(-1);
  recipient->SetKeyAt(1,middle_key);
  //更新父节点中对应的middle_key并且写出到磁盘
  BPlusTreeInternalPage *ppage = reinterpret_cast<BPlusTreeInternalPage *>(buffer_pool_manager->FetchPage(recipient->GetParentPageId()));
  ppage->SetKeyAt(ValueIndex(recipient->GetPageId()),recipient->KeyAt(0));
  buffer_pool_manager->UnpinPage(recipient->GetParentPageId(),true);
  buffer_pool_manager->FlushPage(recipient->GetParentPageId());
}

/* Append an entry at the beginning.
 * Since it is an internal page, the moved entry(page)'s parent needs to be updated.
 * So I need to 'adopt' it by changing its parent page id, which needs to be persisted with BufferPoolManger
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::CopyFirstFrom(const MappingType &pair, BufferPoolManager *buffer_pool_manager) {
  //将item插入所有元素之后，不用移位
  for (int i = GetSize()-1; i >0; i--) {
    array[i + 1] = array[i];
  }
  array[0]=pair;
  this->IncreaseSize(1);
  //传入键值对的子节点的父亲节点改变，做持久化
  BPlusTreePage *cpage=reinterpret_cast<BPlusTreePage *>(buffer_pool_manager->FetchPage(pair.second));
  cpage->SetParentPageId(this->GetPageId());
  buffer_pool_manager->UnpinPage(pair.second,true);
  buffer_pool_manager->FlushPage(pair.second);
}

// valuetype for internalNode should be page id_t
template class BPlusTreeInternalPage<GenericKey<4>, page_id_t, GenericComparator<4>>;
template class BPlusTreeInternalPage<GenericKey<8>, page_id_t, GenericComparator<8>>;
template class BPlusTreeInternalPage<GenericKey<16>, page_id_t, GenericComparator<16>>;
template class BPlusTreeInternalPage<GenericKey<32>, page_id_t, GenericComparator<32>>;
template class BPlusTreeInternalPage<GenericKey<64>, page_id_t, GenericComparator<64>>;
}  // namespace bustub
