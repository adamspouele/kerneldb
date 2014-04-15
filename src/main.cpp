
#include "kerneldb/kerneldb.h"

#include <iostream>

#include <rocksdb/db.h>
#include <rocksdb/merge_operator.h>

#include <v8.h>

#include "config.h"

using namespace std;
using namespace v8;


int main(int argc, const char* const argv[]) {
	Isolated::create();

	KernelDBConfig config = KernelDBConfig::Defaults();
	auto kernels = KernelDBConfig::Kernels();
	
	// TODO: add kernel validation
	
	
	// rocksdb options
	rocksdb::Options options = config.options();
	options.merge_operator.reset(new KMergeOperator(kernels["counter.js"]));
	
	// rocksdb db
	rocksdb::DB* db;
	rocksdb::Status status = rocksdb::DB::Open(options, string(LOCALSTATEDIR_PATH)+"/meta", &db);
	
	rocksdb::Status s;
	
	s = db->Merge(rocksdb::WriteOptions(), "a", "1");
	
	//s = db->Merge(rocksdb::WriteOptions(), "a", "[1,2,3]");
	//s = db->Merge(rocksdb::WriteOptions(), "a", "\"function (v) { return v.concat([v.length+1]); }\"");
	//s = db->Merge(rocksdb::WriteOptions(), "a", "\"function (v) { return v.map(function (x) { return x*2; }); }\"");
	
	rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
	for (it->SeekToFirst();it->Valid();it->Next()) {
		cout << it->key().ToString() << " = " << it->value().ToString() << endl;
	}
	delete it;

	
	delete db;
		
	Isolated::release();
	
	return 0;
}
