#include "World.h"
#include <iostream>

int main(int argc, const char **argv) {
    if(argc > 1)
        std::cin.get();
    srand(time(nullptr));
    World world;
    world.mainLoop();
    return 0;
}
