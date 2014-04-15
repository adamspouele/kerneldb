
#include "v8merge.h"


#include <iostream>
using namespace std;
using namespace v8;


KernelInstance::KernelInstance(const std::string& kernelScript) {
	Handle<Script> script = Script::Compile(String::NewFromUtf8(isolated->isolate, kernelScript.c_str()));
	Handle<Object> object = Handle<v8::Object>::Cast(script->Run());
	mergeFunc = Handle<v8::Function>::Cast(object->Get(String::NewFromUtf8(isolated->isolate, "merge")));
	partialMergeFunc = Handle<v8::Function>::Cast(object->Get(String::NewFromUtf8(isolated->isolate, "partialMerge")));
	hasPartial = partialMergeFunc->IsFunction();
}

KernelInstance::~KernelInstance() {
	
}

v8::Handle<v8::Value> KernelInstance::jsonParse(const char* str) const {
	Handle<Value> v = String::NewFromUtf8(isolated->isolate, str);
	return isolated->jsonParse->Call(isolated->context->Global(), 1, &v);
}

v8::Handle<v8::Value> KernelInstance::jsonStringify(v8::Handle<v8::Value>& v) const {
	return isolated->jsonStringify->Call(isolated->context->Global(), 1, &v);
}




V8MergeOperator::V8MergeOperator(const std::string& _kernelScript)
: kernelScript(_kernelScript) {
	pthread_key_create(&tsd, NULL);
}

V8MergeOperator::~V8MergeOperator() {
	pthread_key_delete(tsd);
}

bool V8MergeOperator::FullMerge(
	const rocksdb::Slice& key,
    const rocksdb::Slice* existing_value,
    const std::deque<std::string>& operands,
    std::string* new_value,
    rocksdb::Logger* logger
) const {
	
	// this method should always return true unless there is some sort of unrecoverable error when processing data
	
	KernelInstance* ki = kernel();
	
	// get existing value, otherwise set to null
	Handle<Value> ev = existing_value ? ki->jsonParse(existing_value->data()) : Handle<Value>::Cast(Null(ki->isolated->isolate));
	
	// key wrapper
	Handle<String> k(String::NewFromUtf8(ki->isolated->isolate, key.data()));
	
	// apply each operand
	for (const auto& operand : operands) {

		// arguments list
		Handle<Value> args[] = {
			k // key
			, ev // existing value
			, ki->jsonParse(operand.data()) // operand to apply
		};
		
		// call merge func
		ev = ki->mergeFunc->Call(ki->isolated->context->Global(), 3, args);
	}
	
	// save value
	*new_value = *String::Utf8Value(ki->jsonStringify(ev));
	
	return true;
}
	
bool V8MergeOperator::PartialMerge(
	const rocksdb::Slice& key,
    const rocksdb::Slice& left,
    const rocksdb::Slice& right,
    std::string* new_value,
    rocksdb::Logger* logger
) const {

	KernelInstance* ki = kernel();
	
	// this method should return false if the partial func is not supplied
	if (!ki->hasPartial)
		return false;
	
	// arguments list
	Handle<Value> args[] = {
		String::NewFromUtf8(ki->isolated->isolate, key.data()) // key
		, ki->jsonParse(left.data()) // left
		, ki->jsonParse(right.data()) // right
	};
	
	// call partial merge func
	Handle<Value> r = ki->partialMergeFunc->Call(ki->isolated->context->Global(), 3, args);
	
	// save value
	*new_value = *String::Utf8Value(ki->jsonStringify(r));
	
	return true;
}

const char* V8MergeOperator::Name() const {
	return "V8MergeOperator";
}

KernelInstance* V8MergeOperator::kernel() const {
	KernelInstance* ki = reinterpret_cast<KernelInstance*>(pthread_getspecific(tsd));
	if (!ki) {
		ki = new KernelInstance(kernelScript);
		pthread_setspecific(tsd, ki); // TODO: Add destructor
	}
	
	return ki;
}

