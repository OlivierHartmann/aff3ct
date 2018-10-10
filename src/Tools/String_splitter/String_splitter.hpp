#ifndef STRING_SPLITTER_HPP_
#define STRING_SPLITTER_HPP_

#include <string>
#include <vector>

namespace aff3ct
{
namespace tools
{

inline std::vector<std::string> split(const std::string& val,
                                      const std::string& head,
                                      const std::string& queue,
                                      const std::string& separator)
{
	std::vector<std::string> list_vals;

	// try first to find a head
	size_t pos = val.find_first_of(head);

	if (pos == std::string::npos)
		pos = 0;
	else
		pos++;

	while (pos < val.size())
	{
		// try to find any separator to get values
		size_t found_pos = val.find_first_of(separator, pos);

		if (found_pos == std::string::npos)
		{
			// try find a queue character
			found_pos = val.find_first_of(queue, pos);

			if (found_pos != std::string::npos)
				list_vals.push_back(val.substr(pos, found_pos - pos));
			else
				list_vals.push_back(val.substr(pos));

			break;
		}

		list_vals.push_back(val.substr(pos, found_pos - pos));


		pos = found_pos + 1;
	}

	return list_vals;
}

inline std::vector<std::string> set_split(const std::string& val)
{
	const std::string head      = "{([";
	const std::string queue     = "})]";
	const std::string separator = ";,.|";

	return split(val, head, queue, separator);
}

inline std::vector<std::string> char_split(const std::string& val)
{
	std::vector<std::string> list_vals(val.size());

	for (unsigned i = 0; i < val.size(); i++)
		list_vals[i].append(1, val[i]); // add only once the character

	return list_vals;
}

}
}
#endif /* STRING_SPLITTER_HPP_ */