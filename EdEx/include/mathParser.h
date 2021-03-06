// Code taken from Rapid (https://github.com/Pencilcaseman/Rapid)

#pragma once

#include <map>
#include <unordered_map>
#include <stack>
#include <vector>
#include <functional>
#include <regex>
#include <string>

namespace rapid
{
	std::vector<std::string> splitString(const std::string &string, const std::vector<std::string> delimiters = {" "})
	{
		std::vector<std::string> res;

		uint64_t start = 0;
		uint64_t end = 0;

		while (end != std::string::npos)
		{
			// Find the nearest delimiter
			uint64_t nearest = -1;
			uint64_t index = 0;

			for (uint64_t i = 0; i < delimiters.size(); i++)
			{
				auto pos = string.find(delimiters[i], start);
				if (pos != std::string::npos && pos < nearest)
				{
					nearest = pos;
					index = i;
				}
			}

			if (nearest == (uint64_t) -1) // Nothing else was found
				break;

			end = nearest;
			res.emplace_back(std::string(string.begin() + start, string.begin() + end));
			res.emplace_back(delimiters[index]);
			start = end + 1;
		}

		if (start < string.length())
			res.emplace_back(std::string(string.begin() + start, string.end()));

		return res;
	}

	inline bool isalphanum(const std::string &string)
	{
		uint64_t i = 0;
		for (const auto &c : string)
		{
			if (!((i == 0 && (c == '-' || c == '+') && string.length() > 1) || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '.')))
				return false;
			i++;
		}
		return true;
	}

	inline bool isalpha(const std::string &string)
	{
		for (const auto &c : string)
			if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
				return false;
		return true;
	}

	inline bool isnum(const std::string &string)
	{
		uint64_t i = 0;
		for (const auto &c : string)
		{
			if (!((i == 0 && (c == '-' || c == '+') && string.length() > 1) || (c >= '0' && c <= '9') || (c == '.')))
				return false;
			i++;
		}
		return true;
	}

	class ExpressionSolver
	{
	public:
		std::string expression;
		std::vector<std::string> infix;
		std::vector<std::string> postfix;
		std::vector<std::pair<double, std::string>> processed;

		// std::vector<std::string> splitBy = {" ", "(", ")", "+", "-", "*", "/", "^", "%"};
		std::vector<std::string> splitBy = {" ", "(", ")", ">", "<", "=", "!", "+", "-", "*", "/", "^", "%"};
		std::unordered_map<std::string, double> variables;

		// Basic math operations
		std::vector<std::string> operators = {"^", "*", "/", "%", "-", "+"};

		// Functions and their definitions
		std::vector<std::string> functionNames;
		std::vector<std::function<double(double)>> functionDefinitions;

		// Tracking
		bool errorOccured = false;
		bool containsEquality = false;

	public:

		ExpressionSolver() = default;

		ExpressionSolver(const std::string &expr) : expression(expr)
		{
			operators.insert(operators.begin(), functionNames.begin(), functionNames.end());

			// Register common functions
			registerFunction("sin", [](double x)
			{
				return std::sin(x);
			});

			registerFunction("cos", [](double x)
			{
				return std::cos(x);
			});

			registerFunction("tan", [](double x)
			{
				return std::tan(x);
			});

			registerFunction("asin", [](double x)
			{
				return std::asin(x);
			});

			registerFunction("acos", [](double x)
			{
				return std::acos(x);
			});

			registerFunction("atan", [](double x)
			{
				return std::atan(x);
			});

			registerFunction("exp", [](double x)
			{
				return std::exp(x);
			});
		}

		inline void expressionToInfix()
		{
			uint64_t i = 0;
			bool append = false;
			for (const auto &term : splitString(expression, splitBy))
			{
				if (term != " " && !term.empty())
				{
					if (!append)
						infix.emplace_back(term);
					else
					{
						infix[i - 1] += term;
						append = false;
					}

					if (i == 0 && (term == "+" || term == "-"))
						append = true;
					else if (i > 1 && (term == "+" || term == "-") && (std::find(operators.end() - 6, operators.end(), infix[infix.size() - 2]) != operators.end()))
						append = true;
				}

				i++;
			}
		}

		inline void processInfix()
		{
			std::vector<std::string> newInfix;

			for (uint64_t i = 0; i < infix.size(); i++)
			{
				bool modified = false;

				if (infix[i] == ">")
				{
					if (i < infix.size() - 1 && infix[i + 1] == "=")
					{
						newInfix.emplace_back(">=");
						modified = true;
						i++;
					}
					else
					{
						newInfix.emplace_back(">");
						modified = true;
					}
				}
				else if (infix[i] == "<")
				{
					if (i < infix.size() - 1 && infix[i + 1] == "=")
					{
						newInfix.emplace_back("<=");
						modified = true;
						i++;
					}
					else if (i < infix.size() - 1 && infix[i + 1] != ">")
					{
						newInfix.emplace_back("<");
						modified = true;
					}
				}
				else if (infix[i] == "=")
				{
					newInfix.emplace_back("=");
					modified = true;
				}
				else if (i < infix.size() - 1 && infix[i] == "<" && infix[i + 1] == ">")
				{
					newInfix.emplace_back("<>");
					modified = true;
					i++;
				}

				if (!modified)
					newInfix.emplace_back(infix[i]);
			}

			infix = newInfix;
		}

		inline void infixToPostfix()
		{
			std::stack<std::string> stack;

			for (const auto &token : infix)
			{
				auto it = std::find(functionNames.begin(), functionNames.end(), token);

				if (it == functionNames.end() && isalphanum(token))
					postfix.emplace_back(token);
				else if (token == "(" || token == "^")
					stack.push(token);
				else if (token == ")")
				{
					while (!stack.empty() && stack.top() != "(")
					{
						postfix.emplace_back(stack.top());
						stack.pop();
					}
					stack.pop();
				}
				else
				{
					while (!stack.empty() && std::find(operators.begin(), operators.end(), token) >= std::find(operators.begin(), operators.end(), stack.top()))
					{
						postfix.emplace_back(stack.top());
						stack.pop();
					}
					stack.push(token);
				}
			}

			while (!stack.empty() > 0)
			{
				postfix.emplace_back(stack.top());
				stack.pop();
			}
		}

		inline void postfixProcess()
		{
			for (const auto &term : postfix)
			{
				if (isnum(term))
					processed.emplace_back(std::make_pair(std::stod(term), ""));
				else
					processed.emplace_back(std::make_pair(0., term));
			}
		}

		inline double postfixEval()
		{
			std::stack<double> stack;

			for (const auto &term : processed)
			{
				bool evaluated = false;

				if (term.second.length() == 0)
				{
					stack.push(term.first);
					evaluated = true;
				}

				if (!evaluated)
				{
					std::string varname;
					double mult = 1;

					if (term.second[0] == '-')
					{
						varname = std::string(term.second.begin() + 1, term.second.end());
						mult = -1;
					}
					else if (term.second[0] == '+')
					{
						varname = std::string(term.second.begin() + 1, term.second.end());
					}
					else
					{
						varname = term.second;
					}

					if (variables.find(varname) != variables.end())
					{
						stack.push(variables.at(varname) * mult);
						evaluated = true;
					}
					else
					{
						errorOccured = true;
					}
				}

				if (!evaluated)
				{
					double a = 0;
					double b = stack.top(); stack.pop();

					// Function
					for (uint64_t i = 0; i < functionDefinitions.size(); i++)
					{
						if (term.second == functionNames[i])
						{
							stack.push(functionDefinitions[i](b));
							evaluated = true;
							break;
						}
					}

					if (!stack.empty() && !evaluated)
					{
						a = stack.top(); stack.pop();

						if (term.second == "+")
						{
							stack.push(a + b);
							evaluated = true;
						}
						else if (term.second == "-")
						{
							stack.push(a - b);
							evaluated = true;
						}
						else if (term.second == "*")
						{
							stack.push(a * b);
							evaluated = true;
						}
						else if (term.second == "/")
						{
							stack.push(a / b);
							evaluated = true;
						}
						else if (term.second == "^")
						{
							stack.push(std::pow(a, b));
							evaluated = true;
						}
						else if (term.second == "%")
						{
							stack.push(std::fmod(a, b));
							evaluated = true;
						}
						else if (term.second == ">")
						{
							stack.push(a > b);
							containsEquality = true;
							evaluated = true;
						}
						else if (term.second == "<")
						{
							stack.push(a < b);
							containsEquality = true;
							evaluated = true;
						}
						else if (term.second == ">=")
						{
							stack.push(a >= b);
							containsEquality = true;
							evaluated = true;
						}
						else if (term.second == "<=")
						{
							stack.push(a <= b);
							containsEquality = true;
							evaluated = true;
						}
						else if (term.second == "<>")
						{
							stack.push(a != b);
							containsEquality = true;
							evaluated = true;
						}
						else if (term.second == "=")
						{
							stack.push(a == b);
							containsEquality = true;
							evaluated = true;
						}
						else
						{
							errorOccured = true;
						}
					}
				}
			}

			return stack.top();
		}

		template<typename lambda>
		inline void registerFunction(const std::string &name, const lambda &func)
		{
			functionNames.emplace_back(name);
			functionDefinitions.emplace_back(std::function<double(double)>(func));

			operators.insert(operators.begin(), name);
		}

		inline void compile()
		{
			infix = {};
			postfix = {};
			processed = {};
			expressionToInfix();
			processInfix();
			infixToPostfix();
			postfixProcess();
		}

		inline double eval()
		{
			return postfixEval();
		}

		operator bool() const
		{
			return errorOccured;
		}
	};
}
