
#ifndef __ISOLATED_H__
#define __ISOLATED_H__

#include <pthread.h>
#include <v8.h>

class Isolated {
public:
	struct Instance {
		Instance();
	
		v8::Isolate* isolate;
		v8::Isolate::Scope isolateScope;
		v8::HandleScope handleScope;
		v8::Handle<v8::Context> context;
		v8::Context::Scope contextScope;
	
		v8::Handle<v8::Function> jsonParse;
		v8::Handle<v8::Function> jsonStringify;
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
