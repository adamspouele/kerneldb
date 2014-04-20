
#ifndef __KCONFIG_H__
#define __KCONFIG_H__

#include "isolated.h"

#include <rocksdb/db.h>
#include <v8.h>

#include <string>
#include <map>
#include <unordered_map>
#include <memory>

typedef std::map<std::string, std::string> kernel_map;
typedef std::unordered_map<std::string, std::shared_ptr<rocksdb::DB> > store_map;

struct KernelDBConfig {
	KernelDBConfig(const std::string& _config, const std::string& _defaults);

	rocksdb::Options options() const;
	store_map stores() const;
	
	static KernelDBConfig Load();
	static kernel_map Kernels();

private:
	bool getBoolean(const v8::Handle<v8::Object> obj, const std::string& key) const;
	int getInteger(const v8::Handle<v8::Object> obj, const std::string& key) const;
	
	v8::Handle<v8::Object> config;
	v8::Handle<v8::Object> defaults;
};

#endif
