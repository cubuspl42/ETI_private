#include "VariousAnimals.h"
#include <cstdlib>
#include "World.h"

void Turtle::performAction(World &world)
{
    if(rand() % 4 == 0) {
        _performingAction = true;
        Animal::performAction(world);
        _performingAction = false;
    }
}

bool Turtle::onCollision(World &world, Organism &other)
{
    if(!_performingAction && other.power() < 5) {
        return true;
    } else {
        return Animal::onCollision(world, other);
    }
}

void Antilope::performAction(World &world)
{
    for(int dy : {-1, 0, 1}) {
        for(int dx : {-1, 0, 1}) if((!dy || !dx) && (dy || dx)) {
            int my = y() + 2 * dy, mx = x() + 2 * dx;
            if(my >= 0 && my < world.worldHeight() &&
               mx >= 0 && mx < world.worldWidth() && rand() % 4) {
                world.moveOrganism(y(), x(), my, mx);
            }
        }
    }
}

void Fox::performAction(World &world)
{
    for(int dy : {-1, 0, 1}) {
        for(int dx : {-1, 0, 1}) if((!dy || !dx) && (dy || dx)) {
            int my = y() + dy, mx = x() + dx;
            if(my >= 0 && my < world.worldHeight() &&
               mx >= 0 && mx < world.worldWidth() && rand() % 4) {
                if(world.organismAt(my, mx) && world.organismAt(my, mx)->power() > power())
                    continue;
                world.moveOrganism(y(), x(), my, mx);
            }
        }
    }
}

bool Antilope::onCollision(World &world, Organism &other)
{
    if(rand() % 2 == 0) {
        for(int dy : {-1, 0, 1}) {
            for(int dx : {-1, 0, 1}) if((!dy || !dx) && (dy || dx)) {
                int my = y() + dy, mx = x() + dx;
                if(my >= 0 && my < world.worldHeight() &&
                   mx >= 0 && mx < world.worldWidth() &&
                   !world.organismAt(my, mx)) {
                    world.moveOrganism(y(), x(), my, mx);
                }
            }
        }
    }
    return Animal::onCollision(world, other);
}