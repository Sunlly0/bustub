//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"

#include <list>
#include <unordered_map>

namespace bustub {

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
  // We allocate a consecutive memory space for the buffer pool.
  pages_ = new Page[pool_size_];
  replacer_ = new LRUReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete replacer_;
}

bool BufferPoolManager::ChooseFromFreeListOrLRU(page_id_t *page_id_replace, frame_id_t *frame_id_replace){
  Page *page_replace;
  if(!free_list_.empty()){
    frame_id_replace=&free_list_.front();
    free_list_.pop_front();
  }
  else{
    //when choose a victim, should it flush to disk first? by Sunlly0
    if(!replacer_->Victim(frame_id_replace)){
        return nullptr;
    }
    else{
      page_replace=pages_+ *frame_id_replace;
      *page_id_replace=page_replace->GetPageId();
      FlushPageImpl(*page_id_replace);
    }
  }

} 

Page *BufferPoolManager::GetPageFromList(page_id_t page_id, frame_id_t *frame_id=nullptr){
  // Page *page_index;
  // page_index=pages_;
  // for(int i=0;i<pool_size_;page_index++){
  //   if(page_index->GetPageId()==page_id){
  //     return page_index;
  //   }
  // }

  //find in page_table_ first!
  auto page_index=page_table_.find(page_id);
  if(page_index!=page_table_.end()){
    // IF page_id is in page_table_
    frame_id=&page_table_.at(page_id);

    return (pages_+ page_table_.at(page_id));
  }
  return nullptr;
}

Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
  // 1.     Search the page table for the requested page (P).
  frame_id_t frame_id;
  Page *page_index=GetPageFromList(page_id, &frame_id);

  // 1.1    If P exists, pin it and return it immediately.
  if(!page_index){
    replacer_->Pin(frame_id);
    return page_index;
  }
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
   // 2.     If R is dirty, write it back to the disk.
  else{
      frame_id_t frame_id_replace;
      page_id_t page_id_replace;
      if(ChooseFromFreeListOrLRU(&page_id_replace,&frame_id_replace)){
            // 3.     Delete R from the page table and insert P.
            page_table_.erase(page_id_replace);
            page_table_.insert({page_id,frame_id_replace});
        // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
            page_index->ResetMemory();
            page_index->page_id_=page_id;
            page_index->pin_count_=1;
            page_index->is_dirty_=true;
            disk_manager_->ReadPage(page_id,page_index->data_);
            return page_index;
        }

    // if(!free_list_.empty()){
    //   //find a free frame from free_list_, by Sunlly0
    //   frame_id_replace=free_list_.front();
    //   free_list_.pop_front();
    // }
    // else{
    //   //use LRU
    //   //return a frame
    //   // all are full, and no frame to replace
    //   if(!replacer_->Victim(&frame_id_replace)){
    //     return nullptr;
      // }
    // }
  }
  return nullptr;
}

bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {
  Page *page_unpin=GetPageFromList(page_id);
  if(!page_unpin&&page_unpin->GetPinCount()>0){
    //decide if the pincount is right, if there is a process to unpin the page,the pincount must >0. by Sunlly0
      page_unpin->is_dirty_=is_dirty;

      if(--page_unpin->pin_count_==0){
        frame_id_t frame_unpin=page_table_.at(page_id);
        replacer_->Unpin(frame_unpin);
      }
      return true; 
  }
  return false; 
}

//TODO: if the page is pinned??
bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!
  Page *page_flush=GetPageFromList(page_id);
  //if page is dirty then
  if(!page_flush&&page_flush->IsDirty()==true){
    disk_manager_->WritePage(page_id, page_flush->GetData());
    return true;
  }
  return false;
  
}

//TODO: how to inilize page_new
Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {
    // 0.   Make sure you call DiskManager::AllocatePage!
  page_id_t page_id_new=disk_manager_->AllocatePage();

  frame_id_t frame_id_replace;
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  if(!ChooseFromFreeListOrLRU(&frame_id_replace)){
    // 3.   Update P's metadata, zero out memory and add P to the page table.
    auto page_new=pages_+frame_id_replace;
    page_new->ResetMemory();
    page_table_.insert({page_id_new,frame_id_replace});
    // 4.   Set the page ID output parameter. Return a pointer to P.
    page_id=&page_id_new;
    page_new->pin_count_=1;
    page_new->is_dirty_=true;
    page_new->page_id_=page_id_new;

    return page_new;
  }
  
  // if(!free_list_.empty()){
  //   frame_id_replace=free_list_.front();
  //   free_list_.pop_front();
  // }
  // else{
  //   //when choose a victim, should it flush to disk first? by Sunlly0
  //   if(!replacer_->Victim(&frame_id_replace)){
  //       return nullptr;
  //   }
  // }
  return nullptr;
}

bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
  // 0.   Make sure you call DiskManager::DeallocatePage!
  disk_manager_->DeallocatePage(page_id);
  // 1.   Search the page table for the requested page (P).
  Page *page_delete=GetPageFromList(page_id);

  // 1.   If P does not exist, return true.
  if(!page_delete){
    return true;
  }
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.
  if(page_delete->GetPinCount()==0){
    frame_id_t frame_id_delete=page_table_.erase(page_id);
    page_delete->ResetMemory();
    free_list_.remove(frame_id_delete);
    return true;
  }
  return false;
}

void BufferPoolManager::FlushAllPagesImpl() {
  // You can do it!
  for(auto iter=page_table_.begin();iter!=page_table_.end();++iter){
    auto page_id=iter->first;
    FlushPageImpl(page_id);
  }

}

}  // namespace bustub
