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
        if (!frame_list.empty()) {
            frame_id = & frame_list.front();
            frame_list.pop_front();
            return true;
        }
        return false;

    }

    void LRUReplacer::Pin(frame_id_t frame_id) {
        if (!frame_list.empty()) {
            frame_list.remove(frame_id);
        }
    }

    void LRUReplacer::Unpin(frame_id_t frame_id) {
        frame_list.push_back(frame_id);
    }

    size_t LRUReplacer::Size() {
        return frame_list.size();
    }

} // namespace bustub