
#ifndef __ISOLATED_H__
#define __ISOLATED_H__

#include <pthread.h>
#include <v8.h>

class Isolated {
public:
	struct Instance {
		Instance();
	
		v8::Handle<v8::Value> jsonParse(const char* str) const;
		v8::Handle<v8::Value> jsonStringify(v8::Handle<v8::Value>& v) const;
		
		v8::Isolate* isolate;
		v8::Isolate::Scope isolateScope;
		v8::HandleScope handleScope;
		v8::Handle<v8::Context> context;
		v8::Context::Scope contextScope;
	
		v8::Handle<v8::Function> jsonParseFunc;
		v8::Handle<v8::Function> jsonStringifyFunc;
	};
	
	Isolated();
	virtual ~Isolated();
	
	Instance* operator->();
	const Instance* operator->() const;
	
	static void create();
	static void release();

private:
	Isolated(const Isolated&);
	Isolated& operator=(const Isolated&);
	Instance* inst;
};

#endif
