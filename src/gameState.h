#ifndef gameState_h
#define gameState_h

#include <vector>
#include <utility>

class gameState{
    private:
        std::vector<std::vector<int > > arena;
        //Position of the player
        std::pair<int,int> position;
    public:
        gameState();
        gameState(std::vector<std::vector<int> > a,std::pair<int,int> p);
        bool down();
        bool up();
        bool right();
        bool left();
        bool isSolved();
        std::vector<std::vector<int> > getArena();
        void print();
    private:
        bool updateArena(std::pair<int,int> move);
        void updateplayer();
        void updatespot(std::pair<int,int> move, int new_space);
};

#endif
