#include "World.h"
#include "VariousAnimals.h"

#include <cassert>
#include <clocale>
#include <iostream>
#include <fstream>
#include "Human.h"

constexpr unsigned WORLD_SIZE = 20;

World::World()
{
    setlocale(LC_CTYPE, "");
    _win = initscr();
    
    if(!_win) {
        std::cerr << "Failed to initialize ncurses" << std::endl;
    }
    
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors();
    
    _world_width = WORLD_SIZE;
    _world_height = WORLD_SIZE;
    
    spawnOrganisms();
}

World::~World()
{
    delwin(_win);
    endwin();
    refresh();
}

Organism *World::organismAt(int y, int x)
{
    auto it = find_if(_organisms.begin(), _organisms.end(), [=](auto& o) {
        return o && o->y() == y && o->x() == x;
    });
    if(it == _organisms.end())
        return nullptr;
    else return it->get();

}

void World::moveOrganism(int y_from, int x_from, int y_to, int x_to)
{
    Organism *attacker = organismAt(y_from, x_from);
    Organism *defender = organismAt(y_to, x_to);
    
    if(attacker && defender) {
        bool preventCollision = defender->onCollision(*this, *attacker);
        if(preventCollision) {
            return;
        }
    }
    
    attacker = organismAt(y_from, x_from); // attacker could be already killed
    defender = organismAt(y_to, x_to); // attacked organism could run away
    
    if(attacker && defender) {
        bool preventCollision = attacker->onCollision(*this, *defender);
        if(preventCollision) {
            return;
        }
    }
    
    attacker = organismAt(y_from, x_from);
    defender = organismAt(y_to, x_to);
    
    if(attacker && defender) {
        if(attacker->power() >= defender->power()) {
            killOrganism(defender, *attacker);
            attacker->setPosition(y_to, x_to);
        } else {
            killOrganism(attacker, *defender);
        }
    } else if(attacker)  {
        attacker->setPosition(y_to, x_to);
    }
}

void World::killOrganism(Organism *organism, Organism &killer)
{
    auto it = find_if(_organisms.begin(), _organisms.end(), [=](auto& o) {
        return &*o == organism;
    });
    it->get()->onKill(*this, killer);
    it->reset();
}

void World::mainLoop()
{
    time_t start;
    time(&start);
    
    int key = 0;
    do {
        Human *_human = getHuman();
        if(_human)
            _human->setInput(0, 0);
        switch(key) {
            case KEY_UP:
                if(_human)
                    _human->setInput(-1, 0);
                break;
            case KEY_DOWN:
                if(_human)
                    _human->setInput(1, 0);
                break;
            case KEY_LEFT:
                if(_human)
                    _human->setInput(0, -1);
                break;
            case KEY_RIGHT:
                if(_human)
                    _human->setInput(0, 1);
                break;
            case '/':
                if(_human) {
                    _human->activateBonus(*this);
                }
                break;
            default:
                break;
        }
        performActions();
        clear();
        mvprintw(0, 0, "Jakub Trzebiatowski, 155215");
        drawOrganisms();
        printEvents();
    } while((key = getch()) != 'q');
}

void World::spawnOrganism(Organism::Type type, int y, int x)
{
    _organisms.push_back(Organism::create((Organism::Type)type));
    _organisms.back()->setPosition(y, x);
}

void World::addEvent(const std::string &event)
{
    _events.push_back(event);
}

void World::spawnOrganismRandomly(Organism::Type type, std::default_random_engine &engine, std::uniform_int_distribution<unsigned> &dist)
{
    unsigned y, x;
    do {
        y = dist(engine);
        x = dist(engine);
    } while(organismAt(y, x));
    spawnOrganism(type, y, x);
}

void World::spawnOrganisms()
{
    std::default_random_engine engine(time(nullptr));
    std::uniform_int_distribution<unsigned> dist(0, WORLD_SIZE-1);
    
    spawnOrganismRandomly(Organism::HUMAN, engine, dist);
    for(unsigned type = Organism::WOLF; type <= Organism::WOLF_BERRIES; ++type) {
        unsigned n = 2 + rand() % 2;
        for(unsigned i = 0; i < n; ++i) {
            spawnOrganismRandomly((Organism::Type)type, engine, dist);
        }
    }
}

void World::performActions()
{
    std::sort(_organisms.begin(), _organisms.end(), [](const auto &a, const auto &b) {
        return a->initiative() > b->initiative();
    });
    for(unsigned i = 0; i < _organisms.size(); ++i) {
        if(_organisms[i]) {
            _organisms[i]->performAction(*this);
        }
    }
    for(auto it = _organisms.begin(); it != _organisms.end();) {
        if(*it) {
            ++it;
        } else {
            it = _organisms.erase(it);
        }
    }
}

void World::drawOrganisms()
{
    for(auto &o : _organisms) {
        mvaddch(o->y() + 2, o->x(), o->characterRepresentation());
    }
}

void World::printEvents()
{
    int y = WORLD_SIZE + 3;
    for(auto &e : _events) {
        mvprintw(y++, 0, e.c_str());
    }
    _events.clear();
}

Human *World::getHuman()
{
    auto it = find_if(_organisms.begin(), _organisms.end(), [=](auto& o) {
        return o && o->type() == Organism::HUMAN;
    });
    if(it == _organisms.end())
        return nullptr;
    else return static_cast<Human*>(it->get());
}

void World::dump(const std::string &filename)
{
    std::ofstream of(filename);
}

void World::read(const std::string &filename)
{
    
}
