//============================================================================
// Name        : test001.cpp
// Author      : flyfish
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "stopwatch.h"
#include "standard-hash-table.h"
#include "cache-hash-table.h"

using namespace std;
using namespace impala;

// Update ht, which is doing a COUNT(*) GROUP BY id,
// by having it process the new tuple probe.
// Templatized on the type of hash table so we can reuse code without virtual calls.
template<typename T>
inline void Process(T* ht, const ProbeTuple* probe) {
	BuildTuple *existing = ht->Find(probe);
	if (existing != NULL) {
		++existing->count;
	} else {
		BuildTuple build;
		build.id = probe->id;
		build.count = 1;
		ht->Insert(&build);
	}
}
// Test ht by aggregating input, which is an array of num_tuples ProbeTuples
// Templatized on the type of hash table so we can reuse code without virtual calls.
template<typename T>
uint64_t Test(T* ht, const ProbeTuple* input, uint64_t num_tuples) {
	StopWatch time;
	time.Start();
	int t = 102400;
	while(t--) {
		for (int i = 0; i < 300; ++i) {
			Process<T>(ht, &input[i]);
		}
	}
	time.Stop();
	return time.ElapsedTime();
}


template <typename T>
uint64_t Test2(T *ht, const ProbeTuple * probe, int num_tuple){
	StopWatch time;
	time.Start();
	BuildTuple build;
	build.id = probe->id;
	build.count = 1;
	ht->Insert(&build);


	time.Stop();

	return time.ElapsedTime();


}


template <typename T>
uint64_t Test3(T *ht, const ProbeTuple * probe, int num_tuple){
	StopWatch time;
	time.Start();

	BuildTuple *existing = ht->Find(probe);


	time.Stop();
	return time.ElapsedTime();


}

int main() {

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	CacheHashTable cache_ht;
/*	cache_ht.GetSize();*/

	// google::InitGoogleLogging(argv[0]);
	// CpuInfo::Init();

	srand(time(NULL));

	const int NUM_TUPLES = 100000000; //10^8
	const int NUM_BUILD_TUPLES = 4 * CacheHashTable::MaxBuildTuples() / 10;


	StandardHashTable std_ht;



	const ProbeTuple* input = GenTuples(NUM_TUPLES, NUM_BUILD_TUPLES);
	uint64_t std_time = Test<StandardHashTable>(&std_ht, input, NUM_TUPLES);

			  cout << "Bucket-chained time: "
			             << std_time<<endl;
	uint64_t cache_time = Test<CacheHashTable>(&cache_ht, input, NUM_TUPLES);
		 cout<< "Cache-aware time: "
		             << cache_time<<endl;



cout<<"-------------------"<<endl;

/*uint64_t time1 = Test2<CacheHashTable>(&cache_ht, input,999);
uint64_t time2 = Test3<CacheHashTable>(&cache_ht, input,999);
cout<<time1<<endl;
cout<<time2<<endl;*/
/*uint64_t time2 = Test2<StandardHashTable>(&std_ht, input,999);
cout<<time2<<endl;*/



	return 0;
}
