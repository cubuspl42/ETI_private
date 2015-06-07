#include "Animal.h"
#include "World.h"

void Animal::performAction(World &world)
{
    for(int dy : {-1, 0, 1}) {
        for(int dx : {-1, 0, 1}) if((!dy || !dx) && (dy || dx)) {
            int my = y() + dy, mx = x() + dx;
            if(my >= 0 && my < world.worldHeight() &&
               mx >= 0 && mx < world.worldWidth() && rand()%4 == 0) {
                world.moveOrganism(y(), x(), my, mx);
            }
        }
    }
}

bool Animal::onCollision(World &world, Organism &other)
{
    if(type() == other.type()) {
        if(!spawnChild(world)) {
            static_cast<Animal&>(other).spawnChild(world);
        }
        return true;
    }
    return false;
}

void Animal::onKill(World &world, Organism &killer)
{
    world.addEvent(killer.repr() + " kills " + repr());
}