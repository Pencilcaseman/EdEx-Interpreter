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
#include <map>

namespace edex
{
	inline double seconds()
	{
		return (double) std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000;
	}

	inline std::string substring(const std::string &string, uint64_t start, uint64_t end, bool ignoreWhitespace)
	{
		uint64_t whitespace = 0;
		
		if (ignoreWhitespace)
		{
			char c = string[whitespace];
			while (c == ' ')
			{
				whitespace++;
				c = string[whitespace];
			}
		}

		std::string res(string.begin() + start + whitespace, string.begin() + std::min(end + whitespace, string.length()));
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

	template<typename a, typename b>
	inline std::pair<bool, b> findInMap(const std::map<a, b> &map, const a &key)
	{
		auto it = map.find(key);
		if (it != map.end())
			return std::make_pair(true, it->second);
		return std::make_pair(false, b());
	}
}

#include "olcPixelGameEngine.h"
