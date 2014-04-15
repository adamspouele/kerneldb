
#include "kconfig.h"

#include "kerneldb.h"

#include "config.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <dirent.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <sstream>
#include <stdexcept>
#include <regex>

using namespace std;
using namespace v8;

KernelDBConfig::KernelDBConfig(const std::string& _config) {
	Isolated isolated;
	config = Handle<Object>::Cast(isolated->jsonParse(_config.c_str()));
}

rocksdb::Options KernelDBConfig::options() const {
	rocksdb::Options o;
	
	o.create_if_missing = getBoolean("create_if_missing");
	o.paranoid_checks = getBoolean("paranoid_checks");
	o.write_buffer_size = getInteger("write_buffer_size");
	o.max_write_buffer_number = getInteger("max_write_buffer_number");
	o.min_write_buffer_number_to_merge = getInteger("min_write_buffer_number_to_merge");
	o.max_open_files = getInteger("max_open_files");
	o.block_size = getInteger("block_size");
	o.block_restart_interval = getInteger("block_restart_interval");
	o.num_levels = getInteger("num_levels");
	o.disableDataSync = getBoolean("disable_data_sync");
	o.use_fsync = getBoolean("use_fsync");
	o.max_background_compactions = getInteger("max_background_compactions");
	o.max_background_flushes = getInteger("max_background_flushes");
	o.max_log_file_size = getInteger("max_log_file_size");
	o.log_file_time_to_roll = getInteger("log_file_time_to_roll");
	o.keep_log_file_num = getInteger("keep_log_file_num");

	return o;
}

KernelDBConfig KernelDBConfig::Defaults() {
	const string configFile = string(SYSCONFDIR_PATH) + "/kerneldb.json";
	string configData = kdb_file_contents(configFile);
	configData = kdb_strip_comments(configData);
	return KernelDBConfig(configData);
}

kernel_map KernelDBConfig::Kernels() {
	kernel_map m;
	struct dirent entry;
	struct dirent* result;
	
	// only files ending in .js
	regex jsExtensionRegex("[.]js$",regex_constants::ECMAScript | regex_constants::icase);
	
	DIR* dp = opendir(KERNELS_PATH);
	for (int rc=readdir_r(dp, &entry, &result);result!=NULL&&rc==0;readdir_r(dp, &entry, &result)) {
		
		// ignore hidden files and anything that isn't a file
		if (entry.d_name[0] == '.' || !(entry.d_type == DT_REG || entry.d_type == DT_LNK))
			continue;
	
		// ignore files that aren't .js
		string file = string(KERNELS_PATH) + string("/") + entry.d_name;
		if (!regex_search(file, jsExtensionRegex))
			continue;
		
		m[entry.d_name] = kdb_file_contents(file);
	}
	
	closedir(dp);
	
	return m;
}


bool KernelDBConfig::getBoolean(const std::string& key) const {
	Isolated isolated;
	Handle<Value> v = config->Get(String::NewFromUtf8(isolated->isolate, key.c_str()));
	
	if (v->IsUndefined()) {
		stringstream buf;
		buf << "config key '" << key << "' was not defined";
		throw runtime_error(buf.str());
	}
	
	if (!v->IsBoolean()) {
		stringstream buf;
		buf << "expected config key '" << key << "' to be a boolean";
		throw runtime_error(buf.str());
	}

	return v->ToBoolean()->Value();
}

int KernelDBConfig::getInteger(const std::string& key) const {
	Isolated isolated;
	Handle<Value> v = config->Get(String::NewFromUtf8(isolated->isolate, key.c_str()));
	
	if (v->IsUndefined()) {
		stringstream buf;
		buf << "config key '" << key << "' was not defined";
		throw runtime_error(buf.str());
	}
	
	if (!v->IsNumber()) {
		stringstream buf;
		buf << "expected config key '" << key << "' to be a number";
		throw runtime_error(buf.str());
	}

	return v->ToInteger()->Value();
	
	return 0;
}


