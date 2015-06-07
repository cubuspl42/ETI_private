#ifndef __VirtualWorld__Animal__
#define __VirtualWorld__Animal__

#include "Organism.h"

class Animal : public Organism
{
public:
    inline Animal(Organism::Type type, unsigned power, unsigned initiative)
        : Organism(type, power, initiative) {}
    virtual void performAction(World &world) override;
    virtual bool onCollision(World &world, Organism &other) override;
    virtual void onKill(World &world, Organism &killer) override;
};

#endif /* defined(__VirtualWorld__Animal__) */
