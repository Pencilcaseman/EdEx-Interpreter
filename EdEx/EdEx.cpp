#include "edex.h"

edex::IDE ide;

int main()
{
    std::cout << "EdEx IDE and Interpreter\n";

    ide = edex::IDE();
    ide.Construct(800, 600, 1, 1);

    // Set up any IDE specific things
    ide.windows[0].setBackground({255, 255, 255});
    ide.windows[0].setTextColor({0, 0, 0});

    ide.windows[0].lines.emplace_back("Hello, World!");

    ide.windows[0].setTextScale(2);

    ide.Start();

    return 0;
}
