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

	edex::Interpreter testInterpreter({"SET x TO 10",
									  "    SET y TO 20"});

	auto start = edex::seconds();
	testInterpreter.run();
	auto end = edex::seconds();
	std::cout << "Time: " << end - start << " seconds\n";
	
	for (const auto &val : testInterpreter.heap)
	{
		std::cout << val.first << ", " << val.second->castToInt() << "\n";
	}

	// auto ide = edex::IDE();
	// ide.Construct(1200, 800, 1, 1);
	// ide.Start();

	return 0;
}
