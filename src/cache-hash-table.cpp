/*

 * CacheHashTable.cpp
 *
 *  Created on: Jun 16, 2015
 *      Author: root
*/

#include <vector>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include "cache-hash-table.h"
#include "hashing-util.h"
#include "logging.h"

using namespace std;


namespace impala {

CacheHashTable::CacheHashTable() {
	// TODO Auto-generated constructor stub
	num_content_allocated_ = 0;

}

CacheHashTable::~CacheHashTable() {
	// TODO Auto-generated destructor stub
}

void CacheHashTable::test(){
cout<<"sysout"<<endl;

}

void CacheHashTable::BucketSizeDistribution() {
  std::vector<int> bucket_size;
  for (int i = 0; i < BUCKETS; ++i) {
    int size = buckets_[i].count;
    if (size >= bucket_size.size()) {
      // grow bucket_size to fit this size
      bucket_size.resize(size + 1, 0);
    }
    ++bucket_size[size];
  }

  std::stringstream distr;
  for (int i = 0; i < bucket_size.size(); ++i) {
    distr << i << ": " << bucket_size[i] << "\n";
  }
  LOG(INFO) << "Bucket Size Distribution\n" << distr.str();
}


BuildTuple* CacheHashTable::Find(const ProbeTuple* probe) {

  uint32_t bucket_idx = hash_id(probe->id) % BUCKETS; //BUCKETS = 348


  Bucket& bucket = buckets_[bucket_idx];	//�õ�buckets_

  int length = bucket.count;

  if (length == 0) return NULL;


	//�õ�bucket����ص�����
  idx_t content_idx = bucket.content;

  ContentBlob* content = &content_[content_idx];
  ContentBlobDescriptor* descriptor = &content_descriptors_[content_idx];

  // TODO lots of branching here
  while (length > 0) {
    DCHECK_NE(content_idx, NULL_CONTENT);
    idx_t start = 0;

    if (length == bucket.count) {
      // on the first block, need to skip any empty slots.
      // The number of items in the first blob is bucket.count % CONTENT_SIZE,
      // unless that is 0, in which case the first blob is full, not empty.
      start = (CONTENT_SIZE - (bucket.count % CONTENT_SIZE)) % CONTENT_SIZE;
    }

    for (int i = start; i < CONTENT_SIZE; ++i) {



      BuildTuple* build = &content->build_tuples[i];
      if (build->id == probe->id) {
        // found
        return build;
      }
    }

    length -= CONTENT_SIZE - start;
    content_idx = descriptor->next;
    content = &content_[content_idx];
    descriptor = &content_descriptors_[content_idx];

  }
  return NULL;
}

void CacheHashTable::Insert(const BuildTuple* row) {
  // insert new
  uint32_t hash = hash_id(row->id);

  Bucket& bucket = buckets_[hash % BUCKETS];   //BUCKETS= 384 hash%BUCKETS ��λ��Ȼ����һ��ָ��ָ�����Ͱ����

  AllocateOneMoreSlot(&bucket);	//�ж�һ��cache line�ǲ����Ѿ�����

  BuildTuple* tuple = InsertionPoint(&bucket); //����һ��Ͱ
  tuple->count = row->count;
  tuple->id = row->id;
  tuple->hash = hash;
  ++bucket.count;
}

BuildTuple* CacheHashTable::InsertionPoint(Bucket* bucket) {
  // Empty slots are in the very front of the chunk list, so we need the index of the
  // last one.
  idx_t tuple_idx = CONTENT_SIZE - (bucket->count % CONTENT_SIZE) - 1;
  return &content_[bucket->content].build_tuples[tuple_idx];
}

inline CacheHashTable::idx_t CacheHashTable::AllocateContent() {
  CHECK_LT(num_content_allocated_, CONTENT_BLOBS);
  return num_content_allocated_++;
}

void CacheHashTable::AllocateOneMoreSlot(Bucket* bucket) {
  count_t count = bucket->count;
  if (count % CONTENT_SIZE == 0) {
    // currently full, need new ContentBlob
    idx_t idx = AllocateContent();
    // Insert to front of bucket's linked list
    content_descriptors_[idx].next = bucket->content;
    bucket->content = idx;
  }
}



}

