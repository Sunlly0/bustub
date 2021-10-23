//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/index/b_plus_tree.cpp
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <string>

#include "common/exception.h"
#include "common/rid.h"
#include "storage/index/b_plus_tree.h"
#include "storage/page/header_page.h"

namespace bustub {
INDEX_TEMPLATE_ARGUMENTS
BPLUSTREE_TYPE::BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager, const KeyComparator &comparator,
                          int leaf_max_size, int internal_max_size)
    : index_name_(std::move(name)),
      root_page_id_(INVALID_PAGE_ID),
      buffer_pool_manager_(buffer_pool_manager),
      comparator_(comparator),
      leaf_max_size_(leaf_max_size),
      internal_max_size_(internal_max_size) {}

/*
 * Helper function to decide whether current b+tree is empty
 */
INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::IsEmpty() const {
  //为空的两种情况：没有根节点、根节点为空
  if (root_page_id_ == INVALID_PAGE_ID) return true;
//  BPlusTreePage *rootpage = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(root_page_id_));
//  if (rootpage->GetSize() == 0) {
//    buffer_pool_manager_->UnpinPage(root_page_id_, false);
//    return true;
//  }
  return false;
}
/*****************************************************************************
 * SEARCH
 *****************************************************************************/
/*
 * Return the only value that associated with input key
 * This method is used for point query
 * @return : true means key exists
 */
INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::GetValue(const KeyType &key, std::vector<ValueType> *result, Transaction *transaction) {
  transaction = nullptr;
  // 1. 树不为空
  if (!IsEmpty()) {
    //初始化为根节点
    page_id_t page_id = root_page_id_;
    BPlusTreePage *page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(page_id));
    //如果是中间节点，则向下找含有该key的叶子节点
    while (!page->IsLeafPage()) {
      InternalPage *inner = reinterpret_cast<InternalPage *>(page);
      auto cpage_id = inner->Lookup(key, comparator_);
      buffer_pool_manager_->UnpinPage(page_id, false);

      page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(cpage_id));
      page_id = cpage_id;
    }
    LeafPage *leaf = reinterpret_cast<LeafPage *>(page);
    //找到叶子节点，则调用Lookup函数，返回查找的bool结果
    ValueType *v=nullptr;
    auto isexist = leaf->Lookup(key, v, comparator_);
    result->push_back(*v);
    buffer_pool_manager_->UnpinPage(page_id, false);

    return isexist;
    // auto index = page->KeyIndex(key, comparator_);
    // // key存在则返回true，返回结果的引用
    // if (KeyAt(index) == key) {
    //   result = GetItem(index).second;
    //   return true;
    // }
  }
  // 2，树为空，返回false
  result = nullptr;
  return false;
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/*
 * Insert constant key & value pair into b+ tree
 * if current tree is empty, start new tree, update root page id and insert
 * entry, otherwise insert into leaf page.
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false, otherwise return true.
 */
INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::Insert(const KeyType &key, const ValueType &value, Transaction *transaction) {
  transaction = nullptr;
  if (IsEmpty()) {
    StartNewTree(key, value);
    return true;
  }
  return InsertIntoLeaf(key, value);
}
/*
 * Insert constant key & value pair into an empty tree
 * User needs to first ask for new page from buffer pool manager(NOTICE: throw
 * an "out of memory" exception if returned value is nullptr), then update b+
 * tree's root page id and insert entry directly into leaf page.
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::StartNewTree(const KeyType &key, const ValueType &value) {
  LeafPage *newpage = reinterpret_cast<LeafPage *>(buffer_pool_manager_->NewPage(&root_page_id_));
  // 1.如果无法new一个page，则内存空间已满
  if (newpage == nullptr) throw Exception(ExceptionType::OUT_OF_MEMORY, "Out of memory");
  // 2. newpage成功, 按根节点初始化该页
  newpage->Init(root_page_id_);
  //更新tree的头文件信息中的root_page_id
  UpdateRootPageId(1);
  newpage->Insert(key, value, comparator_);
  buffer_pool_manager_->UnpinPage(root_page_id_, true);
}

/*
 * Insert constant key & value pair into leaf page
 * User needs to first find the right leaf page as insertion target, then look
 * through leaf page to see whether insert key exist or not. If exist, return
 * immdiately, otherwise insert entry. Remember to deal with split if necessary.
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false, otherwise return true.
 */
INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::InsertIntoLeaf(const KeyType &key, const ValueType &value, Transaction *transaction) {
  transaction = nullptr;
  //初始化为根节点
  page_id_t page_id = root_page_id_;
  BPlusTreePage *page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(page_id));
  // if (page->IsLeafPage())
  //   page = reinterpret_cast<LeafPage *>(page);
  // else
  //   page = reinterpret_cast<InternalPage *>(page);
  //如果是中间节点，则向下找含有该key的叶子节点
  while (!page->IsLeafPage()) {
    InternalPage *inner = reinterpret_cast<InternalPage *>(page);
    auto cpage_id = inner->Lookup(key, comparator_);
    buffer_pool_manager_->UnpinPage(page_id, false);
    page_id = cpage_id;
    page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(page_id));
  }
  LeafPage *leaf = reinterpret_cast<LeafPage *>(page);
  //如果是叶子节点，则判断key是否已经存在
  ValueType *v=nullptr;
  bool isexist=leaf->Lookup(key,v,comparator_);
  // LeafPage *leaf = reinterpret_cast<LeafPage *>(page);
  // auto index = leaf->KeyIndex(key, comparator_);
  // KeyType keynow = leaf->KeyAt(index);
  // bool isexist = (comparator_(keynow, key) == 0);
  // 1. 如果key不存在，则插入
  if (!isexist) {
    auto size = leaf->Insert(key, value, comparator_);
    // 2.1 如果插入之后没有超过叶子页的上限，插入结束，返回true
    if (size > leaf_max_size_) {
      // 2.2 如果超过上限，则进行分裂。分裂后将新增加的页和关键词插入父节点（中间节点）
      auto newpage = this->Split(leaf);
      InsertIntoParent(leaf, newpage->KeyAt(0), newpage);
    }
    buffer_pool_manager_->UnpinPage(page_id, true);
    return true;
  }
  // 2.key存在，由于key的唯一性，插入失败，则返回false
  buffer_pool_manager_->UnpinPage(page_id, false);
  return false;
}

/*
 * Split input page and return newly created page.
 * Using template N to represent either internal page or leaf page.
 * User needs to first ask for new page from buffer pool manager(NOTICE: throw
 * an "out of memory" exception if returned value is nullptr), then move half
 * of key & value pairs from input page to newly created page
 */
INDEX_TEMPLATE_ARGUMENTS
template <typename N>
N *BPLUSTREE_TYPE::Split(N *node) {
  page_id_t newpage_id;
  //新建一个页
  BPlusTreePage *newpage = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->NewPage(&newpage_id));
  if (newpage == nullptr) throw Exception(ExceptionType::OUT_OF_MEMORY, "Out of memory");
  N *newpagereturn;
  BPlusTreePage *page = reinterpret_cast<BPlusTreePage *>(node);
  // 1. 如果是叶子节点
  if (page->IsLeafPage()) {
    LeafPage *leaf = reinterpret_cast<LeafPage *>(page);
    LeafPage *newleaf = reinterpret_cast<LeafPage *>(newpage);
    //初始化新叶子页
    newleaf->Init(newpage_id,leaf->GetParentPageId(),leaf_max_size_);
    //原叶子页移动后一半元素到新页
    leaf->MoveHalfTo(newleaf);
    //设置叶子节点的双向链表
    newleaf->SetNextPageId(leaf->GetNextPageId());
    leaf->SetNextPageId(newpage_id);
    newleaf->SetPrePageId(leaf->GetPageId());
    //Q:增加向前的指针后，在设置双向链表的时候需要增加new后一个节点向前的设置？？
    LeafPage *after_leaf = reinterpret_cast<LeafPage *>(buffer_pool_manager_->FetchPage(newleaf->GetNextPageId()));
    after_leaf->SetPrePageId(newpage_id);
    buffer_pool_manager_->UnpinPage(after_leaf->GetPageId(),true);
    newpagereturn=reinterpret_cast<N *>(newleaf);
  }
  // 2. 如果是内部节点，索引改变后要做持久化操作
  else {
    InternalPage *inner = reinterpret_cast<InternalPage *>(page);
    InternalPage *newinner = reinterpret_cast<InternalPage *>(newpage);
    newinner->Init(newpage_id,inner->GetParentPageId(),internal_max_size_);
    inner->MoveHalfTo(newinner, buffer_pool_manager_);
    newpagereturn=reinterpret_cast<N *>(newinner);
  }
  buffer_pool_manager_->UnpinPage(newpage_id, true);
  return newpagereturn;

}

/*
 * Insert key & value pair into internal page after split
 * @param   old_node      input page from split() method
 * @param   key
 * @param   new_node      returned page from split() method
 * User needs to first find the parent page of old_node, parent node must be
 * adjusted to take info of new_node into account. Remember to deal with split
 * recursively if necessary.
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertIntoParent(BPlusTreePage *old_node, const KeyType &key, BPlusTreePage *new_node,
                                      Transaction *transaction) {
  transaction = nullptr;
  page_id_t ppage_id = old_node->GetParentPageId();
  // 1. 如果父节点为空，就说明分裂的节点已经是根节点。需要新建一个根节点
  // Q: 此处是否要调用bpm新取页？
  // A: 是的，并且新建的根节点一定是中间页
  if (old_node->IsRootPage()) {
    InternalPage *ppage = reinterpret_cast<InternalPage *>(buffer_pool_manager_->NewPage(&ppage_id));
    ppage->Init(ppage_id,INVALID_PAGE_ID,internal_max_size_);
    ppage->PopulateNewRoot(old_node->GetPageId(), key, new_node->GetPageId());
    //设置分裂新旧节点的父亲为ppage
    old_node->SetPageId(ppage_id);
    new_node->SetPageId(ppage_id);
    UpdateRootPageId(0);//跟新根节点id
    this->root_page_id_ = ppage_id;
  }
  // 2. 如果父节点不为空
  else {
    InternalPage *ppage = reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(ppage_id));
    new_node->SetPageId(ppage_id);
    int size = ppage->InsertNodeAfter(old_node->GetPageId(), key, new_node->GetPageId());
    // 2.1 插入后，如果父节点没有满，则结束
    // 2.2 插入后，如果父节点的size大于maxsize，则递归调用本函数
    if (size > internal_max_size_) {
      auto newppage = this->Split<InternalPage>(ppage);
      InsertIntoParent(ppage, newppage->KeyAt(0), newppage);
    }
  }
  buffer_pool_manager_->UnpinPage(ppage_id, true);
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/*
 * Delete key & value pair associated with input key
 * If current tree is empty, return immdiately.
 * If not, User needs to first find the right leaf page as deletion target, then
 * delete entry from leaf page. Remember to deal with redistribute or merge if
 * necessary.
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::Remove(const KeyType &key, Transaction *transaction) {
  transaction = nullptr;
  //1.如果树为空，删除失败。
  //2.如果树不为空：
  if (!IsEmpty()) {
    //初始化为根节点
    page_id_t page_id = root_page_id_;
    BPlusTreePage *page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(page_id));
    //如果是中间节点，则向下找含有该key的叶子节点
    while (!page->IsLeafPage()) {
      InternalPage *inner = reinterpret_cast<InternalPage *>(page);
      auto cpage_id = inner->Lookup(key, comparator_);
      buffer_pool_manager_->UnpinPage(page_id, false);
      page_id = cpage_id;
      page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(page_id));
    }

    //找到叶子节点，在叶子节点中删除key
    LeafPage *leaf = reinterpret_cast<LeafPage *>(page);
    int pre_size=leaf->GetSize();
    int size=leaf->RemoveAndDeleteRecord(key,comparator_);

    //2.1 如果size变化，删除成功。
    if(pre_size!=size){
      //更新父节点的key值，保证和删除key后的子节点一致
      InternalPage *ppage = reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(leaf->GetParentPageId()));
      auto index=ppage->ValueIndex(page_id);
      ppage->SetKeyAt(index,leaf->KeyAt(0));
      //对删除后叶子节点的size进行判断
      //2.1.1. 删除后，叶子节点的size不满足最小值，向兄弟节点借元素或合并
      if(size<leaf->GetMinSize()){
        CoalesceOrRedistribute(leaf);
      }
      //2.1.2.满足条件，删除结束
    }
    //2.2 如果size没变，说明删除失败。
 }
}

/*
 * User needs to first find the sibling of input page. If sibling's size + input
 * page's size > page's max size, then redistribute. Otherwise, merge.
 * Using template N to represent either internal page or leaf page.
 * @return: true means target leaf page should be deleted, false means no
 * deletion happens
 */
INDEX_TEMPLATE_ARGUMENTS
template <typename N>
bool BPLUSTREE_TYPE::CoalesceOrRedistribute(N *node, Transaction *transaction) {
  transaction= nullptr;
  if(node->IsRootPage()){
    return AdjustRoot(node);
  }
  //叶子节点
  if(node->IsLeafPage()){
    LeafPage *leaf = reinterpret_cast<LeafPage *>(node);
    auto rleaf_page_id=leaf->GetNextPageId();
    auto lleaf_page_id=leaf->GetPrePageId();
    LeafPage *lsibling = reinterpret_cast<LeafPage *>(buffer_pool_manager_->FetchPage(lleaf_page_id));
    LeafPage *rsibling = reinterpret_cast<LeafPage *>(buffer_pool_manager_->FetchPage(rleaf_page_id));
    //1.判断是否可以从左边兄弟借元素
    if(lsibling->GetSize()+leaf->GetSize()>leaf_max_size_) {
      Redistribute(lsibling, leaf, 1);
    }
    //2.左兄弟不能借，判断能否从右边兄弟借元素
    else if(rsibling->GetSize()+leaf->GetSize()>leaf_max_size_) {
      Redistribute(rsibling, leaf, 0);
    }
    //3. 左右两边都不能借元素，做合并操作
    else{
      InternalPage *ppage = reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(leaf->GetParentPageId()));
      auto index=ppage->ValueAt(node->GetPageId());
      Coalesce(&rsibling,&leaf,&ppage,index);
      return true;
    }
  }
  //内部节点
  else{
    InternalPage *inner = reinterpret_cast<InternalPage *>(node);
    InternalPage *ppage = reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(inner->GetParentPageId()));
    auto index=ppage->ValueIndex(inner->GetPageId());
    auto lpage_id=ppage->ValueAt(index-1);
    auto rpage_id=ppage->ValueAt(index+1);
    InternalPage *lpage = reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(lpage_id));
    InternalPage *rpage=reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(rpage_id));
    //1.判断是否可以从左边兄弟借元素
    if(lpage->GetSize()+inner->GetSize()>internal_max_size_) {
      Redistribute(lpage, inner, 1);
    }
    //2.左兄弟不能借，判断能否从右边兄弟借元素
    else if(rpage->GetSize()+inner->GetSize()>leaf_max_size_) {
      Redistribute(rpage, inner, 0);
    }
    //3. 左右两边都不能借元素，做合并操作
    else{
      Coalesce(&rpage,&inner,&ppage,index);
      return true;
    }
  }
  return false;
}

/*
 * Move all the key & value pairs from one page to its sibling page, and notify
 * buffer pool manager to delete this page. Parent page must be adjusted to
 * take info of deletion into account. Remember to deal with coalesce or
 * redistribute recursively if necessary.
 * Using template N to represent either internal page or leaf page.
 * @param   neighbor_node      sibling page of input "node"
 * @param   node               input from method coalesceOrRedistribute()
 * @param   parent             parent page of input "node"
 * @return  true means parent node should be deleted, false means no deletion
 * happend
 */
INDEX_TEMPLATE_ARGUMENTS
template <typename N>
bool BPLUSTREE_TYPE::Coalesce(N **neighbor_node, N **node,
                              BPlusTreeInternalPage<KeyType, page_id_t, KeyComparator> **parent, int index,
                              Transaction *transaction) {
  transaction= nullptr;
  //1. 如果是叶子节点，调用叶子节点的moveallto,将本节点的全部元素移动给兄弟节点
  if((*node)->IsLeafPage()){
    LeafPage *leaf = reinterpret_cast<LeafPage *>(*node);
    LeafPage *n_leaf = reinterpret_cast<LeafPage *>(*neighbor_node);
    leaf->MoveAllTo(n_leaf);
  }
  //2.如果是内部节点，调用内部节点的moveallto，对被移动的子节点做索引持久化
  else{
    InternalPage *inner = reinterpret_cast<InternalPage *>(*node);
    InternalPage *n_inner = reinterpret_cast<InternalPage *>(*neighbor_node);
    inner->MoveAllTo(n_inner,n_inner->KeyAt(0),buffer_pool_manager_);
  }
  buffer_pool_manager_->DeletePage((*node)->GetPageId());
  //父亲节点删除node页面
  (*parent)->Remove(index);
  //1. 如果删除后，父节点size不满足最小条件，父节点借元素或合并
  if((*parent)->GetSize()<(*parent)->GetMinSize()){
    return CoalesceOrRedistribute(*parent);
  }
  //2. 如果满足最小值条件，删除结束
  return false;
}

/*
 * Redistribute key & value pairs from one page to its sibling page. If index ==
 * 0, move sibling page's first key & value pair into end of input "node",
 * otherwise move sibling page's last key & value pair into head of input
 * "node".
 * Using template N to represent either internal page or leaf page.
 * @param   neighbor_node      sibling page of input "node"
 * @param   node               input from method coalesceOrRedistribute()
 */
INDEX_TEMPLATE_ARGUMENTS
template <typename N>
void BPLUSTREE_TYPE::Redistribute(N *neighbor_node, N *node, int index) {
  //node是叶子节点
  if(node->IsLeafPage()) {
    LeafPage *leaf = reinterpret_cast<LeafPage *>(node);
    LeafPage *n_leaf = reinterpret_cast<LeafPage *>(neighbor_node);
    if (index == 0) {
      // neigbor_node在右边，将头元素借给node
      n_leaf->MoveFirstToEndOf(leaf);
    } else {
      // neigbor_node在左边，将末尾的元素借给node
      n_leaf->MoveLastToFrontOf(leaf);
    }
  }
  //node是内部节点
  else{
    InternalPage *inner = reinterpret_cast<InternalPage *>(node);
    InternalPage *n_inner = reinterpret_cast<InternalPage *>(neighbor_node);
//    InternalPage *ppage = reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(n_inner->GetParentPageId()));
    if (index == 0) {
      //Q: 是否可以认为n_inner的首元素没有使用的key和父节点中的索引key保持一致？
      // neigbor_node在右边，将头元素借给node
      n_inner->MoveFirstToEndOf(inner,n_inner->KeyAt(0),buffer_pool_manager_);
    } else {
      // neigbor_node在左边，将末尾的元素借给node
      n_inner->MoveLastToFrontOf(inner, n_inner->KeyAt(0), buffer_pool_manager_);
    }
  }
}
/*
 * Update root page if necessary
 * NOTE: size of root page can be less than min size and this method is only
 * called within coalesceOrRedistribute() method
 * case 1: when you delete the last element in root page, but root page still
 * has one last child
 * case 2: when you delete the last element in whole b+ tree
 * @return : true means root page should be deleted, false means no deletion
 * happend
 */
INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::AdjustRoot(BPlusTreePage *old_root_node) {
  //对根不满足最小值条件时做判断
  //1. 如果根的size>1，不做删除操作
  //2. 如果根还有子节点，将子节点作为新的根节点，该节点删除

  if(old_root_node->GetSize()==1){
    InternalPage *root_inner = reinterpret_cast<InternalPage *>(old_root_node);
    BPlusTreePage *cpage=reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(root_inner->ValueAt(0)));
    cpage->SetParentPageId(INVALID_PAGE_ID);
    root_page_id_=cpage->GetPageId();
    buffer_pool_manager_->UnpinPage(cpage->GetParentPageId(),true);
    buffer_pool_manager_->FlushPage(cpage->GetParentPageId());
    return true;
  }
  //3. 如果根没有子节点，整个树至为空
  else if(old_root_node->GetSize()==0){
    root_page_id_=INVALID_PAGE_ID;
    return true;
  }
  return false;
}

/*****************************************************************************
 * INDEX ITERATOR
 *****************************************************************************/
/*
 * Input parameter is void, find the leaftmost leaf page first, then construct
 * index iterator
 * @return : index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE BPLUSTREE_TYPE::begin() {
  Page *page= FindLeafPage(KeyType{},true);
  LeafPage *leftmost_leaf=reinterpret_cast<LeafPage *>(page);
  buffer_pool_manager_->UnpinPage(leftmost_leaf->GetPageId(), false);
  return INDEXITERATOR_TYPE(leftmost_leaf,0,buffer_pool_manager_); }

/*
 * Input parameter is low key, find the leaf page that contains the input key
 * first, then construct index iterator
 * @return : index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE BPLUSTREE_TYPE::Begin(const KeyType &key) {
  Page *page= FindLeafPage(key, false);
  LeafPage *leaf=reinterpret_cast<LeafPage *>(page);
  auto index=leaf->KeyIndex(key,comparator_);
  buffer_pool_manager_->UnpinPage(leaf->GetPageId(), false);
  return INDEXITERATOR_TYPE(leaf,index,buffer_pool_manager_); }

/*
 * Input parameter is void, construct an index iterator representing the end
 * of the key/value pair in the leaf node
 * @return : index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE BPLUSTREE_TYPE::end() {
  Page *page= FindLeafPage(KeyType(), true);
  LeafPage *leaf=reinterpret_cast<LeafPage *>(page);
  while(leaf->GetNextPageId()!=INVALID_PAGE_ID){
    LeafPage *next_leaf=reinterpret_cast<LeafPage *>(buffer_pool_manager_->FetchPage(leaf->GetNextPageId()));
    buffer_pool_manager_->UnpinPage(leaf->GetPageId(), false);
    leaf=next_leaf;
  }
  buffer_pool_manager_->UnpinPage(leaf->GetPageId(), false);
  return INDEXITERATOR_TYPE(leaf,leaf->GetSize(),buffer_pool_manager_);;
}

/*****************************************************************************
 * UTILITIES AND DEBUG
 *****************************************************************************/
/*
 * Find leaf page containing particular key, if leftMost flag == true, find
 * the left most leaf page
 */
INDEX_TEMPLATE_ARGUMENTS
Page *BPLUSTREE_TYPE::FindLeafPage(const KeyType &key, bool leftMost) {
  //初始化为根节点
  page_id_t page_id = root_page_id_;
  BPlusTreePage *page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(page_id));
  //如果是中间节点，则向下找含有该key的叶子节点
  while (!page->IsLeafPage()) {
    InternalPage *inner = reinterpret_cast<InternalPage *>(page);
    auto cpage_id = inner->Lookup(key, comparator_);
    buffer_pool_manager_->UnpinPage(page_id, false);
    page_id = cpage_id;
    page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(page_id));
  }
  return reinterpret_cast<Page *>(page);
  // throw Exception(ExceptionType::NOT_IMPLEMENTED, "Implement this for test");
}

/*
 * Update/Insert root page id in header page(where page_id = 0, header_page is
 * defined under include/page/header_page.h)
 * Call this method everytime root page id is changed.
 * @parameter: insert_record      defualt value is false. When set to true,
 * insert a record <index_name, root_page_id> into header page instead of
 * updating it.
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::UpdateRootPageId(int insert_record) {
  HeaderPage *header_page = static_cast<HeaderPage *>(buffer_pool_manager_->FetchPage(HEADER_PAGE_ID));
  if (insert_record != 0) {
    // create a new record<index_name + root_page_id> in header_page
    header_page->InsertRecord(index_name_, root_page_id_);
  } else {
    // update root_page_id in header_page
    header_page->UpdateRecord(index_name_, root_page_id_);
  }
  buffer_pool_manager_->UnpinPage(HEADER_PAGE_ID, true);
}

/*
 * This method is used for test only
 * Read data from file and insert one by one
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertFromFile(const std::string &file_name, Transaction *transaction) {
  int64_t key;
  std::ifstream input(file_name);
  while (input) {
    input >> key;

    KeyType index_key;
    index_key.SetFromInteger(key);
    RID rid(key);
    Insert(index_key, rid, transaction);
  }
}
/*
 * This method is used for test only
 * Read data from file and remove one by one
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::RemoveFromFile(const std::string &file_name, Transaction *transaction) {
  int64_t key;
  std::ifstream input(file_name);
  while (input) {
    input >> key;
    KeyType index_key;
    index_key.SetFromInteger(key);
    Remove(index_key, transaction);
  }
}

/**
 * This method is used for debug only, You don't  need to modify
 * @tparam KeyType
 * @tparam ValueType
 * @tparam KeyComparator
 * @param page
 * @param bpm
 * @param out
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::ToGraph(BPlusTreePage *page, BufferPoolManager *bpm, std::ofstream &out) const {
  std::string leaf_prefix("LEAF_");
  std::string internal_prefix("INT_");
  if (page->IsLeafPage()) {
    LeafPage *leaf = reinterpret_cast<LeafPage *>(page);
    // Print node name
    out << leaf_prefix << leaf->GetPageId();
    // Print node properties
    out << "[shape=plain color=green ";
    // Print data of the node
    out << "label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n";
    // Print data
    out << "<TR><TD COLSPAN=\"" << leaf->GetSize() << "\">P=" << leaf->GetPageId() << "</TD></TR>\n";
    out << "<TR><TD COLSPAN=\"" << leaf->GetSize() << "\">"
        << "max_size=" << leaf->GetMaxSize() << ",min_size=" << leaf->GetMinSize() << "</TD></TR>\n";
    out << "<TR>";
    for (int i = 0; i < leaf->GetSize(); i++) {
      out << "<TD>" << leaf->KeyAt(i) << "</TD>\n";
    }
    out << "</TR>";
    // Print table end
    out << "</TABLE>>];\n";
    // Print Leaf node link if there is a next page
    if (leaf->GetNextPageId() != INVALID_PAGE_ID) {
      out << leaf_prefix << leaf->GetPageId() << " -> " << leaf_prefix << leaf->GetNextPageId() << ";\n";
      out << "{rank=same " << leaf_prefix << leaf->GetPageId() << " " << leaf_prefix << leaf->GetNextPageId() << "};\n";
    }

    // Print parent links if there is a parent
    if (leaf->GetParentPageId() != INVALID_PAGE_ID) {
      out << internal_prefix << leaf->GetParentPageId() << ":p" << leaf->GetPageId() << " -> " << leaf_prefix
          << leaf->GetPageId() << ";\n";
    }
  } else {
    InternalPage *inner = reinterpret_cast<InternalPage *>(page);
    // Print node name
    out << internal_prefix << inner->GetPageId();
    // Print node properties
    out << "[shape=plain color=pink ";  // why not?
    // Print data of the node
    out << "label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n";
    // Print data
    out << "<TR><TD COLSPAN=\"" << inner->GetSize() << "\">P=" << inner->GetPageId() << "</TD></TR>\n";
    out << "<TR><TD COLSPAN=\"" << inner->GetSize() << "\">"
        << "max_size=" << inner->GetMaxSize() << ",min_size=" << inner->GetMinSize() << "</TD></TR>\n";
    out << "<TR>";
    for (int i = 0; i < inner->GetSize(); i++) {
      out << "<TD PORT=\"p" << inner->ValueAt(i) << "\">";
      if (i > 0) {
        out << inner->KeyAt(i);
      } else {
        out << " ";
      }
      out << "</TD>\n";
    }
    out << "</TR>";
    // Print table end
    out << "</TABLE>>];\n";
    // Print Parent link
    if (inner->GetParentPageId() != INVALID_PAGE_ID) {
      out << internal_prefix << inner->GetParentPageId() << ":p" << inner->GetPageId() << " -> " << internal_prefix
          << inner->GetPageId() << ";\n";
    }
    // Print leaves
    for (int i = 0; i < inner->GetSize(); i++) {
      auto child_page = reinterpret_cast<BPlusTreePage *>(bpm->FetchPage(inner->ValueAt(i))->GetData());
      ToGraph(child_page, bpm, out);
      if (i > 0) {
        auto sibling_page = reinterpret_cast<BPlusTreePage *>(bpm->FetchPage(inner->ValueAt(i - 1))->GetData());
        if (!sibling_page->IsLeafPage() && !child_page->IsLeafPage()) {
          out << "{rank=same " << internal_prefix << sibling_page->GetPageId() << " " << internal_prefix
              << child_page->GetPageId() << "};\n";
        }
        bpm->UnpinPage(sibling_page->GetPageId(), false);
      }
    }
  }
  bpm->UnpinPage(page->GetPageId(), false);
}

/**
 * This function is for debug only, you don't need to modify
 * @tparam KeyType
 * @tparam ValueType
 * @tparam KeyComparator
 * @param page
 * @param bpm
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::ToString(BPlusTreePage *page, BufferPoolManager *bpm) const {
  if (page->IsLeafPage()) {
    LeafPage *leaf = reinterpret_cast<LeafPage *>(page);
    std::cout << "Leaf Page: " << leaf->GetPageId() << " parent: " << leaf->GetParentPageId()
              << " next: " << leaf->GetNextPageId() << std::endl;
    for (int i = 0; i < leaf->GetSize(); i++) {
      std::cout << leaf->KeyAt(i) << ",";
    }
    std::cout << std::endl;
    std::cout << std::endl;
  } else {
    InternalPage *internal = reinterpret_cast<InternalPage *>(page);
    std::cout << "Internal Page: " << internal->GetPageId() << " parent: " << internal->GetParentPageId() << std::endl;
    for (int i = 0; i < internal->GetSize(); i++) {
      std::cout << internal->KeyAt(i) << ": " << internal->ValueAt(i) << ",";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < internal->GetSize(); i++) {
      ToString(reinterpret_cast<BPlusTreePage *>(bpm->FetchPage(internal->ValueAt(i))->GetData()), bpm);
    }
  }
  bpm->UnpinPage(page->GetPageId(), false);
}

template class BPlusTree<GenericKey<4>, RID, GenericComparator<4>>;
template class BPlusTree<GenericKey<8>, RID, GenericComparator<8>>;
template class BPlusTree<GenericKey<16>, RID, GenericComparator<16>>;
template class BPlusTree<GenericKey<32>, RID, GenericComparator<32>>;
template class BPlusTree<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
