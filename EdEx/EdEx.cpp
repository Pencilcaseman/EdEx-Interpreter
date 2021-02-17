#include "edex.h"

// edex::IDE ide;

struct testObj
{
	uint64_t type;
	void *val;
};

int32_t main()
{
	std::cout << "EdEx IDE and Interpreter\n";

	std::cout << std::fixed;
	std::cout.precision(10);

	edex::Interpreter testInterpreter({"SET value TO 5 + 5 - 3 * 10 / 123",
									   "SEND value TO DISPLAY"});

	auto start = edex::seconds();
	auto compilerOutput = testInterpreter.compile();
	auto end = edex::seconds();

	if (compilerOutput.isError)
	{
		std::cerr << "Compiler Error: " << compilerOutput.infoString << "\n";
		exit(1);
	}

	std::cout << "Time: " << end - start << " seconds\n";

	for (const auto &line : testInterpreter.compiled)
	{
		std::cout << line.name << " | ";
		for (const auto &parsed : line.vars)
			std::cout << parsed << " | ";
		for (const auto &parsed : line.info)
			std::cout << parsed << " | ";
		for (const auto &parsed : line.solver.processed)
		{
			if (parsed.second.length() > 0)
				std::cout << parsed.second << " ";
			else
				std::cout << parsed.first << " ";
		}
		std::cout << " | " << line.typeInfo << "\n";
		std::cout << "\n";
	}

	start = edex::seconds();
	auto interpreterOutput = testInterpreter.interpret();
	end = edex::seconds();

	if (interpreterOutput.isError)
	{
		std::cerr << "Interpreter Error: " << interpreterOutput.infoString << "\n"; exit(1);
		exit(1);
	}

	std::cout << "\n\n";

	for (const auto &out : testInterpreter.output)
	{
		std::cout << ">>> " << out.line << "\n";
	}

	// auto ide = edex::IDE();
	// ide.Construct(1200, 800, 1, 1);
	// ide.Start();

	return 0;
}
