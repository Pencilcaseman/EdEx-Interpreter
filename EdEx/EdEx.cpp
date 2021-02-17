#include "edex.h"

int32_t main()
{
	std::cout << "EdEx IDE and Interpreter\n";

	std::cout << std::fixed;
	std::cout.precision(10);

	edex::Interpreter testInterpreter({
		// "SET x TO \" HELLO\"",
		// "SET y TO \" THERE\"",
		// "SET z TO \" BOB!\"",
		// "SET xy TO x & y",
		// "SET xyz TO x & y & z",
		// "SET xy2 TO \" HELLO\" & \" THERE\"",
		// "SET xyz2 TO \" HELLO\" & \" THERE\" & \" BOB!\"",
		// "SEND x TO DISPLAY",
		// "SEND y TO DISPLAY",
		// "SEND z TO DISPLAY",
		// "SEND xy TO DISPLAY",
		// "SEND xyz TO DISPLAY",
		// "SEND xy2 TO DISPLAY",
		// "SEND xyz2 TO DISPLAY",
		// "SEND \"\n\n\n\" TO DISPLAY",
		// 

		"SET x TO 5",
		"SET y TO 10",
		"SEND x TO DISPLAY",
		"SEND y TO DISPLAY",
		"SEND x > y TO DISPLAY"
									  });

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

// 	auto ide = edex::IDE();
// 	ide.Construct(1200, 800, 1, 1);
// 	ide.Start();

	return 0;
}
