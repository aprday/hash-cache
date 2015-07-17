/*

 * CacheHashTable.h
 *
 *  Created on: Jun 16, 2015
 *      Author: root
*/

#ifndef CACHEHASHTABLE_H_
#define CACHEHASHTABLE_H_

#include <stdlib.h>
#include "tuple-types.h"


namespace impala {

// L1-sized hash table that hopes to use cache well
// Each bucket is a chunk list of tuples. Each chunk is a cache line.
class CacheHashTable {
 public:
  CacheHashTable();
  ~CacheHashTable();

  void test();
  // Lookup probe in the hashtable. If there is an aggregation tuple that matches probe
  // on the aggregation columns (currently just id), then this returns a pointer to that
  // build_tuple in the hash table.
  // Else, returns NULL
  BuildTuple* Find(const ProbeTuple* probe);

  // Inserts a new BuildTuple row into the hash table. There must not already be an
  // existing entry that matches row on the aggregation columns (currently just id).
  void Insert(const BuildTuple* row);

  // Returns the maximum number of build tuples that can fit in the hash table
  // In practice, we must not attempt to store this much unless the entries hash
  // perfectly.
  static int MaxBuildTuples() { return MAX_BUILD_TUPLES; }

  // EXPERIMENTING: Print the distribution of bucket sizes
  void BucketSizeDistribution();


 private:
  typedef uint16_t count_t; // type for bucket counts
  typedef uint16_t idx_t; // type for indexes into our arrays
  static const idx_t NULL_CONTENT = UINT16_MAX; // NULL index

  static const int L1_SIZE = 32 * 1024; // 32K cache    32k���� L1 Cache(һ������)��CPU��һ����ٻ��棬��Ϊ��ݻ����ָ��档���õ�L1���ٻ���������ͽṹ��CPU������Ӱ��ϴ󣬲�����ٻ���洢�����ɾ�̬RAM��ɣ��ṹ�ϸ��ӣ���CPU��о�����̫�������£�L1�����ٻ������������������̫��һ�������CPU��L1���������ͨ��������32
  static const int TABLE_MEMORY = 30 * 1024; // 30K table
  static const int LINE_SIZE = 64; // 64B L1 cache lines  ÿ��cache lines�Ĵ�С��64B

  static const int LINES_PER_BLOB = 1; // cache lines per	cacheline��ŵ�ContentBlob ÿ�����ݿ�
  // BuildTuples per ContentBlob
  static const int CONTENT_SIZE = (LINES_PER_BLOB * LINE_SIZE) / sizeof(BuildTuple);  //�������ݵĸ���

  // Storage of the actual tuples
  // Each blob is 1 cache line big.
  struct ContentBlob {
    BuildTuple build_tuples[CONTENT_SIZE]; // the content	ÿ�����ݿ�Ĵ�С��һ��cache lines�Ĵ�С
    uint8_t padding[LINE_SIZE % sizeof(BuildTuple)]; // pad to fill a cache line  ����һ��ƫ���Ƶ�ÿ��cacheline����ʹbuildtuple����
  } __attribute__((__packed__)) __attribute__((aligned(64)));//��ʹ�ý��ն��뷽ʽ

  // Metadata about a ContentBlob contentblobԪ���
  struct ContentBlobDescriptor {
    idx_t next; // the index of the content blob that follows �������ݵ�BLOB������
  } __attribute__((__packed__));

  struct Bucket {
    count_t count; // how many BuildTuples are currently in the bucket ����һ��unsigned short��count����
    idx_t content; // index of the first ContentBlob ����һ��unsigned short��content����

    ContentBlob *index;//build a index to a ContentBlob

    Bucket() {
      count = 0;
      content = NULL_CONTENT;
    }
  } __attribute__((__packed__));

  // TODO code the math that determines this is the max number of buckets that allows the
  // table to fit in 30k
  static const int BUCKETS = 348;
  // ratio of ContentBlobs to Buckets
  static const int CONTENT_RATIO_NUM = 5; // numerator
  static const int CONTENT_RATIO_DENOM = 4; // denominator
  static const int CONTENT_BLOBS = CONTENT_RATIO_NUM * BUCKETS / CONTENT_RATIO_DENOM;

  static const int MAX_BUILD_TUPLES = CONTENT_BLOBS * CONTENT_SIZE;


  // Returns a pointer to an empty tuple that is the next to be filled.
  BuildTuple* InsertionPoint(Bucket* bucket);

  // Make that index idx in bucket is valid, allocating OverflowLines as needed.
  void AllocateUpTo(Bucket* bucket, int32_t idx);

  // Allocate and return the index of the next free ContentBlob
  idx_t AllocateContent();

  // Make sure that Bucket has capacity for (bucket->count + 1) tuples.
  // Allocates a ContentBlob and adds it to the end of the chunk list if necessary.
  // Caller *MUST* ensure that this slot is filled before Find is next called,
  // as this may break invariants.
  void AllocateOneMoreSlot(Bucket* bucket);

  ContentBlob content_[CONTENT_BLOBS]; // the ContentBlobs that hold the tuples
  Bucket buckets_[BUCKETS]; // the metadata for the buckets
 // the metadata for content_. Index i of this describes index i of content_
  ContentBlobDescriptor content_descriptors_[CONTENT_BLOBS];
  // How many blobs in content_ have been allocated.
  // They are allocated linearly. (index 0, then 1, etc.)
  int32_t num_content_allocated_;
};

}
#endif  CACHEHASHTABLE_H_

