
#include "kerneldb.h"

#include "config.h"

#include <unistd.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <regex>
#include <stdexcept>
#include <sstream>
#include <iostream>

using namespace std;
using namespace v8;


std::string kdb_file_contents(const std::string& file) {
	int fd = open(file.c_str(), O_RDONLY);
	if (fd == -1)
		throw runtime_error(strerror(errno));
	
	stringstream buf;
	char tmp[4096];
	ssize_t n;
	while ((n = read(fd, tmp, sizeof(tmp))) > 0) {
		buf.write(tmp, n);
	}
	
	close(fd);
	
	if (n == -1) {
		throw runtime_error(strerror(errno));
	}
	
	return buf.str();
}

std::string kdb_strip_comments(const std::string& json) {
	return regex_replace(json, regex("//.*?\n"), "\n");
}


