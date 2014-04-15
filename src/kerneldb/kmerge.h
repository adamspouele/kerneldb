
#ifndef __KMERGE_H__
#define __KMERGE_H__

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
	
	v8::Handle<v8::Function> mergeFunc;
	v8::Handle<v8::Function> partialMergeFunc;
	bool hasPartial;
	Isolated isolated;
};

class KMergeOperator : public rocksdb::MergeOperator {
public:
	KMergeOperator(const std::string& _kernelScript);
	virtual ~KMergeOperator();
	
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
