/*
 * standard-hash-table.cpp
 *
 *  Created on: Jun 16, 2015
 *      Author: root
 */
#include <inttypes.h>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "standard-hash-table.h"
#include "hashing-util.h"
#include "logging.h"

using namespace std;

namespace impala {


StandardHashTable::StandardHashTable() {
  num_nodes_ = 0;
}

void StandardHashTable::BucketSizeDistribution() {
  std::vector<int> bucket_size;
  for (int i = 0; i < BUCKETS; ++i) {
    int len = 0;
    for (int j = buckets_[i].node_idx_; j != NULL_CONTENT; j = nodes_[j].next_idx_) {
      ++len;
    }
    if (len >= bucket_size.size()) {
      // grow bucket_size to fit this size
      bucket_size.resize(len + 1, 0);
    }
    ++bucket_size[len];
  }

  std::stringstream distr;
  for (int i = 0; i < bucket_size.size(); ++i) {
    distr << i << ": " << bucket_size[i] << "\n";
  }
  LOG(INFO) << "Bucket Size Distribution\n" << distr.str();
}

BuildTuple* StandardHashTable::Find(const ProbeTuple* probe) {
  uint32_t bucket =hash_id(probe->id) % BUCKETS;

  for (int i = buckets_[bucket].node_idx_; i != NULL_CONTENT; i = nodes_[i].next_idx_) {

    if (probe->id == nodes_[i].tuple.id) {
      // found
      return &nodes_[i].tuple;
    }
  }
  return NULL;
}

void StandardHashTable::Insert(const BuildTuple* row) {
  DCHECK(!Full()); // caller is responsible for ensuring this passes.
  uint32_t hash = hash_id(row->id);
  int bucket_idx = hash % BUCKETS;

  Node* node = &nodes_[num_nodes_];


  node->next_idx_ = buckets_[bucket_idx].node_idx_;
  node->tuple.hash = hash;
  node->tuple.id = row->id;

  node->tuple.count = row->count;
  buckets_[bucket_idx].node_idx_ = num_nodes_;
  ++num_nodes_;


}

StandardHashTable::Iterator StandardHashTable::Begin() {
  if (num_nodes_ > 0) {
    return Iterator(this);
  } else {
    // If table is empty, give them an empty iterator.
    return End();
  }
}

void StandardHashTable::Iterator::Next() {
  if (++node_idx_ >= table_->num_nodes_) {
    // done
    node_idx_ = -1;
  }
}

} /* namespace impala */
