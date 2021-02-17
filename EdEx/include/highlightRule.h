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

		rules.push_back(Rule{"\\bSEND\\b\\s.*\\s\\bTO\\b\\s\\.*", {{"SEND", {226, 142, 70}}, {"TO", {226, 142, 70}}}, false});
		rules.push_back(Rule{"\\bRECEIVE\\b\\s.*\\s\\bFROM\\b\\s\\(.*\\)\\s\\bKEYBOARD\\b", {{"RECEIVE", {226, 142, 70}}, {"FROM", {226, 142, 70}}, {"KEYBOARD", {226, 142, 70}}}, false});

		rules.push_back(Rule{"\\bWHILE\\b\\s.*\\s\\bDO\\b", {{"WHILE", {169, 100, 164}}, {"DO", {169, 100, 164}}}, false});
		rules.push_back(Rule{"\\bFOR\\b\\s.*\\s\\bFROM\\b\\s.*\\s\\bTO\\b\\s.*\\s\\bDO\\b", {{"FOR", {169, 100, 164}}, {"FROM", {169, 100, 164}}, {"TO", {169, 100, 164}}, {"DO", {169, 100, 164}}}, false});
		rules.push_back(Rule{"\\bREPEAT\\b\\s.*\\s\\bUNTIL\\b", {{"WHILE", {169, 100, 164}}, {"DO", {169, 100, 164}}}, false});

		rules.push_back(Rule{"\\bEND\\b\\s\\bFOR\\b", {{"END", {169, 100, 164}}, {"FOR", {169, 100, 164}}}, false});
		rules.push_back(Rule{"\\bEND\\b\\s\\bWHILE\\b", {{"END", {169, 100, 164}}, {"WHILE", {169, 100, 164}}}, false});
		rules.push_back(Rule{"\\bEND\\b\\s\\bREPEAT\\b\\s\\.*", {{"END", {169, 100, 164}}, {"WHILE",{169, 100, 164}}}, false});

		rules.push_back(Rule{"\\bIF\\b\\s.*\\s\\bTHEN\\b", {{"IF", {109, 116, 189}}, {"THEN", {109, 116, 189}}}, false});

		rules.push_back(Rule{"\\bEND\\b\\s\\bIF\\b", {{"END", {109, 116, 189}}, {"IF", {109, 116, 189}}}, false});

		rules.push_back(Rule{"\\bSET\\b\\s.*\\s\\bTO\\b\\s\\.*", {{"SET", {32, 145, 145}}, {"TO", {32, 145, 145}}}, false});

		rules.push_back(Rule{"", {{"AND", {123, 126, 224}}, {"OR", {123, 126, 224}}, {"NOT", {123, 126, 224}}, {"XOR", {123, 126, 224}}}, true});
		rules.push_back(Rule{"", {{"<", {123, 126, 224}}, {">", {123, 126, 224}}, {"=", {123, 126, 224}}}, true});

		rules.push_back(Rule{"", {{"DIV", {107, 224, 134}}, {"MOD", {107, 224, 134}}}, true});
		rules.push_back(Rule{"", {{"+", {107, 224, 134}}, {"-", {107, 224, 134}}, {"*", {107, 224, 134}}, {"/", {107, 224, 134}}}, true});

		rules.push_back(Rule{"", {{"[", {150, 133, 33}}, {"]", {150, 133, 33}}, {"(", {150, 133, 33}}, {")", {150, 133, 33}}}, true});
		
		rules.push_back(Rule{"", {{"DISPLAY", {34, 164, 32}}}, true});
		rules.push_back(Rule{"", {{"LENGTH", {229, 92, 68}}}, true});
		rules.push_back(Rule{"", {{"ELSE", {109, 116, 189}}}, true});

		rules.push_back(Rule{"", {{"TRUE", {104, 255, 92}}}, true});
		rules.push_back(Rule{"", {{"FALSE", {241, 93, 255}}}, true});

		return {delimiters, rules};
	}
}
