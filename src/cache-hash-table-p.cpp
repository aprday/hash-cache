/*

 * CacheHashTable.cpp
 *
 *  Created on: Jun 16, 2015
 *      Author: root


#include <vector>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include "cache-hash-table-p.h"
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

  if (bucket.count==0) return NULL;

  ContentBlob* content = bucket.index;

  // TODO lots of branching here
  while (content!= NULL) {


    for (int i = 0; i < CONTENT_SIZE; ++i) {



      BuildTuple* build = &content->build_tuples[i];
      if (build->id == probe->id) {
        // found
        return build;
      }
    }

    content = content->next;

  }
  return NULL;
}

void CacheHashTable::Insert(const BuildTuple* row) {
  // insert new
  uint32_t hash = hash_id(row->id);

  Bucket& bucket = buckets_[hash % BUCKETS];   //BUCKETS= 384 hash%BUCKETS ��λ��Ȼ����һ��ָ��ָ�����Ͱ����

  AllocateOneMoreSlot(&bucket);

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

//judge the last content is null or not
  if(bucket->index == NULL){
	  ContentBlob content;
	  bucket->index = &content;

  }else{//if not judge tuple num
	  ContentBlob * content = bucket->index;

	  return &(content->build_tuples[tuple_idx]);
  }
}

inline CacheHashTable::idx_t CacheHashTable::AllocateContent(Bucket* bucket) {
  CHECK_LT(num_content_allocated_, CONTENT_BLOBS);

  ContentBlob *content = new ContentBlob;
  content->next = bucket->index;
  bucket->index = content;

  return num_content_allocated_++;
}

void CacheHashTable::AllocateOneMoreSlot(Bucket* bucket) {
  count_t count = bucket->count;
  if (count % CONTENT_SIZE == 0) {
    // currently full, need new ContentBlob
    idx_t idx = AllocateContent(bucket);

  }
}




}  namespace impala
*/
