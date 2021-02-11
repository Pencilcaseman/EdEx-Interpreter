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
		std::vector<char> delimiters = {' ', '+', '-', '*', '/', '=', '<', '>', '[', ']', '(', ')'};

		std::vector<Rule> rules;

		rules.push_back(Rule{"\\bSEND\\b\\s.*\\s\\bTO\\b\\s\\.*\\bDISPLAY\\b", {{"SEND", {0, 0, 0}}, {"TO", {0, 0, 0}}}, false});
		rules.push_back(Rule{"\\bRECEIVE\\b\\s.*\\s\\bFROM\\b\\s\\(.*\\)\\s\\bKEYBOARD\\b", {{"RECEIVE", {0, 0, 0}}, {"FROM", {0, 0, 0}}, {"KEYBOARD", {0, 0, 0}}}, false});

		rules.push_back(Rule{"\\bWHILE\\b\\s.*\\s\\bDO\\b", {{"WHILE", {0, 0, 0}}, {"DO", {0, 0, 0}}}, false});
		rules.push_back(Rule{"\\bFOR\\b\\s.*\\s\\bFROM\\b\\s.*\\s\\bTO\\b\\s.*\\s\\bDO\\b", {{"FOR", {0, 0, 0}}, {"FROM", {0, 0, 0}}, {"TO", {0, 0, 0}}, {"DO", {0, 0, 0}}}, false});
		rules.push_back(Rule{"\\bREPEAT\\b\\s.*\\s\\bUNTIL\\b", {{"WHILE", {0, 0, 0}}, {"DO", {0, 0, 0}}}, false});

		rules.push_back(Rule{"\\bEND\\b\\s\\bFOR\\b", {{"END", {0, 0, 0}}, {"FOR", {0, 0, 0}}}, false});
		rules.push_back(Rule{"\\bEND\\b\\s\\bWHILE\\b", {{"END", {0, 0, 0}}, {"WHILE", {0, 0, 0}}}, false});
		rules.push_back(Rule{"\\bEND\\b\\s\\bREPEAT\\b\\s\\.*", {{"END", {0, 0, 0}}, {"WHILE", {0, 0, 0}}}, false});

		rules.push_back(Rule{"\\bIF\\b\\s.*\\s\\bTHEN\\b", {{"IF", {0, 0, 0}}, {"THEN", {0, 0, 0}}}, false});

		rules.push_back(Rule{"\\bEND\\b\\s\\bIF\\b", {{"END", {0, 0, 0}}, {"IF", {0, 0, 0}}}, false});

		rules.push_back(Rule{"\\bSET\\b\\s.*\\s\\bTO\\b\\s\\.*", {{"SET", {0, 0, 0}}, {"TO", {0, 0, 0}}}, false});

		rules.push_back(Rule{"", {{"AND", {0, 0, 0}}, {"OR", {0, 0, 0}}, {"NOT", {0, 0, 0}}, {"XOR", {0, 0, 0}}}, true});
		rules.push_back(Rule{"", {{"<", {0, 0, 0}}, {">", {0, 0, 0}}, {"=", {0, 0, 0}}}, true});

		rules.push_back(Rule{"", {{"DIV", {0, 0, 0}}, {"MOD", {0, 0, 0}}}, true});
		rules.push_back(Rule{"", {{"+", {0, 0, 0}}, {"-", {0, 0, 0}}, {"*", {0, 0, 0}}, {"/", {0, 0, 0}}}, true});

		rules.push_back(Rule{"", {{"[", {0, 0, 0}}, {"]", {0, 0, 0}}, {"(", {0, 0, 0}}, {")", {0, 0, 0}}}, true});
		
		rules.push_back(Rule{"", {{"DISPLAY", {0, 0, 0}}}, true});
		rules.push_back(Rule{"", {{"LENGTH", {0, 0, 0}}}, true});
		rules.push_back(Rule{"", {{"ELSE", {0, 0, 0}}}, true});

		return {delimiters, rules};
	}
}
