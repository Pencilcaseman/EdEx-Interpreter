#include "edex.h"

// edex::IDE ide;

int32_t main()
{
	std::cout << "EdEx IDE and Interpreter\n";

	auto ide = edex::IDE();
	ide.Construct(800, 600, 1, 1);
	ide.Start();

	return 0;
}
