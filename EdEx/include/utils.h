#pragma once

// Include some things
#include <iostream>
#include <vector>
#include <chrono>
#include <regex>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>

namespace edex
{
	inline double seconds()
	{
		return (double) std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000;
	}

	inline std::string substring(const std::string &string, uint32_t start, uint32_t len, bool ignoreWhitespace)
	{
		uint32_t whitespace = 0;

		if (!ignoreWhitespace)
		{
			whitespace = string.find_first_not_of(' ');
			if (whitespace == std::string::npos)
				whitespace = 0;
		}

		std::string res(string.begin() + start + whitespace, string.begin() + start + whitespace + len);
		return res;
	}

	inline std::vector<std::string> splitString(const std::string &line, const std::vector<char> &delimiters)
	{
		std::vector<std::string> res;
		std::istringstream stream(line);
		std::string token;

		while (std::getline(stream, token, delimiters[0]))
		{
			int32_t found = -1;
			for (uint64_t i = 0; i < delimiters.size(); i++)
			{
				if (token.find_first_of(delimiters[i]) != std::string::npos)
				{
					auto split = splitString(token, std::vector<char>(delimiters.begin() + i, delimiters.end()));
					res.insert(res.end(), split.begin(), split.end());
					if (split.size() != 0)
						res.insert(res.end() - split.size() + 1, std::string(1, delimiters[i]));
					else
						res.emplace_back(std::string(1, delimiters[i]));
					found = i;
					break;
				}
			}

			if (found == -1 && token != "")
				res.push_back(token);

			if (delimiters[0] == ' ')
				res.push_back(" ");
		}

		return res;
	}
}

#include "olcPixelGameEngine.h"
