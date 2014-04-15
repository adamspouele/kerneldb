
#include "isolated.h"

#include <stdexcept>
using namespace std;
using namespace v8;


static pthread_key_t tsd;


Isolated::Instance::Instance()
: isolate(Isolate::New())
, isolateScope(isolate)
, handleScope(isolate)
, context(Context::New(isolate))
, contextScope(context)
{
	Handle<Object> json = context->Global()->Get(String::NewFromUtf8(isolate, "JSON"))->ToObject();
	jsonParseFunc = Handle<Function>::Cast(json->Get(String::NewFromUtf8(isolate, "parse")));
	jsonStringifyFunc = Handle<Function>::Cast(json->Get(String::NewFromUtf8(isolate, "stringify")));
}

v8::Handle<v8::Value> Isolated::Instance::jsonParse(const char* str) const {
	Handle<Value> v = String::NewFromUtf8(isolate, str);
	return jsonParseFunc->Call(context->Global(), 1, &v);
}

v8::Handle<v8::Value> Isolated::Instance::jsonStringify(v8::Handle<v8::Value>& v) const {
	return jsonStringifyFunc->Call(context->Global(), 1, &v);
}



Isolated::Isolated() {
	inst = reinterpret_cast<Instance*>(pthread_getspecific(tsd));
	if (!inst) {
		inst = new Instance;
		pthread_setspecific(tsd, inst);
	}
}

Isolated::~Isolated() {
	
}

Isolated::Instance* Isolated::operator->() {
	return inst;
}

const Isolated::Instance* Isolated::operator->() const {
	return inst;
}

void Isolated::create() {
	pthread_key_create(&tsd, NULL);
}

void Isolated::release() {
	pthread_key_delete(tsd);
}

