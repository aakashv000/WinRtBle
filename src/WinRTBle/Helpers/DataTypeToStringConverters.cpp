#include <string>
#include <sstream>


inline std::string uint64_t_to_string(int64_t value) {
	std::ostringstream o;
	o << value;
	return o.str();
}