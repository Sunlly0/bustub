//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

    LRUReplacer::LRUReplacer(size_t num_pages) {}

    LRUReplacer::~LRUReplacer() =default;

    bool LRUReplacer::Victim(frame_id_t * frame_id) {
        std::lock_guard<std::mutex> lock(latch_); // in case there are several threads to operate at the same time. by Sunlly0

        if (!frame_list.empty()) {
            *frame_id = frame_list.front();
            frame_list.pop_front();
            return true;
        }
        return false;

    }

    void LRUReplacer::Pin(frame_id_t frame_id) {
        std::lock_guard<std::mutex> lock(latch_);
        if (!frame_list.empty()) {
            frame_list.remove(frame_id);
        }
    }

    void LRUReplacer::Unpin(frame_id_t frame_id) {
        std::lock_guard<std::mutex> lock(latch_);
        std::list<frame_id_t>::iterator itePos = std::find(frame_list.begin(),frame_list.end(),frame_id);
        //in case two same frame in frame_list. by Sunlly0
        if(itePos==frame_list.end()){
            frame_list.push_back(frame_id);
        }


    }

    size_t LRUReplacer::Size() {
        return frame_list.size();
    }

} // namespace bustub