
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
#include <iostream>

using namespace std;
using namespace v8;

KernelDBConfig::KernelDBConfig(const std::string& _config, const std::string& _defaults) {
	Isolated isolated;
	config = Handle<Object>::Cast(isolated->jsonParse(_config.c_str()));
	defaults = Handle<Object>::Cast(isolated->jsonParse(_defaults.c_str()));
}

rocksdb::Options KernelDBConfig::options() const {
	rocksdb::Options o;
	
	o.create_if_missing = getBoolean(defaults, "create_if_missing");
	o.paranoid_checks = getBoolean(defaults, "paranoid_checks");
	o.write_buffer_size = getInteger(defaults, "write_buffer_size");
	o.max_write_buffer_number = getInteger(defaults, "max_write_buffer_number");
	o.min_write_buffer_number_to_merge = getInteger(defaults, "min_write_buffer_number_to_merge");
	o.max_open_files = getInteger(defaults, "max_open_files");
	o.block_size = getInteger(defaults, "block_size");
	o.block_restart_interval = getInteger(defaults, "block_restart_interval");
	o.num_levels = getInteger(defaults, "num_levels");
	o.disableDataSync = getBoolean(defaults, "disable_data_sync");
	o.use_fsync = getBoolean(defaults, "use_fsync");
	o.max_background_compactions = getInteger(defaults, "max_background_compactions");
	o.max_background_flushes = getInteger(defaults, "max_background_flushes");
	o.max_log_file_size = getInteger(defaults, "max_log_file_size");
	o.log_file_time_to_roll = getInteger(defaults, "log_file_time_to_roll");
	o.keep_log_file_num = getInteger(defaults, "keep_log_file_num");

	return o;
}

store_map KernelDBConfig::stores() const {

	rocksdb::Status status;
	rocksdb::Options defaultOptions = options();
	kernel_map kernels = Kernels();
	store_map s;
	
	Isolated isolated;
	Handle<Object> storeObj = Handle<Object>::Cast(config->Get(String::NewFromUtf8(isolated->isolate, "stores")));
	Handle<Array> storeNames = storeObj->GetPropertyNames();
	for (size_t i=0;i<storeNames->Length();++i) {
		Handle<Value> kernelName = storeNames->Get(i);
		Handle<String> kernelFileName = Handle<String>::Cast(storeObj->Get(kernelName));
		
		String::Utf8Value utfKernelName(kernelName);
		String::Utf8Value utfKernelFileName(kernelFileName);
		
		rocksdb::Options o = defaultOptions;
		
		// TODO: add kernel validation
		o.merge_operator.reset(new KMergeOperator(kernels[*utfKernelFileName]));
		
		// TODO: compute kernel specific overrides
		
		rocksdb::DB* db;
		stringstream buf;
		buf << LOCALSTATEDIR_PATH << "/stores/" << *utfKernelName;
		status = rocksdb::DB::Open(o, buf.str(), &db);
		// TODO: verify status
		
		s[*utfKernelName] = shared_ptr<rocksdb::DB>(db);
	}
	
	return s;
}

KernelDBConfig KernelDBConfig::Load() {
	string configData = kdb_strip_comments(kdb_file_contents(string(SYSCONFDIR_PATH) + "/kerneldb.json"));
	string defaultsData = kdb_strip_comments(kdb_file_contents(string(SYSCONFDIR_PATH) + "/defaults.json"));
	return KernelDBConfig(configData, defaultsData);
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


bool KernelDBConfig::getBoolean(const v8::Handle<v8::Object> obj, const std::string& key) const {
	Isolated isolated;
	Handle<Value> v = obj->Get(String::NewFromUtf8(isolated->isolate, key.c_str()));
	
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

int KernelDBConfig::getInteger(const v8::Handle<v8::Object> obj, const std::string& key) const {
	Isolated isolated;
	Handle<Value> v = obj->Get(String::NewFromUtf8(isolated->isolate, key.c_str()));
	
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


