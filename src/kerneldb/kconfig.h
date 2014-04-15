
#ifndef __KCONFIG_H__
#define __KCONFIG_H__

#include "isolated.h"

#include <rocksdb/db.h>
#include <v8.h>

#include <string>
#include <map>

typedef std::map<std::string, std::string> kernel_map;

struct KernelDBConfig {
	KernelDBConfig(const std::string& _config);

	rocksdb::Options options() const;
	
	static KernelDBConfig Defaults();
	static kernel_map Kernels();

private:
	bool getBoolean(const std::string& key) const;
	int getInteger(const std::string& key) const;
	
	v8::Handle<v8::Object> config;
};

#endif
