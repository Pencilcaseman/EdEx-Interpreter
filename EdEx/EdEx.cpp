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

	edex::Interpreter testInterpreter({"SET x TO 10",
									   "SET y TO 20",
									   "SET xy TO 10 + 20",
									   "SET pi TO 3.14",
									   "SET pi2 TO 5 + 3.14",
									   "SET thing TO x + y"});

	auto start = edex::seconds();
	testInterpreter.compile();
	auto end = edex::seconds();
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

	std::cout << "\n\n\n";

	start = edex::seconds();
	testInterpreter.interpret();
	end = edex::seconds();
	std::cout << "Time: " << end - start << " seconds\n";

	for (const auto &variable : testInterpreter.heap)
	{
		std::cout << "Name: " << variable.first << " | Value: " << variable.second->castToString() << "\n";
	}

	// auto ide = edex::IDE();
	// ide.Construct(1200, 800, 1, 1);
	// ide.Start();

	return 0;
}
