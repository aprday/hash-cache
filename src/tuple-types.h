// This file now describes the interface for defining operations for our hash-store.
// Ultimately, function pointers will be passed in as parameters instead, but without
// codegen, we cannot assess efficiency with that interface because the calls can't be
// inlined.

#ifndef IMPALA_EXPERIMENTS_HASHING_TUPLE_TYPES_H
#define IMPALA_EXPERIMENTS_HASHING_TUPLE_TYPES_H

#include <sys/types.h>
#include <inttypes.h>

#ifndef UINT16_MAX
#define UINT16_MAX 65535U
#endif


namespace impala {

// aggregating tuple stored in the hashtable
struct BuildTuple {
  uint32_t hash; // hash of the column we're grouping on (id)
  int32_t id; // id column, what we're aggregating on
  int64_t count; // current value of aggregate function (COUNT(*))
} __attribute__((__packed__));

// tuple from the db
struct ProbeTuple {
  int32_t id;
  uint32_t hash;
} __attribute__((__packed__));

// Generate n random tuples, with ids in the range [0, max_id).
inline ProbeTuple* GenTuples(uint64_t n, int max_id) {
  ProbeTuple* tuples
    = static_cast<ProbeTuple*>(malloc(n * sizeof(ProbeTuple)));

  //CHECK(tuples != NULL);

  for (uint64_t i = 0; i < n; ++i) {
    tuples[i].id = rand() % max_id;
  }
  return tuples;
}


}


#endif
