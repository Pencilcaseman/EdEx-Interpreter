#pragma once

#include "mathParser.h"

namespace edex
{
	class Object
	{
	public:
		uint64_t scope = 0;

		virtual inline int64_t castToInt() const = 0;
		virtual inline double castToFloat() const = 0;
		virtual inline std::string castToString() const = 0;
		virtual inline bool castToBool() const = 0;

		virtual std::string getType() const = 0;
	};

	class EdExInt : public Object
	{
	public:
		int64_t value;

		EdExInt(int64_t val) : value(val)
		{}

		EdExInt(const double &val) : value((double) val)
		{}

		EdExInt(const std::string &val) : value(std::stoll(val))
		{}

		int64_t castToInt() const override
		{
			return value;
		}

		double castToFloat() const override
		{
			return (double) value;
		}

		std::string castToString() const override
		{
			return std::to_string(value);
		}

		bool castToBool() const override
		{
			return value;
		}

		std::string getType() const override
		{
			return "int";
		}
	};

	class EdExFloat : public Object
	{
	public:
		double value;

		EdExFloat(int64_t val) : value((double) val)
		{}
		
		EdExFloat(double val) : value(val)
		{}

		EdExFloat(const std::string &val) : value(std::stod(val))
		{}

		int64_t castToInt() const override
		{
			return (int64_t) value;
		}

		double castToFloat() const override
		{
			return value;
		}

		std::string castToString() const override
		{
			return std::to_string(value);
		}

		bool castToBool() const override
		{
			return value;
		}

		std::string getType() const override
		{
			return "float";
		}
	};

	class EdExString : public Object
	{
	public:
		std::string value;

		EdExString(const int64_t &val) : value(std::to_string(val))
		{}

		EdExString(const double &val) : value(std::to_string(val))
		{}
		
		EdExString(const std::string &val) : value(val)
		{}

		int64_t castToInt() const override
		{
			return std::stoll(value);
		}

		double castToFloat() const override
		{
			return std::stod(value);
		}

		std::string castToString() const override
		{
			return value;
		}

		bool castToBool() const override
		{
			if (value == "TRUE")
				return true;
			return false;
		}

		std::string getType() const override
		{
			return "string";
		}
	};

	class EdExBool : public Object
	{
	public:
		bool value;

		EdExBool(const bool &val) : value(val)
		{}

		int64_t castToInt() const override
		{
			return value;
		}

		double castToFloat() const override
		{
			return (double) value;
		}

		std::string castToString() const override
		{
			return value ? "TRUE" : "FALSE";
		}

		bool castToBool() const override
		{
			return value;
		}

		std::string getType() const override
		{
			return "bool";
		}
	};

	class ResultContainer
	{
	public:
		std::string infoString;
		uint64_t infoInt;
		double infoDouble;
		std::shared_ptr<Object> objPtr;
		bool isError;

		ResultContainer(const std::string &sInfo, uint64_t iInfo, double dInfo, bool error) : infoString(sInfo), infoInt(iInfo), infoDouble(dInfo), isError(error)
		{}

		ResultContainer(const std::string &sInfo, uint64_t iInfo, double dInfo, std::shared_ptr<Object> pObj, bool error) : infoString(sInfo), infoInt(iInfo), infoDouble(dInfo), objPtr(pObj), isError(error)
		{}
	};

	class Instruction
	{
	public:
		uint64_t scope;
		std::string name;
		std::vector<std::string> vars;
		std::vector<std::string> info;
		std::string typeInfo;
		rapid::ExpressionSolver solver;

		Instruction(const std::string &sName,
					const std::vector<std::string> &lVars,
					const std::vector<std::string> &lInfo,
					const std::string &sType) :
			name(sName), vars(lVars), info(lInfo), typeInfo(sType), scope(-1)
		{}

		Instruction(const std::string &sName,
					const std::vector<std::string> &lVars,
					const std::vector<std::string> &lInfo,
					const std::string &sType,
					const rapid::ExpressionSolver &cSolver) :
			name(sName), vars(lVars), info(lInfo), typeInfo(sType), solver(cSolver), scope(-1)
		{}
	};

	class Output
	{
	public:
		std::string line;
		std::string destination;

		Output(const std::string &out, const std::string &dest) :
			line(out), destination(dest)
		{}
	};

	class Interpreter
	{
	public:
		std::vector<std::string> programString;
		std::vector<Instruction> compiled;

		std::map<std::string, std::shared_ptr<Object>> heap;
		std::vector<Output> output;

		std::vector<std::string> operatorSplit = {" ", "(", ")", "+", "-", "*", "/", "^", "%", "&", "<", ">", "="};

		Interpreter() = default;

		Interpreter(const std::vector<std::string> &toRun) : programString(toRun)
		{}

		template<typename t>
		inline std::shared_ptr<Object> createObject(const t &val, const std::string &type) const
		{
			if (type == "int")
				return std::make_shared<EdExInt>(val);
			if (type == "float")
				return std::make_shared<EdExFloat>(val);
			if (type == "string")
				return std::make_shared<EdExString>(val);
			if (type == "bool")
				return std::make_shared<EdExBool>(val);
		}

		inline ResultContainer extractType(const std::string &term, const std::map<std::string, std::shared_ptr<Object>> &variables = {}, bool ignoreStageTwo = false, bool foundQuote = false) const
		{
			if (term.empty())
				return ResultContainer("NONE", 0, 0., false);

			auto stringEnd = term.length() - 1;
			if (rapid::isnum(term) && term.find_first_of('.') == std::string::npos)
				return ResultContainer("int", 0, 0, false);      // Integer
			if (rapid::isnum(term))
				return ResultContainer("float", 0, 0, false);    // Float
			if (term == "TRUE" || term == "FALSE")
				return ResultContainer("bool", 0, 0, false);	 // Boolean

			if (rapid::isalphanum(term))						 // Variable
			{
				// Search variables
				for (const auto &variable : variables)
					if (term == variable.first)
						return ResultContainer(variable.second->getType(), 0, 0., false);

				return ResultContainer("variable", 0, 0, false);
			}

			if ((term[0] == term[stringEnd] || foundQuote) && (term[stringEnd] == '\"' || term[stringEnd] == '\''))
				return ResultContainer("string", 0, 0, false);   // String

			if (!ignoreStageTwo)
			{
				// Check for valid variables or scalars
				auto split = rapid::splitString(term, operatorSplit);
				bool valid = true;
				bool foundInt = false;
				bool foundFloat = false;
				bool foundBool = false;
				bool foundString = false;
				bool foundVariable = false;
				bool foundDivision = false;

				for (const auto &val : split)
				{
					// Check it is not a delimiter
					if (val == "/")
						foundDivision = true;
					else if (val == "\"" || val == "\'")
					{
						foundQuote = true;
						continue;
					}

					if (std::find(operatorSplit.begin(), operatorSplit.end(), val) != operatorSplit.end())
						continue;

					auto res = extractType(val, variables, true, foundQuote);
					if (res.isError)
						return res;

					if (res.infoString == "int")
						foundInt = true;
					else if (res.infoString == "float")
						foundFloat = true;
					else if (res.infoString == "bool")
						foundBool = true;
					else if (res.infoString == "string")
						foundString = true;
					else if (res.infoString == "variable")
						foundVariable = true;
					else if (res.infoString != "NONE")
						return ResultContainer("Invalid Expression", 0, 0, true);
				}

				if (foundInt && !foundFloat && !foundDivision && !foundString)
					return ResultContainer("int", 0, 0., false);

				if (foundInt && !foundFloat && foundDivision && !foundString)
					return ResultContainer("float", 0, 0., false);

				if (foundFloat && !foundString)
					return ResultContainer("float", 0, 0., false);

				if (foundBool)
					return ResultContainer("bool", 0, 0., false);

				if (foundString)
					return ResultContainer("string", 0, 0., false);

				if (foundVariable)
				{
					// Search variables
					for (const auto &variable : variables)
						if (term == variable.first)
							return ResultContainer(variable.second->getType(), 0, 0., false);

					return ResultContainer("variable", 0, 0., false);
				}
			}

			return ResultContainer("Unknown Type or Syntax", 0, 0, true);
		}

		inline ResultContainer evaluateStringExpression(const std::string &expression) const
		{
			// Evaluate expressions involving string variables
			// >>> "HELLO"
			// >>> "HELLO " & "THERE"
			// >>> "Hi " * 100

			std::string res;

			auto split = rapid::splitString(expression, operatorSplit);

			bool isConcatenate = true;
			bool foundQuote = false;
			bool addSpace = false;
			bool isVariable = false;

			uint64_t index = 0;
			for (auto &val : split)
			{
				isVariable = false;

				if (val.empty())
					continue;
				if (val == " ")
					continue;

				if (val == "\"" || val == "\'")
				{
					foundQuote = true;
				}
				else
				{
					bool valid = false;

					// Search variables
					for (const auto &var : heap)
					{
						if (val == var.first)
						{
							val = var.second->castToString();
							isVariable = true;
							valid = true;
							addSpace = false;
							break;
						}
					}

					if (val[0] == val[val.length() - 1] && (val[0] == '\"' || val[0] == '\''))
						valid = true;
					else if ((val[val.length() - 1] == '\"' || val[val.length() - 1] == '\'') && foundQuote)
					{
						valid = true;
						addSpace = true;
						isConcatenate = true;
					}
					else
					{
						if (val != "&")
						{
							for (uint64_t i = index; i < split.size(); i++)
							{
								if (split[i] == "\"" || split[i] == "\'")
								{
									valid = true;
									addSpace = true;
								}
							}
						}
					}

					if (valid && isConcatenate)
					{
						if (addSpace && index > 0)
							res += " ";
						res += substring(val, isVariable ? 0 : !foundQuote, val.length() - (val[val.length() - 1] == '\"' || val[val.length() - 1] == '\''), false);
						isConcatenate = false;
						foundQuote = false;
						addSpace = false;
					}
					else
					{
						bool isVariable = false;

						for (const auto &variable : heap)
						{
							if (variable.first == val)
							{
								if (addSpace)
									res += " ";

								res += variable.second->castToString();
								isVariable = true;
								isConcatenate = false;
								foundQuote = false;
								addSpace = false;
								break;
							}
						}

						if (val[0] == '&') // && !isVariable)
						{
							if (isConcatenate)
								return ResultContainer("Invalid Syntax", 0, 0., true);
							isConcatenate = true;
							foundQuote = false;
							addSpace = false;
						}
					}
				}

				index++;
			}

			if (addSpace)
				res += " ";

			return ResultContainer("", 0, 0., std::make_shared<EdExString>(res), false);
		}

		inline ResultContainer evaluateEquality(const double &lhs, const double &rhs, const std::string &op) const
		{
			// Evaluate the result of a binary operation
			// E.g. x > y
			// E.g. a <> b

					// Compare values
			if (op == "=")
				return ResultContainer("", lhs == rhs, 0., false);
			if (op == "<")
				return ResultContainer("", lhs < rhs, 0., false);
			if (op == ">")
				return ResultContainer("", lhs > rhs, 0., false);
			if (op == "<=")
				return ResultContainer("", lhs <= rhs, 0., false);
			if (op == ">=")
				return ResultContainer("", lhs >= rhs, 0., false);
			if (op == "<>")
				return ResultContainer("", lhs != rhs, 0., false);

			return ResultContainer("Invalid Operation: " + op, 0, 0., true);
		}

		inline ResultContainer evaluateBooleanExpression(const std::string &string) const
		{
			// Search for value
			if (string == "TRUE")
				return ResultContainer("", 0, 0., std::make_shared<EdExBool>(1), false);
			if (string == "FALSE")
				return ResultContainer("", 0, 0., std::make_shared<EdExBool>(0), false);

			// Search for variable
			for (const auto &var : heap)
				if (string == var.first)
					return ResultContainer("", 0, 0., var.second, false);

			auto split = rapid::splitString(string, operatorSplit);

			// Search for an operator
			std::string operation;
			uint64_t operatorsFound = 0;
			for (uint64_t i = 0; i < split.size() - 1; i++)
			{
				bool found = false;
				auto len = operation.size();

				// Check for the main token segment
				if (split[i] == ">" || split[i] == "<" || split[i] == "=")
				{
					operation += split[i]; found = true;
					operatorsFound++;
				}

				// Check for the (potential) second part of the token segment
				if (found && (split[i + 1] == ">" || split[i + 1] == "="))
				{
					operation += split[i + 1];
				}

				if (operatorsFound > 1)
					return ResultContainer("Cannot have multiple conditional operators in one expression", 0, 0., true);
			}

			if (operatorsFound == 0)
			{
				auto spaceEnd = string.find_first_not_of(' ');
				auto spaceStart = string.find_last_not_of(' ');

				if (spaceEnd == std::string::npos) spaceEnd = 0;

				if (spaceStart != std::string::npos && spaceStart < string.length() - 1) spaceStart++;
				else spaceStart = 0;

				auto solver = rapid::ExpressionSolver(std::string(string.begin() + spaceEnd, string.end() - spaceStart));
				solver.compile();
				return calculateExpression(solver, heap, extractType(string, heap).infoString);
			}

			auto lhsString = std::string(string.begin(), string.begin() + string.find(operation));
			auto rhsString = std::string(string.begin() + string.find(operation) + operation.length(), string.end());

			std::cout << "Left: " << lhsString << "\n";
			std::cout << "Right: " << rhsString << "\n";

			auto lhsSolver = rapid::ExpressionSolver(lhsString);
			auto rhsSolver = rapid::ExpressionSolver(rhsString);

			lhsSolver.compile(); rhsSolver.compile();

			std::string lhsType = extractType(lhsString, heap).infoString;
			std::string rhsType = extractType(rhsString, heap).infoString;

			auto lhsRes = calculateExpression(lhsSolver, heap, lhsType);
			auto rhsRes = calculateExpression(rhsSolver, heap, rhsType);

			std::cout << "Left res: " << lhsRes.objPtr->castToString() << "\n";
			std::cout << "Right res: " << rhsRes.objPtr->castToString() << "\n";

			auto res = evaluateEquality(lhsRes.infoDouble, rhsRes.infoDouble, operation);

			return ResultContainer("", 0, 0., std::make_shared<EdExBool>(res.infoDouble), false);
		}

		inline ResultContainer parseSetVar(const std::string &line)
		{
			// Parse "SET x TO y"
			std::string variable;	// "x"
			std::string expression; // "y"
			std::string assignType; // Type of "y"

			uint64_t to = line.find("TO");

			variable = substring(line, 4, to - 1, true);
			expression = substring(line, to + 3, line.length(), true);

			auto assignTypeContainer = extractType(expression);
			if (assignTypeContainer.isError)
				return assignTypeContainer;

			assignType = assignTypeContainer.infoString;

			auto solver = rapid::ExpressionSolver(expression);
			solver.expressionToInfix();
			solver.infixToPostfix();
			solver.postfixProcess();

			if (assignType == "int" || assignType == "float" || assignType == "string")
				compiled.emplace_back(Instruction("SET", {variable}, {}, assignType, solver));
			else if (assignType == "variable")
				compiled.emplace_back(Instruction("SET", {variable, expression}, {}, assignType, solver));
			else if (assignType == "bool")
				compiled.emplace_back(Instruction("SET", {variable, expression}, {}, assignType, solver));
			else
				return ResultContainer("Unknown assignment type", 0, 0, true);

			return ResultContainer("", 0, 0, false);
		}

		inline ResultContainer parseSendTo(const std::string &line)
		{
			// Parse "SEND x TO y"

			std::string value;
			std::string destination;

			uint64_t to = line.find("TO");

			value = substring(line, 5, to - 1, true);
			destination = substring(line, to + 3, line.length(), true);

			auto assignTypeContainer = extractType(value);
			if (assignTypeContainer.isError)
				return assignTypeContainer;

			auto solver = rapid::ExpressionSolver(value);
			solver.compile();

			compiled.emplace_back(Instruction("SEND", {value}, {destination, rapid::isalphanum(value) ? "" : " "}, assignTypeContainer.infoString, solver));

			return ResultContainer("", 0, 0, false);
		}

		inline ResultContainer calculateExpression(rapid::ExpressionSolver &solver, const std::map<std::string, std::shared_ptr<Object>> variables, const std::string &type) const
		{
			// Process the data to insert variable values
			bool foundString = false;
			bool foundBool = false;
			bool foundQuote = false;
			uint64_t index = 0;

			for (auto &val : solver.processed)
			{
				if (!val.second.empty()) // Not a numeric type
				{
					bool found = false;

					if (val.second[0] == val.second[val.second.length() - 1] && (val.second[0] == '\"' || val.second[0] == '\''))
					{
						foundString = true;
						found = true;
					}
					else if ((val.second[val.second.length() - 1] == '\"' || val.second[val.second.length() - 1] == '\'') && foundQuote)
					{
						foundString = true;
						found = true;
					}
					else
					{
						for (uint64_t i = index; i < solver.processed.size(); i++)
						{
							if (solver.processed[i].second == "\"" || solver.processed[i].second == "\'")
							{
								foundString = true;
								found = true;
							}
						}
					}

					if (!found) // Not a simple string type
					{
						// if (val.second == ">" || val.second == "<" || val.second == "=")
						// 	foundBool = true;

						if (std::find(operatorSplit.begin(), operatorSplit.end(), val.second) != operatorSplit.end() || foundString)
							continue;

						// Check for boolean value
						if (val.second == "TRUE" || val.second == "FALSE")
						{
							foundBool = true;
							val.first = val.second == "TRUE";
							val.second = "";
						}
						else
						{
							bool varExists = false;
							for (const auto &var : heap)
							{
								if (var.first == val.second)
								{
									varExists = true; break;
								}
							}

							if (!varExists)
								return ResultContainer("Variable Not Found: " + val.second, 0, 0., true);

							auto variableType = variables.at(val.second)->getType();
							if (variableType != "string")
							{
								if (variableType == "bool")
								{
									foundBool = true;
									val.first = val.second == "TRUE";
									val.second = "";
								}
								else
								{
									val.first = variables.at(val.second)->castToFloat();
									val.second = "";
								}
							}
							else
							{
								foundString = true;
								val.first = 0;
								val.second = variables.at(val.second)->castToString();
							}
						}
					}
				}

				index++;
			}

			if (foundString)
				return evaluateStringExpression(solver.expression);

			if (foundBool || type == "bool")
				return evaluateBooleanExpression(solver.expression);

			// ====================================================================
			for (const auto &variable : variables)
			{
				solver.variables[variable.first] = variable.second->castToFloat();
			}
			// ====================================================================

			solver.compile();
			auto res = solver.eval();

			if (type == "int")
				return ResultContainer("int", 0, res, std::make_shared<EdExInt>((int64_t) res), false);
			if (type == "float")
				return ResultContainer("float", 0, res, std::make_shared<EdExFloat>(res), false);
			if (type == "variable")
			{
				auto resType = extractType(solver.expression, variables);

				if (resType.isError)
					return ResultContainer("Invalid Syntax. Unable to evaluate expression", 0, 0., true);

				if (resType.infoString != type)
				{
					if (foundBool)
						return ResultContainer(resType.infoString, 0, res, createObject(res, "bool"), false);
					return ResultContainer(resType.infoString, 0, res, createObject(res, resType.infoString), false);
				}

				auto &var = variables.at(solver.expression);
				return ResultContainer(var->getType(), 0, res, var, false);
			}

			return ResultContainer("Invalid Syntax. Unable to evaluate expression", 0, 0., true);
		}

		inline ResultContainer compile()
		{
			// Compile the program string into a set of instructions that can be run.
			// This is done to make the running process faster, as the code does not
			// need to be parsed and interpreted at runtime.

			for (const auto &line : programString)
			{
				if (substring(line, 0, 3, true) == "SET")
				{
					// Check for "SET x TO y"

					auto result = parseSetVar(line);
					if (result.isError)
						return result;
				}
				else if (substring(line, 0, 4, true) == "SEND")
				{
					// Check for "SEND x TO y"

					auto result = parseSendTo(line);
					if (result.isError)
						return result;
				}
			}

			return ResultContainer("", 0, 0., false);
		}

		inline ResultContainer interpret()
		{
			for (auto &instruction : compiled)
			{
				if (instruction.name == "SET")
				{
					auto res = calculateExpression(instruction.solver, heap, instruction.typeInfo);
					if (res.isError)
						return res;

					heap[instruction.vars[0]] = res.objPtr;
				}
				else if (instruction.name == "SEND")
				{
					if (instruction.info[0] != "DISPLAY")
						return ResultContainer("Invalid Output Location", 0, 0., true);

					if (!instruction.info[1].empty())
					{
						auto res = calculateExpression(instruction.solver, heap, instruction.typeInfo);
						if (res.isError)
							return res;

						output.emplace_back(Output(res.objPtr->castToString(), instruction.info[0]));
					}
					else
					{
						output.emplace_back(Output(heap.at(instruction.vars[0])->castToString(), instruction.info[0]));
					}

					std::cout << "Interpreter Output: " << output[output.size() - 1].line << "\n";
				}
			}

			return ResultContainer("", 0, 0., false);
		}
	};
}
