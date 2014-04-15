
#ifndef __KERNELDB_H__
#define __KERNELDB_H__

#include "isolated.h"
#include "kmerge.h"
#include "kconfig.h"

#include <vector>
#include <string>

std::string kdb_file_contents(const std::string& file);
std::string kdb_strip_comments(const std::string& json);

#endif
