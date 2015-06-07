#ifndef __VirtualWorld__VariousAnimals__
#define __VirtualWorld__VariousAnimals__

#include "Animal.h"

class Wolf : public Animal
{
public:
    inline Wolf() : Animal(Organism::WOLF, 9, 5) {}
    inline char characterRepresentation() override { return 'W'; };
    inline virtual std::string repr() { return "wolf"; }
};

class Sheep : public Animal
{
public:
    inline Sheep() : Animal(Organism::SHEEP, 4, 4) {}
    inline char characterRepresentation() override { return 'S'; };
    inline virtual std::string repr() { return "sheep"; }
};

class Fox : public Animal
{
public:
    inline Fox() : Animal(Organism::FOX, 3, 7) {}
    void performAction(World &world) override;
    inline char characterRepresentation() override { return 'F'; };
    inline virtual std::string repr() { return "fox"; }
};

class Turtle : public Animal
{
    bool _performingAction = false;
public:
    inline Turtle() : Animal(Organism::TURTLE, 2, 1) {}
    void performAction(World &world) override;
    bool onCollision(World &world, Organism &other) override;
    inline char characterRepresentation() override { return 'T'; };
    inline virtual std::string repr() { return "turtle"; }
};

class Antilope : public Animal
{
public:
    inline Antilope() : Animal(Organism::ANTILOPE, 4, 4) {}
    void performAction(World &world) override;
    bool onCollision(World &world, Organism &other) override;
    inline char characterRepresentation() override { return 'A'; };
    inline virtual std::string repr() { return "antilope"; }
};

#endif /* defined(__VirtualWorld__VariousAnimals__) */
