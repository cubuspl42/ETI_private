#include "Human.h"
#include "World.h"

void Human::performAction(World &world)
{
    --_bonus_left;
    int ny = y() + _dy, nx = x() + _dx;
    if((_dy || _dx) && ny >= 0 && ny < world.worldHeight() && nx >= 0 && nx < world.worldWidth()) {
        world.moveOrganism(y(), x(), y()+_dy, x()+_dx);
    }
}

void Human::activateBonus(World &world)
{
    if(_bonus_left < -5) {
        world.addEvent("human activated the godmode");
        _bonus_left = 5;
    }
}

bool Human::onCollision(World &world, Organism &other)
{
    if(other.power() > power() && _bonus_left > 0) {
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
        return true;
    } else return false;
}
