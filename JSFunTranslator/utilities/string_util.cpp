#include "string_util.h"

std::vector<std::string> SplitStr(const std::string &content, char c, bool skipNull)
{
	std::vector<std::string> ret;
	if (content.empty()) {
		if (!skipNull) {
			ret.push_back(std::string());
		}
		return std::move(ret);
	}

	size_t lpos = 0;
	const size_t size = content.size();
	while(true) {
		size_t pos = content.find(c, lpos);

		if (pos == std::string::npos) {
			ret.push_back(content.substr(lpos));
			break;
		}

		if (pos == lpos) {
			if (!skipNull) {
				ret.push_back(std::string());
			}
		} else {
			ret.push_back(content.substr(lpos, pos - lpos));
			if (pos == size - 1) {
				if (!skipNull) {
					ret.push_back(std::string());
				}
			}
		}

		if (pos == size - 1) {
			break;
		}

		lpos = pos + 1;
	}

	return std::move(ret);
}