
#include "kmerge.h"


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



KMergeOperator::KMergeOperator(const std::string& _kernelScript)
: kernelScript(_kernelScript) {
	pthread_key_create(&tsd, NULL);
}

KMergeOperator::~KMergeOperator() {
	pthread_key_delete(tsd);
}

bool KMergeOperator::FullMerge(
	const rocksdb::Slice& key,
    const rocksdb::Slice* existing_value,
    const std::deque<std::string>& operands,
    std::string* new_value,
    rocksdb::Logger* logger
) const {
	
	// this method should always return true unless there is some sort of unrecoverable error when processing data
	
	KernelInstance* ki = kernel();
	
	// get existing value, otherwise set to null
	Handle<Value> ev = existing_value ? ki->isolated->jsonParse(existing_value->data()) : Handle<Value>::Cast(Null(ki->isolated->isolate));
	
	// key wrapper
	Handle<String> k(String::NewFromUtf8(ki->isolated->isolate, key.data()));
	
	// apply each operand
	for (const auto& operand : operands) {

		// arguments list
		Handle<Value> args[] = {
			k // key
			, ev // existing value
			, ki->isolated->jsonParse(operand.data()) // operand to apply
		};
		
		// call merge func
		ev = ki->mergeFunc->Call(ki->isolated->context->Global(), 3, args);
	}
	
	// save value
	*new_value = *String::Utf8Value(ki->isolated->jsonStringify(ev));
	
	return true;
}
	
bool KMergeOperator::PartialMerge(
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
		, ki->isolated->jsonParse(left.data()) // left
		, ki->isolated->jsonParse(right.data()) // right
	};
	
	// call partial merge func
	Handle<Value> r = ki->partialMergeFunc->Call(ki->isolated->context->Global(), 3, args);
	
	// save value
	*new_value = *String::Utf8Value(ki->isolated->jsonStringify(r));
	
	return true;
}

const char* KMergeOperator::Name() const {
	return "KMergeOperator";
}

KernelInstance* KMergeOperator::kernel() const {
	KernelInstance* ki = reinterpret_cast<KernelInstance*>(pthread_getspecific(tsd));
	if (!ki) {
		ki = new KernelInstance(kernelScript);
		pthread_setspecific(tsd, ki); // TODO: Add destructor
	}
	
	return ki;
}

