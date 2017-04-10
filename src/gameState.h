#ifndef gameState_h
#define gameState_h

#include <vector>
#include <utility>

class gameState{
    private:
        std::vector<std::vector<int > > arena;
        //Position of the player
        std::pair<int,int> position;
        std::vector<int> lastmove;
    public:
        gameState();
        gameState(std::vector<std::vector<int> > a,std::pair<int,int> p);
        gameState(std::vector<std::vector<int> > a,std::pair<int,int> p, std::vector<int> l);
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
    private:
        bool updateArena(std::pair<int,int> move);
        void updateplayer();
        void updatespot(std::pair<int,int> move, int new_space);
};

#endif
