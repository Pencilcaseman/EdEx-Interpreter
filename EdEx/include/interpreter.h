#pragma once

#include "mathParser.h"

namespace edex
{
	class ResultContainer
	{
	public:
		std::string infoString;
		uint64_t infoInt;
		double infoDouble;
		bool isError;

		ResultContainer(const std::string &sInfo, uint64_t iInfo, double dInfo, bool error) : infoString(sInfo), infoInt(iInfo), infoDouble(dInfo), isError(error)
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

	class Interpreter
	{
	public:
		std::vector<std::string> programString;
		std::vector<Instruction> compiled;

		std::map<std::string, std::shared_ptr<Object>> heap;
		std::vector<Output> output;

		std::vector<std::string> operatorSplit = {" ", "(", ")", "+", "-", "*", "/", "^", "%"};

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

				if (foundString)
					return ResultContainer("No support for string math yet", 0, 0, true);

				if (foundInt && !foundFloat && !foundDivision)
					return ResultContainer("int", 0, 0, false);

				if (foundInt && !foundFloat && foundDivision)
					return ResultContainer("float", 0, 0, false);

				if (foundFloat)
					return ResultContainer("float", 0, 0, false);

				if (foundVariable)
					return ResultContainer("variable", 0, 0, false);
			}

			return ResultContainer("Unknown Type", 0, 0, true);
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

			if (assignType == "int" || assignType == "float")
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

			compiled.emplace_back(Instruction("SEND", {value}, {destination}, ""));

			return ResultContainer("", 0, 0, false);
		}

		inline std::shared_ptr<Object> calculateExpression(rapid::ExpressionSolver &solver, const std::map<std::string, std::shared_ptr<Object>> variables, const std::string &type) const
		{
			// Process the data to insert variable values
			for (auto &val : solver.processed)
			{
				if (!val.second.empty())
				{
					if (std::find(operatorSplit.begin(), operatorSplit.end(), val.second) != operatorSplit.end())
						continue;

					val.first = variables.at(val.second)->castToFloat();
					val.second = "";
				}
			}

			auto res = solver.eval();

			if (type == "int")
				return std::make_shared<EdExInt>((int64_t) res);
			if (type == "float")
				return std::make_shared<EdExFloat>(res);

			std::cout << "Unable to calculate expression\n";
			exit(1);
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
					heap[instruction.vars[0]] = calculateExpression(instruction.solver, heap, instruction.typeInfo);
				}
				else if (instruction.name == "SEND")
				{
					std::cout << "Interpreter Output (" << instruction.info[0] << "): " << heap.at(instruction.vars[0])->castToString() << "\n";
					output.emplace_back(Output(heap.at(instruction.vars[0])->castToString(), instruction.info[0]));
				}
			}

			return ResultContainer("", 0, 0., false);
		}
	};
}
