#ifndef __VirtualWorld__Human__
#define __VirtualWorld__Human__

#include "Animal.h"

class Human : public Animal
{
    int _dy = 0, _dx = 0;
    int _bonus_left = 0;
public:
    inline Human() : Animal(Organism::HUMAN, 5, 4) {}
    void performAction(World &world) override;
    bool onCollision(World &world, Organism &other) override;
    inline char characterRepresentation() override { return 'H'; };
    inline virtual std::string repr() { return "human"; }
    inline void setInput(int dy, int dx) { _dy = dy, _dx = dx; }
    void activateBonus(World &world);
};

#endif /* defined(__VirtualWorld__Human__) */
