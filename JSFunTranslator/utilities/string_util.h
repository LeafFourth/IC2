#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>

std::vector<std::string> SplitStr(const std::string &content, char c, bool skipNull);

#endif