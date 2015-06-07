#include "Plant.h"
#include <cstdlib>
#include "World.h"

void Plant::performAction(World &world)
{
    trySpread(world);
}

void Plant::trySpread(World &world)
{
    if(rand() % 256 == 0) {
        spawnChild(world);
    }
}

void Plant::onKill(World &world, Organism &killer)
{
    world.addEvent(killer.repr() + " eats " + repr());
}