#include <cassert>
#include "Organism.h"
#include "Human.h"
#include "VariousAnimals.h"
#include "VariousPlants.h"
#include "World.h"

std::unique_ptr<Organism> Organism::create(Organism::Type type)
{
    switch (type) {
        case HUMAN:
            return std::make_unique<Human>();
            break;
        case WOLF:
            return std::make_unique<Wolf>();
            break;
        case SHEEP:
            return std::make_unique<Sheep>();
            break;
        case FOX:
            return std::make_unique<Fox>();
            break;
        case TURTLE:
            return std::make_unique<Turtle>();
            break;
        case ANTILOPE:
            return std::make_unique<Antilope>();
            break;
        case GRASS:
            return std::make_unique<Grass>();
            break;
        case DANDELION:
            return std::make_unique<Dandelion>();
            break;
        case GUARANA:
            return std::make_unique<Guarana>();
            break;
        case WOLF_BERRIES:
            return std::make_unique<WolfBerries>();
            break;
        default:
            assert(false);
            return std::unique_ptr<Organism> {};
            break;
    }
}

bool Organism::spawnChild(World &world)
{
    for(int dy : {-1, 0, 1}) {
        for(int dx : {-1, 0, 1}) if((!dy || !dx) && (dy || dx)) {
            int my = y() + dy, mx = x() + dx;
            if(my >= 0 && my < world.worldHeight() &&
               mx >= 0 && mx < world.worldWidth() &&
               !world.organismAt(my, mx)) {
                world.spawnOrganism(type(), my, mx);
                world.addEvent(repr() + " multiplies");
                return true;
            }
        }
    }
    return false;
}