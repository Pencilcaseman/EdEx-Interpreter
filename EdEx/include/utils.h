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

	inline std::vector<std::string> splitStringByChars(const std::string &string)
	{
		std::vector<std::string> res;
		for (uint64_t i = 0; i < string.length(); i++)
			res.emplace_back(std::string(1, string[i]));
		return res;
	}

	inline std::string replaceString(const std::string &main, const std::string &find, const std::string &replace)
	{
		std::string res = main;

		size_t index = 0;
		while (true)
		{
			index = res.find(find, index);
			if (index == std::string::npos) break;
			res.replace(index, find.length(), replace);
			index += replace.length();
		}

		return res;
	}

	inline std::vector<std::string> splitString(const std::string &line, const std::vector<char> &delimiters)
	{
		std::vector<std::string> res;
		std::istringstream stream(line);
		std::string token;

		while (std::getline(stream, token, delimiters[0]))
		{
			bool found = false;
			for (uint64_t i = 1; i < delimiters.size(); i++)
			{
				if (token.find_first_of(delimiters[i]) != std::string::npos)
				{
					auto split = splitStringByChars(token);
					res.insert(res.end(), split.begin(), split.end());
					found = true;
					break;
				}
			}

			if (!found)
				res.insert(res.end(), {token});

			res.insert(res.end(), {" "});
		}

		return res;
	}
}

#include "olcPixelGameEngine.h"
