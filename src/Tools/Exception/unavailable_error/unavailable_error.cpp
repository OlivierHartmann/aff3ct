#include "unavailable_error.hpp"

using namespace aff3ct::tools;

const std::string unavailable_error::default_message = "Unavailable ressource.";

unavailable_error
::unavailable_error() throw()
: exception()
{
}

unavailable_error
::unavailable_error(const std::string &message) throw()
: exception(message.empty() ? default_message : message)
{
}

unavailable_error
::unavailable_error(const std::string &filename,
                      const int line_num,
                      const std::string &funcname,
                      const std::string &message) throw()
: exception(filename, line_num, funcname, message.empty() ? default_message : message)
{
}
