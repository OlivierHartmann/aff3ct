#ifndef UNAVAILABLE_ERROR_HPP_
#define UNAVAILABLE_ERROR_HPP_

#include "../exception.hpp"

namespace aff3ct
{
namespace tools
{
class unavailable_error : public exception
{
	static const std::string default_message;

public:
	unavailable_error() throw();

	explicit unavailable_error(const std::string &message) throw();

	unavailable_error(const std::string &filename,
	                    const int line_num,
	                    const std::string &funcname = exception::empty_string,
	                    const std::string &message = exception::empty_string) throw();

	virtual ~unavailable_error() throw() = default;
};
}
}

#endif /* UNAVAILABLE_ERROR_HPP_ */
