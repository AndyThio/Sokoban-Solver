#include <iostream>
#include <fstream>
#include <pthread.h>
#include <vector>
#include <string>
using namespace std;

#include "gameState.h"

/*
 * Types of Spaces:
 *     empty  - place with no wall/barrel
 *     barrel - pushable object
 *     hole   - an empty barrel target
 *     f_hole - a target with a barrel on it
 *     wall   - a wall
 *     player - the player/pusher
 */
const int empty = 0;
const int barrel = 1;
const int hole = 2;
const int f_hole = 3;
const int wall = 4;
const int player = 5;
const int playerhole = 6;

const int right = 7;
const int left = 8;
const int down = 9;
const int up = 10;

struct queueNode{
    queueNode *parent;
    gameState arena;
    int dept;
    int cost;

    queueNode(){}
    queueNode(queueNode &n, gameState g, int d, int c)
        :parent(n), arena(g), dept(d),cost(c)
    {}
};

gameState formArena(){
    vector<vector<int> > arena;
    int player_exist = -1;
    pair<int,int> player;

    string fname;
    cout << "Enter text file name: ";
    cin >> fname;

    /*
     * Text File Requirements:
     * - All integers
     * - First number should be height
     * - Second number should be width
     * - numbers should be separated by whitespace
     * - Exactly one player must exist
     * - Do NOT include the border walls outlining your puzzle
     * - See "Types of Spaces" for what numbers to put for different spaces
     */

    fstream fin(fname, fstream::in);
    if(!fin.is_open()){
        cerr << "Invalid Text File" << endl;
        exit(1);
    }
    int height, width,spot;
    fin >> height >> width;
    arena.resize(height);

    for (int i = 0; i < height ; ++i){
        for (int j = 0; j < width; ++j){
            fin >> spot;
            if(spot == 5 || spot == 6){
                ++player_exist;
                player = make_pair(i,j);
                cout << "Player Coordinates: " << player.first <<", " << player.second << endl;
            }
            arena.at(i).push_back(spot);
        }
    }
    if(!player_exist){
        return gameState(arena,player);
    }
    else{
        cerr << "Player doesn't exist or more than one player";
        return gameState();
    }
}

vector<int> createbacktrace(const vector<queueNode>& history, gameState s,const queueNode &temp){
    vector<int> trace;
    trace.push_back(s.getlastmove());
    trace.push_back((*temp).arena.getlastmove());
    auto temp2=(*temp).parent;
    while(temp2->arena.getlastmove()!=0){
        trace.push_back(temp2->arena.getlastmove());
        temp2 = temp2->parent;
    }
    return trace;



}

//TODO: need to generate the vector int
vector<int> findSolution(gameState s){
    vector<queueNode> history;
    priority_queue<queueNode> pq([](const queueNode& rhs, const queueNode& lhs)
        {return rhs.cost > lhs.cost} , vector<queueNode>);
    pq.push(queueNode(nullptr, s, 0, s.getheur()));
    bool isfinished = false;
    gameState finishedState;

    while(!isfinished){

        history.push_back(pq.pop());

        auto temp = &(history.at(history.size()-1));

        gameState tempr = (*temp).arena;
        gameState templ = tempr;
        gameState tempu = tempr;
        gameState tempd = tempr;

        if(tempr.right()){
            if(tempr.isSolved()){
                return createbacktrace(history,tempr, temp);
            }
            pq.push(queueNode(temp,tempr,temp.dept+1,tempr.getheur()));
        }
        if(templ.left()){
            if(templ.isSolved()){
                return createbacktrace(history,templ, temp);
            }
            pq.push(queueNode(temp,templ,temp.dept+1,templ.getheur()));
        }
        if(tempd.down()){
            if(tempd.isSolved()){
                return createbacktrace(history,tempd, temp);
            }
            pq.push(queueNode(temp,tempd,temp.dept+1,tempd.getheur()));
        }
        if(tempu.up()){
            if(tempu.isSolved()){
                return createbacktrace(history,tempu, temp);
            }
            pq.push(queueNode(temp,tempu,temp.dept+1,tempu.getheur()));
        }
    }
    //TODO: generate some kind of backtrace
    return createbacktrace(history,finishedState,


}



void printBt(vector<int> bt){
    ofstream rfil;
    rfil.open("../bin/results.txt");
    for(auto &e: bt){
        switch(e){
            case right: rfil << "Right" << endl;
                break;
            case left: rfil << "Left" << endl;
                break;
            case up: rfil << "Up" << endl;
                break;
            case down: rfil << "Down" << endl;
                break;
            default: cerr << "Invalid move: " + e << endl;
                return;
                break;
        }
    }
    rfil.close();
}


int main(){
    gameState arena;
    arena = formArena();

    arena.print();
    cout << arena.getheur() << endl;
}
