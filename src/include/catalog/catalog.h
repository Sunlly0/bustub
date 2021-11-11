#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "catalog/schema.h"
#include "storage/index/b_plus_tree_index.h"
#include "storage/index/index.h"
#include "storage/table/table_heap.h"

namespace bustub {

/**
 * Typedefs
 */
using table_oid_t = uint32_t;
using column_oid_t = uint32_t;
using index_oid_t = uint32_t;

/**
 * Metadata about a table.
 */
struct TableMetadata {
  TableMetadata(Schema schema, std::string name, std::unique_ptr<TableHeap> &&table, table_oid_t oid)
      : schema_(std::move(schema)), name_(std::move(name)), table_(std::move(table)), oid_(oid) {}
  Schema schema_;
  std::string name_;
  std::unique_ptr<TableHeap> table_;
  table_oid_t oid_;
};

/**
 * Metadata about a index
 */
struct IndexInfo {
  IndexInfo(Schema key_schema, std::string name, std::unique_ptr<Index> &&index, index_oid_t index_oid,
            std::string table_name, size_t key_size)
      : key_schema_(std::move(key_schema)),
        name_(std::move(name)),
        index_(std::move(index)),
        index_oid_(index_oid),
        table_name_(std::move(table_name)),
        key_size_(key_size) {}
  Schema key_schema_;
  std::string name_;
  std::unique_ptr<Index> index_;
  index_oid_t index_oid_;
  std::string table_name_;
  const size_t key_size_;
};

/**
 * Catalog is a non-persistent catalog that is designed for the executor to use.
 * It handles table creation and table lookup.
 */
class Catalog {
 public:
  /**
   * Creates a new catalog object.
   * @param bpm the buffer pool manager backing tables created by this catalog
   * @param lock_manager the lock manager in use by the system
   * @param log_manager the log manager in use by the system
   */
  Catalog(BufferPoolManager *bpm, LockManager *lock_manager, LogManager *log_manager)
      : bpm_{bpm}, lock_manager_{lock_manager}, log_manager_{log_manager} {}

  /**
   * Create a new table and return its metadata.
   * @param txn the transaction in which the table is being created
   * @param table_name the name of the new table
   * @param schema the schema of the new table
   * @return a pointer to the metadata of the new table
   */
  TableMetadata *CreateTable(Transaction *txn, const std::string &table_name, const Schema &schema) {
    BUSTUB_ASSERT(names_.count(table_name) == 0, "Table names should be unique!");
    //1.创建新表（TableMetaData），分配table_oid
    TableHeap *tableheap=new TableHeap(bpm_,lock_manager_,log_manager_,txn);
    auto table_oid=next_table_oid_++;
    auto *tablemetadata=new TableMetadata(schema,table_name,static_cast<std::unique_ptr<TableHeap>>(tableheap),table_oid);

    //2.添加到hash表：tables_（table_oid->tablementadata）
    tables_[table_oid]=static_cast<std::unique_ptr<TableMetadata>>(tablemetadata);

    //3.添加到hash表：names_（name->table_oid）
    names_[table_name]=table_oid;
    return tablemetadata;
  }

  /** @return table metadata by name */
  TableMetadata *GetTable(const std::string &table_name) {
    //从hash表：names_中找table_oid，再从tables_中找table_meta_data
    if(names_.count(table_name)==0) {
      throw std::out_of_range(table_name);
    }
    auto table_oid=names_[table_name];
    if(tables_[table_oid]!=0){
      //Q：为啥要加get才行呢？
      return tables_[table_oid].get();
    }
    return nullptr;
  }

  /** @return table metadata by oid */
  TableMetadata *GetTable(table_oid_t table_oid) {
    //从hash表：tables_中找table_meta_data
    if(tables_[table_oid]!=0){
      //Q：同上
      return tables_[table_oid].get();
    }
    return nullptr;
 }

  /**
   * Create a new index, populate existing data of the table and return its metadata.
   * @param txn the transaction in which the table is being created
   * @param index_name the name of the new index
   * @param table_name the name of the table
   * @param schema the schema of the table
   * @param key_schema the schema of the key
   * @param key_attrs key attributes
   * @param keysize size of the key
   * @return a pointer to the metadata of the new table
   */
  template <class KeyType, class ValueType, class KeyComparator>
  IndexInfo *CreateIndex(Transaction *txn, const std::string &index_name, const std::string &table_name,
                         const Schema &schema, const Schema &key_schema, const std::vector<uint32_t> &key_attrs,
                         size_t keysize) {
    //1.创建新索引（IndexMetadata）（Index）（IndexInfo），分配index_oid
    auto index_oid=next_index_oid_++;
    auto *indexmetadata=new IndexMetadata(index_name,table_name,&schema,key_attrs);
    auto *index=new BPlusTreeIndex<KeyType, ValueType, KeyComparator>(indexmetadata,bpm_);
    auto *indexinfo=new IndexInfo(key_schema,index_name,static_cast<std::unique_ptr<Index>>(index),index_oid,table_name,keysize);

    //2.添加到hash表：indexes_（index_oid->indexmentadata）
    indexes_[index_oid]=static_cast<std::unique_ptr<IndexInfo>>(indexinfo);

    //3.添加到hash表：index_names_（table_name->index_name,_index_oid）
    index_names_[table_name].insert(std::unordered_map<std::string, index_oid_t>::value_type (index_name, index_oid));

    //TODO: add index for every tuple??

    return indexinfo;
  }

  IndexInfo *GetIndex(const std::string &index_name, const std::string &table_name) {
    //从hash表：index_names_中找index_oid，再从indexes_中找index_info
    if(index_names_.count(table_name)==0) {
      throw std::out_of_range(table_name);
    }
    auto index_oid=index_names_[table_name][index_name];
    if(indexes_[index_oid]!=0){
      return indexes_[index_oid].get();
    }
    return nullptr;
}

  IndexInfo *GetIndex(index_oid_t index_oid) {
    //从hash表：indexes_中找index_info
    if(indexes_[index_oid]!=0){
      return indexes_[index_oid].get();
    }
    return nullptr;
  }

  std::vector<IndexInfo *> GetTableIndexes(const std::string &table_name) {
    if(index_names_.count(table_name)==0) {
      throw std::out_of_range(table_name);
    }
    //用vector保存index_info
    std::vector<IndexInfo*> index_vector;
    //先在index_names_中找到table中所有的index_oid，再在indexes_中找到index_info.
    for (auto &pair : index_names_[table_name]) {
      auto index_oid=pair.second;
//      index_vector.push_back(static_cast<IndexInfo *>(indexes_[index_oid].get()));
      index_vector.push_back(indexes_[index_oid].get());
    }
    return index_vector;
  }

 private:
  [[maybe_unused]] BufferPoolManager *bpm_;
  [[maybe_unused]] LockManager *lock_manager_;
  [[maybe_unused]] LogManager *log_manager_;

  /** tables_ : table identifiers -> table metadata. Note that tables_ owns all table metadata. */
  std::unordered_map<table_oid_t, std::unique_ptr<TableMetadata>> tables_;
  /** names_ : table names -> table identifiers */
  std::unordered_map<std::string, table_oid_t> names_;
  /** The next table identifier to be used. */
  std::atomic<table_oid_t> next_table_oid_{0};
  /** indexes_: index identifiers -> index metadata. Note that indexes_ owns all index metadata */
  std::unordered_map<index_oid_t, std::unique_ptr<IndexInfo>> indexes_;
  /** index_names_: table name -> index names -> index identifiers */
  std::unordered_map<std::string, std::unordered_map<std::string, index_oid_t>> index_names_;
  /** The next index identifier to be used */
  std::atomic<index_oid_t> next_index_oid_{0};
};
}  // namespace bustub
