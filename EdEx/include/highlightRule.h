#pragma once

namespace edex
{
	typedef struct Rule
	{
		std::string regex;
		std::vector<std::pair<std::string, olc::Pixel>> format;
		bool checkTokens = false;
	} Rule;
	
	typedef struct Rules
	{
		std::vector<char> delimiters;
		std::vector<Rule> rule;
	} Rules;
	
	inline Rules generateRules()
	{
		std::vector<char> delimiters = {' ', '+', '-', '*', '/', '=', '<', '>'};
	
		std::vector<Rule> rules;
	
		rules.emplace_back(Rule{"\\bSET\\b\\s.*\\s\\bTO\\b\\s\\.*", {{"SET", {194, 89, 234}}, {"TO", {194, 89, 234}}}, false}); // SET x TO y
		rules.emplace_back(Rule{"\\bFOR\\b\\s.*\\s\\bFROM\\b\\s.*\\s\\bTO\\b\\s.*\\s\\bDO\\b", {{"FOR", {179, 23, 103}}, {"FROM", {179, 23, 103}}, {"TO", {179, 23, 103}}, {"DO", {179, 23, 103}}}, false}); // FOR x FROM a TO b DO
		rules.emplace_back(Rule{"\\bEND\\b\\s\\bFOR\\b", {{"END", {179, 23, 103}}, {"FOR", {179, 23, 103}}}, false}); // END FOR
		rules.emplace_back(Rule{"\\bEND\\b\\s\\bWHILE\\b", {{"END", {179, 23, 103}}, {"WHILE", {179, 23, 103}}}, false}); // END WHILE
		rules.emplace_back(Rule{"\\bEND\\b\\s\\bREPEAT\\b", {{"END", {179, 23, 103}}, {"REPEAT", {179, 23, 103}}}, false}); // END REPEAT
	
		rules.emplace_back(Rule{"\b", {{"AND", {219, 121, 58}}, {"OR", {219, 121, 58}}, {"NOT", {219, 121, 58}}, {"XOR", {219, 121, 58}}, {"MOD", {219, 121, 58}}}, true});
		// rules.emplace_back(Rule{"\b", {{}}, true});
	
		return {delimiters, rules};
	}
}
