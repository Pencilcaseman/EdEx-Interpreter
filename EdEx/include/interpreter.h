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

		virtual inline std::string type() const = 0;
	};

	class EdExInt : public Object
	{
	public:
		int64_t value;

		EdExInt(int64_t val) : value(val)
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

		std::string type() const override
		{
			return "int";
		}
	};

	class EdExFloat : public Object
	{
	public:
		double value;

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

		std::string type() const override
		{
			return "float";
		}
	};

	class EdExString : public Object
	{
	public:
		std::string value;

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

		std::string type() const override
		{
			return "string";
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

		std::vector<std::string> operatorSplit = {" ", "(", ")", "+", "-", "*", "/", "^", "%", "&"};

		Interpreter() = default;

		Interpreter(const std::vector<std::string> &toRun) : programString(toRun)
		{}

		inline ResultContainer extractType(const std::string &term, bool ignoreStageTwo = false) const
		{
			// Check for int type
			if (rapid::isnum(term) && term.find_first_of('.') == std::string::npos)
				return ResultContainer("int", 0, 0, false);
			else if (rapid::isnum(term))
				return ResultContainer("float", 0, 0, false);
			else if (rapid::isalphanum(term))
				return ResultContainer("variable", 0, 0, false);
			else if (term[0] == term[term.length() - 1] && (term[0] == '\"' || term[0] == '\''))
				return ResultContainer("string", 0, 0, false);

			if (!ignoreStageTwo)
			{
				// Check for valid variables or scalars
				auto split = rapid::splitString(term, operatorSplit);
				bool valid = true;
				bool foundInt = false;
				bool foundFloat = false;
				bool foundString = false;
				bool foundVariable = false;
				bool foundDivision = false;

				for (const auto &val : split)
				{
					// Check it is not a delimiter
					if (val == "/")
						foundDivision = true;

					if (std::find(operatorSplit.begin(), operatorSplit.end(), val) != operatorSplit.end())
						continue;

					auto res = extractType(val, true);
					if (res.isError)
						return res;

					if (res.infoString == "int")
						foundInt = true;
					else if (res.infoString == "float")
						foundFloat = true;
					else if (res.infoString == "string")
						foundString = true;
					else if (res.infoString == "variable")
						foundVariable = true;
					else
						return ResultContainer("Invalid Expression", 0, 0, true);
				}

				if (foundInt && !foundFloat && !foundDivision)
					return ResultContainer("int", 0, 0., false);

				if (foundInt && !foundFloat && foundDivision)
					return ResultContainer("float", 0, 0., false);

				if (foundFloat)
					return ResultContainer("float", 0, 0., false);

				if (foundString)
					return ResultContainer("string", 0, 0., false);

				if (foundVariable)
					return ResultContainer("variable", 0, 0., false);
			}

			return ResultContainer("Unknown Type", 0, 0, true);
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

			uint64_t index = 0;
			for (const auto &val : split)
			{
				if (val.empty())
					continue;

				if (val == "\"" || val == "\'")
				{
					foundQuote = true;
				}
				else
				{
					bool valid = false;

					if (val[0] == val[val.length() - 1] && (val[0] == '\"' || val[0] == '\''))
						valid = true;
					else if ((val[val.length() - 1] == '\"' || val[val.length() - 1] == '\'') && foundQuote)
					{
						valid = true;
						addSpace = true;
					}
					else
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

					if (valid && isConcatenate)
					{
						if (addSpace && index > 0)
							res += " ";
						res += substring(val, !foundQuote, val.length() - (val[val.length() - 1] == '\"' || val[val.length() - 1] == '\''), false);
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

						if (val[0] == '&' && !isVariable)
						{
							if (isConcatenate)
								return ResultContainer("Invalid Syntax", 0, 0., true);
							isConcatenate = true;
							foundQuote = false;
						}
					}
				}

				index++;
			}

			if (addSpace)
				res += " ";

			return ResultContainer("", 0, 0., std::make_shared<EdExString>(res), false);
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
			else
				assignType = assignTypeContainer.infoString;

			auto solver = rapid::ExpressionSolver(expression);
			solver.expressionToInfix();
			solver.infixToPostfix();
			solver.postfixProcess();

			if (assignType == "int" || assignType == "float" || assignType == "string")
				compiled.emplace_back(Instruction("SET", {variable}, {}, assignType, solver));
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
			bool foundQuote = false;
			uint64_t index = 0;

			for (auto &val : solver.processed)
			{
				if (!val.second.empty())
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

					if (!found)
					{
						if (std::find(operatorSplit.begin(), operatorSplit.end(), val.second) != operatorSplit.end() || foundString)
							continue;

						if (variables.at(val.second)->type() != "string")
						{
							val.first = variables.at(val.second)->castToFloat();
							val.second = "";
						}
						else
						{
							foundString = true;
							val.first = 0;
							val.second = variables.at(val.second)->castToString();
						}
					}
				}

				index++;
			}

			if (foundString)
			{
				return evaluateStringExpression(solver.expression);
			}

			auto res = solver.eval();

			if (type == "int")
				return ResultContainer("int", 0, res, std::make_shared<EdExInt>((int64_t) res), false); // std::make_shared<EdExInt>((int64_t) res);
			if (type == "float")
				return ResultContainer("float", 0, res, std::make_shared<EdExFloat>(res), false);  // std::make_shared<EdExFloat>(res);

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

					std::cout << "Interpreter output: " << output[output.size() - 1].line << "\n";
				}
			}

			return ResultContainer("", 0, 0., false);
		}
	};
}
