#ifndef __VirtualWorld__Plant__
#define __VirtualWorld__Plant__

#include "Organism.h"

class Plant : public Organism
{
public:
    inline Plant(Organism::Type type, unsigned power) : Organism(type, power, 0) {}
    virtual void performAction(World &world) override;
    inline virtual bool onCollision(World &world, Organism &other) override { return false; };
    virtual void onKill(World &world, Organism &killer) override;
    void trySpread(World &world);
};

#endif /* defined(__VirtualWorld__Plant__) */
