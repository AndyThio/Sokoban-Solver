#ifndef gameState_h
#define gameState_h

#include <vector>
#include <pair>

class gameState{
    private:
        std::vector<vector<int > > arena;
        //Position of the player
        std::pair<int,int> position;
    public:
        gameState(vector<vector<int> > a,pair<int,int> p);
        bool down();
        bool up();
        bool right();
        bool left();
        bool isSolved();
        std::vector<vector<int> > getArena();
    private:
        bool updateArena(pair<int,int> move);
        void updateplayer();
        void updatespot(pair<int,int> move, int new_space);
};

#endif
