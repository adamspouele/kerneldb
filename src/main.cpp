
#include "kerneldb/kerneldb.h"

#include <iostream>

#include <rocksdb/db.h>
#include <rocksdb/merge_operator.h>

#include <v8.h>

using namespace std;
using namespace v8;


int main(int argc, const char* const argv[]) {
	Isolated::create();

	auto kernel_files = kernels_list();
	cout << "kernels: " << kernel_files.size() << endl;
	for (auto& str : kernel_files) {
		cout << str << endl;
	}
	
	const char* kernel = R"(
		({
			"name": "Counter",
			"author": "subprotocol",
			"description": "Increments old value by new value",
			"merge": function(key, old, value) {
				return (old||0) + value;
			},
			"partialMerge": function(key, left, right) {
				return left + right;
			}
		});
	)";
	
	/*const char* kernel = R"(
		({
			"merge": function(key, old, value) {
				return (old||0) + value;
			}
		});
	)";*/
	
	// rocksdb options
	rocksdb::Options options;
	options.merge_operator.reset(new KMergeOperator(kernel));
	options.create_if_missing = true;
	
	// rocksdb db
	rocksdb::DB* db;
	rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/testdb", &db);
	
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
