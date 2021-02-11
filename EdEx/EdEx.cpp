#include "edex.h"

// edex::IDE ide;

int32_t main()
{
	std::cout << "EdEx IDE and Interpreter\n";

	auto str = edex::replaceString("[[[", "[", "[");
	for (const auto &token : edex::splitString("[[[ ]]]", {' ', '[', ']'}))
		std::cout << "Token: " << token << "\n";
	
	auto ide = edex::IDE();
	ide.Construct(800, 600, 1, 1);
	ide.Start();

	return 0;
}
