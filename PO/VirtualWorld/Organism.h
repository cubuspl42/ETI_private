#ifndef __VirtualWorld__Organism__
#define __VirtualWorld__Organism__

#include <memory>
#include <string>

class World;

class Organism
{
public:
    enum Type {
        NONE,
        HUMAN,
        WOLF,
        SHEEP,
        FOX,
        TURTLE,
        ANTILOPE,
        GRASS,
        DANDELION,
        GUARANA,
        WOLF_BERRIES
    };
    inline Organism(Type type, unsigned power, unsigned initiative)
        : _type(type), _power(power), _initiative(initiative), _age(0) {}
    inline virtual void performAction(World &world) { ++_age; }
    virtual bool onCollision(World &world, Organism &other) = 0; // returns true if collision is prevented
    inline virtual char characterRepresentation() { return 'X'; };
    inline virtual std::string repr() { return "organizm"; }
    virtual void onKill(World &world, Organism &killer) = 0;
    inline Type type() { return _type; }
    inline int y() { return _y; }
    inline int x() { return _x; }
    inline void setPosition(int y, int x) { _y = y, _x = x; }
    inline unsigned power() { return _power; }
    inline void increasePower(unsigned delta) { _power += delta; }
    inline unsigned initiative() { return _initiative; }
    inline unsigned age() { return _age; }
    static std::unique_ptr<Organism> create(Type type);
    bool spawnChild(World &world);
private:
    Type _type;
    int _y, _x;
    unsigned _power;
    unsigned _initiative;
    unsigned _age;
};

#endif /* defined(__VirtualWorld__Organism__) */
