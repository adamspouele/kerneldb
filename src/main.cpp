
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


	// load config and stores
	KernelDBConfig config = KernelDBConfig::Load();
	store_map stores = config.stores();

	// mutate stores
	stores["counts"]->Merge(rocksdb::WriteOptions(), "a", "1");
	stores["counts"]->Merge(rocksdb::WriteOptions(), "a", "1");
	stores["counts"]->Merge(rocksdb::WriteOptions(), "a", "1");
	
	stores["visitors"]->Merge(rocksdb::WriteOptions(), "b", "{\"value\": [1,2,3], \"size\": 5}");
	
	
	// display stores
	Isolated isolated;
	
	rocksdb::Iterator* it = stores["counts"]->NewIterator(rocksdb::ReadOptions());
	for (it->SeekToFirst();it->Valid();it->Next()) {
		Handle<String> v = Handle<String>::Cast(isolated->jsonParse(it->value().ToString().c_str()));
		String::Utf8Value utf(v);
		cout << it->key().ToString() << " = " << *utf << endl;
	}
	delete it;
	
	
	it = stores["visitors"]->NewIterator(rocksdb::ReadOptions());
	for (it->SeekToFirst();it->Valid();it->Next()) {
		Handle<String> v = Handle<String>::Cast(isolated->jsonParse(it->value().ToString().c_str()));
		String::Utf8Value utf(v);
		cout << it->key().ToString() << " = " << *utf << endl;
	}
	delete it;
	
	
	Isolated::release();
	return 0;
}
