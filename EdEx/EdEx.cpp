#include "edex.h"

edex::IDE ide;

int main()
{
	std::cout << "EdEx IDE and Interpreter\n";

	ide = edex::IDE();
	ide.Construct(800, 600, 1, 1);
	ide.Start();

	return 0;
}
