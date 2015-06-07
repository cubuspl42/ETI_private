#ifndef __VirtualWorld__World__
#define __VirtualWorld__World__

#include <cstdlib>
#include <vector>
#include <ncurses.h>
#include <random>
#include <string>

#include "Organism.h"

class Human;

class World
{
    WINDOW *_win;
    size_t _world_width, _world_height;
    std::vector<std::unique_ptr<Organism>> _organisms;
    std::vector<std::string> _events;
public:
    World();
    ~World();
    inline size_t worldWidth() { return _world_width; }
    inline size_t worldHeight() { return _world_height; }
    Organism *organismAt(int y, int x);
    void spawnOrganism(Organism::Type type, int y, int x);
    void moveOrganism(int y_from, int x_from, int y_to, int x_to);
    void killOrganism(Organism *organism, Organism &killer);
    void mainLoop();
    void addEvent(const std::string &event);
    Human *getHuman();
    void dump(const std::string &filename);
    void read(const std::string &filename);
private:
    void spawnOrganismRandomly(Organism::Type type, std::default_random_engine &engine,
                               std::uniform_int_distribution<unsigned> &dist);
    void spawnOrganisms();
    void performActions();
    void drawOrganisms();
    void printEvents();
};

#endif /* defined(__VirtualWorld__World__) */
