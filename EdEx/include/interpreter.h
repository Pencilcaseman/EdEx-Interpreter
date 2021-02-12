#pragma once

namespace edex
{
	constexpr uint64_t edexTypeError = 1;
	uint64_t edexInterpreterError = 0;

	class Object
	{
	public:

		virtual inline int64_t castToInt() const = 0;
	};

	class EdExInt : public Object
	{
	public:
		int64_t value;

		EdExInt(int64_t val) : value(val)
		{}

		EdExInt(const std::string &val)
		{
			std::istringstream stream(val);
			if (!(stream >> value))
			{
				value = 0;
				edexInterpreterError = edexTypeError;
			}
		}

		int64_t castToInt() const override
		{
			return value;
		}
	};

	class Interpreter
	{
	public:

		std::vector<std::string> program;
		std::map<std::string, std::shared_ptr<Object>> heap;

		Interpreter() = default;

		Interpreter(const std::vector<std::string> &toRun) : program(toRun)
		{}

		inline void raiseError(const std::string &error)
		{
			std::cerr << "Error: " << error << "\n";
			exit(1);
		}

		inline std::shared_ptr<Object> evaluateExpression(const std::string &line) const
		{
			return std::make_shared<EdExInt>(line);
		}

		inline void initializeVariable(const std::string &line)
		{
			uint64_t toIndex;
			if ((toIndex = line.find_last_of("TO")) == std::string::npos)
				raiseError("Invalid syntax. Could not find token \"TO\", which is required");

			auto varName = substring(line, line.find_first_of("SET") + 4, line.length() - line.find_last_of("TO") - 3, false);
			auto expression = substring(line, line.find_last_of("TO") + 2, line.length(), false);

			// TODO: Check variable name is valid
			auto found = findInMap(heap, varName);

			if (found.first) // Variable already exists
				found.second = evaluateExpression(expression);
			else
				heap[varName] = evaluateExpression(expression);
		}

		inline void run()
		{
			// Run the interpreter on the program
			for (const auto &line : program)
			{
				if (substring(line, 0, 3, true) == "SET")
				{
					initializeVariable(line);
				}
			}
		}
	};
}
