
#ifndef __V8MERGE_H__
#define __V8MERGE_H__

#include "isolated.h"

#include <rocksdb/db.h>
#include <rocksdb/merge_operator.h>

#include <v8.h>

#include <string>
#include <pthread.h>


class KernelInstance {
public:
	KernelInstance(const std::string& kernelScript);
	virtual ~KernelInstance();
	
	v8::Handle<v8::Value> jsonParse(const char* str) const;
	v8::Handle<v8::Value> jsonStringify(v8::Handle<v8::Value>& v) const;
	
	v8::Handle<v8::Function> mergeFunc;
	v8::Handle<v8::Function> partialMergeFunc;
	bool hasPartial;
	Isolated isolated;
};

class V8MergeOperator : public rocksdb::MergeOperator {
public:
	V8MergeOperator(const std::string& _kernelScript);
	virtual ~V8MergeOperator();
	
	virtual bool FullMerge(
		const rocksdb::Slice& key,
	    const rocksdb::Slice* existing_value,
	    const std::deque<std::string>& operands,
	    std::string* new_value,
	    rocksdb::Logger* logger
	) const override;
		
	virtual bool PartialMerge(
		const rocksdb::Slice& key,
	    const rocksdb::Slice& left,
	    const rocksdb::Slice& right,
	    std::string* new_value,
	    rocksdb::Logger* logger
	) const override;
		
	virtual const char* Name() const override;
	
	KernelInstance* kernel() const;
			
private:
	const std::string kernelScript;
	pthread_key_t tsd; // thread specific data
};

#endif
