#include "edex.h"

int32_t main()
{
	std::cout << "EdEx IDE and Interpreter\n";

	std::cout << std::fixed;
	std::cout.precision(10);

// 	edex::Interpreter testInterpreter({"SET x TO \"HELLO\"",
// 									   "SEND x TO DISPLAY",
// 									   "SEND x & \" THERE\" TO DISPLAY"});

	edex::Interpreter testInterpreter({"SET x TO 10",
									   "SET y TO x",
									   "SEND x TO DISPLAY",
									   "SEND y TO DISPLAY"});

	auto start = edex::seconds();
	auto compilerOutput = testInterpreter.compile();
	auto end = edex::seconds();

	if (compilerOutput.isError)
	{
		std::cerr << "Compiler Error: " << compilerOutput.infoString << "\n";
		exit(1);
	}

	std::cout << "Time: " << (end - start) * 1000 << " ms\n";

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

	std::cout << "Time: " << (end - start) * 1000 << " ms\n";

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
