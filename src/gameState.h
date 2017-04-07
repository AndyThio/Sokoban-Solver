#ifndef gameState_h
#define gameState_h

#include <vector>
#include <pair>

class gameState{
    private:
        std::vector<vector<int > > arena;
        //Position of the player
        std::pair<int,int> position;
        //Refers to number of Barrels/Targets
        int barrels;
        int targets;
    public:
        gameState(vector<vector<int> > a,pair<int,int> p, int b, int t);
        bool down();
        bool up();
        bool right();
        bool left();
        bool isSolved();
        std::vector<vector<int> > getArena();
    private:
        bool isValid( pair<int,int> move);
        vector<vector<int> > updateArena(pair<int,int> move);
};

#endif
