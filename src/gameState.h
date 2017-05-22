#ifndef gameState_h
#define gameState_h

#include <vector>
#include <utility>
#include <functional>

struct gameState{
    //private:
        std::vector<bool> barrels;
        std::vector<bool> targets;
        std::vector<bool> walls;
        int width;
        //Position of the player
        std::pair<int,int> position;
        std::vector<int> lastmove;
    //public:
        gameState();
        gameState(std::vector<std::vector<int> > a,std::pair<int,int> p, int w);
        bool down();
        bool up();
        bool right();
        bool left();
        bool isSolved();
        std::vector<std::vector<int> > getArena();
        void print();
        int getheur();
        std::vector<int> getlastmove();
        bool isequal(const gameState s);
        unsigned int getplayerhash(int k);
        unsigned int getbarrelhash(int k);
    //private:
        bool updateArena(std::pair<int,int> move);
};

#endif
