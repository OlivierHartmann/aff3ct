#ifndef CLI_HELP_FORMATTER_HPP
#define CLI_HELP_FORMATTER_HPP

#include <CLI/CLI.hpp>

namespace CLI
{


class Help_formatter : public CLI::Formatter
{
public:
	Help_formatter() = default;
};

}

#endif //CLI_HELP_FORMATTER_HPP