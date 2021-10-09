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
#include "common/logger.h"

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

bool BufferPoolManager::ChooseFromFreeListOrLRU(frame_id_t *frame_id_replace){

  if(!free_list_.empty()){
    *frame_id_replace=free_list_.front();
    free_list_.pop_front();
    return true;
  }
  else{
    //when choose a victim, should it flush to disk first? by Sunlly0
    if(!replacer_->Victim(frame_id_replace)){
        return false;
    }
    else{
      Page *page_replace;
      page_id_t page_id_replace;
      page_replace=pages_+ *frame_id_replace;
      page_id_replace=page_replace->GetPageId();
      if(page_replace->IsDirty()){
          disk_manager_->WritePage(page_id_replace, page_replace->GetData());

      }
      page_replace->is_dirty_=false;
      page_replace->pin_count_=0;
      page_table_.erase(page_id_replace);

      return true;
    }
  }
  return false;

} 

Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
  std::lock_guard<std::mutex> lock(latch_);
  // 1.     Search the page table for the requested page (P).

  // frame_id_t frame_id;
  auto it = page_table_.find(page_id);
  // Page *page_index=GetPageFromList(page_id, &frame_id);

  // 1.1    If P exists, pin it and return it immediately.
  if(it!=page_table_.end()){
    Page *page_index=pages_+(it->second);
    page_index->pin_count_++;
    if(page_index->GetPinCount()==1){
      replacer_->Pin(it->second);
    }
    return page_index;
  }
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
   // 2.     If R is dirty, write it back to the disk.
  else{
      frame_id_t frame_id_replace;
      if(ChooseFromFreeListOrLRU(&frame_id_replace)){
            // 3.     Delete R from the page table and insert P.
            auto page_new=pages_+frame_id_replace;
        // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
            page_new->ResetMemory();
            page_new->page_id_=page_id;
            page_new->pin_count_++;
            page_new->is_dirty_=false;
            disk_manager_->ReadPage(page_id,page_new->GetData());
            replacer_->Pin(frame_id_replace);
            page_table_.insert({page_id,frame_id_replace});
            return page_new;
        }
  }
  return nullptr;
}

bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {
  std::lock_guard<std::mutex> lock(latch_);
  // Page *page_unpin=GetPageFromList(page_id,nullptr);
  auto it=page_table_.find(page_id);
  if(it==page_table_.end())return false;
  Page *page_unpin=pages_+ it->second;

    // if(page_unpin->GetPinCount()>0){
      //decide if the pincount is right, if there is a process to unpin the page,the pincount must >0. by Sunlly0
  page_unpin->is_dirty_=page_unpin->IsDirty()||is_dirty;
  page_unpin->pin_count_--;
  if(page_unpin->pin_count_==0){
    replacer_->Unpin(it->second);
  }
  return true; 
}

//TODO: if the page is pinned??
bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  std::lock_guard<std::mutex> lock(latch_);
  // Make sure you call DiskManager::WritePage!
  if(page_id==INVALID_PAGE_ID){
    return false;
  }
  // Page *page_flush=GetPageFromList(page_id,nullptr);
  auto it=page_table_.find(page_id);
  if(it==page_table_.end()){
    return false;
  }

  Page *page_flush=pages_+ it->second;
  //if page is dirty then
  disk_manager_->WritePage(page_id, page_flush->GetData());
  page_flush->is_dirty_=false;
  return true;

}

//TODO: how to inilize page_new
Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {
  std::lock_guard<std::mutex> lock(latch_);
    // 0.   Make sure you call DiskManager::AllocatePage!
  page_id_t page_id_new=disk_manager_->AllocatePage();

  frame_id_t frame_id_replace;
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  if(ChooseFromFreeListOrLRU(&frame_id_replace)){
    // 3.   Update P's metadata, zero out memory and add P to the page table.
    auto page_new=pages_+frame_id_replace;
    page_new->ResetMemory();
    // 4.   Set the page ID output parameter. Return a pointer to P.
    page_new->page_id_=page_id_new;
    page_new->pin_count_++;
    page_new->is_dirty_=false;
    replacer_->Pin(frame_id_replace);
    page_table_.insert({page_id_new,frame_id_replace});

    *page_id=page_id_new;
    return page_new;
  }

  return nullptr;
}

bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
  std::lock_guard<std::mutex> lock(latch_);
  // 0.   Make sure you call DiskManager::DeallocatePage!
  disk_manager_->DeallocatePage(page_id);
  // 1.   Search the page table for the requested page (P).
  // Page *page_delete=GetPageFromList(page_id,nullptr);
  auto it=page_table_.find(page_id);
  // 1.   If P does not exist, return true.
  if(it==page_table_.end()){
    return true;
  }

  Page *page_delete=pages_+it->second;
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.
  if(page_delete->GetPinCount()==0){
    page_delete->ResetMemory();
    page_delete->is_dirty_=false;
    page_delete->page_id_=INVALID_PAGE_ID;
    replacer_->Pin(it->second);
    free_list_.push_back(it->second);
    page_table_.erase(page_id);
    return true;
  }
  return false;
}

void BufferPoolManager::FlushAllPagesImpl() {
  std::lock_guard<std::mutex> lock(latch_);
  // You can do it!

  Page *page_flush=nullptr;
  for(auto it=page_table_.begin();it!=page_table_.end();++it){
    page_flush=pages_+ it->second;
    disk_manager_->WritePage(page_flush->GetPageId(), page_flush->GetData());
    page_flush->is_dirty_=false;
  }

}

}  // namespace bustub
