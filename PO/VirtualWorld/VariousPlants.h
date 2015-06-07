#ifndef __VirtualWorld__VariousPlants__
#define __VirtualWorld__VariousPlants__

#include "Plant.h"

class Grass : public Plant
{
public:
    inline Grass() : Plant(Organism::GRASS, 0) {}
    inline char characterRepresentation() override { return 'G'; };
    inline virtual std::string repr() { return "grass"; }
};

class Dandelion : public Plant
{
public:
    inline Dandelion() : Plant(Organism::DANDELION, 0) {}
    void performAction(World &world) override;
    inline char characterRepresentation() override { return 'D'; };
    inline virtual std::string repr() { return "dandelion"; }
};

class Guarana : public Plant
{
public:
    inline Guarana() : Plant(Organism::GUARANA, 0) {}
    bool onCollision(World &world, Organism &other) override;
    inline char characterRepresentation() override { return 'U'; };
    inline virtual std::string repr() { return "guarana"; }
};

class WolfBerries : public Plant
{
public:
    inline WolfBerries() : Plant(Organism::WOLF_BERRIES, 99) {}
    inline char characterRepresentation() override { return 'B'; };
    inline virtual std::string repr() { return "wolf berries"; }
};

#endif /* defined(__VirtualWorld__VariousPlants__) */
