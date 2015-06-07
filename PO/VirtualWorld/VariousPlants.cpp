#include "VariousPlants.h"

void Dandelion::performAction(World &world)
{
    trySpread(world);
    trySpread(world);
    trySpread(world);
}

bool Guarana::onCollision(World &world, Organism &other)
{
    other.increasePower(3);
    return false;
}