#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <map>

#define EPS 1e-13

template <typename D, typename B>
inline bool isInstanceOf(const B* pointer) {
	return dynamic_cast<const D*>(pointer) != nullptr;
}

template <typename T>
inline std::string toString(T&& val) {
	std::ostringstream oss;
	oss << std::forward<T>(val);
	return oss.str();
}



#endif
