#include <iostream>
#include <fstream>
#include <pthread.h>
#include <vector>
#include <queue>
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

const int rht = 7;
const int lft = 8;
const int down = 9;
const int up = 10;

struct queueNode{
    gameState parent;
    gameState arena;
    int dept;
    int cost;

    queueNode(){}
    queueNode(gameState l, gameState g, int d, int c)
        :parent(l),arena(g), dept(d),cost(c)
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


//TODO: need to generate the vector int
vector<int> findSolution(gameState s){
    auto mycmp = [](const queueNode& rhs, const queueNode& lhs)
        {return rhs.cost > lhs.cost;};
    priority_queue<queueNode , vector<queueNode> ,decltype(mycmp)> pq(mycmp);
    pq.push(queueNode(s,s, 0, s.getheur()));
    bool isfinished = false;
    gameState finishedState;

    while(!isfinished){
        auto temp = pq.top();
        pq.pop();

        gameState tempr = temp.arena;
        gameState templ = tempr;
        gameState tempu = tempr;
        gameState tempd = tempr;

        cout << "current depth" << temp.dept << endl;
        temp.arena.print();
        cout << endl;

        //cout << "Moving right" << endl;
        if(tempr.right()){
            if(tempr.isSolved() && !tempr.isequal(temp.parent)){
                return tempr.getlastmove();
            }
            pq.push(queueNode(temp.arena,tempr,temp.dept+1,temp.dept+1+tempr.getheur()));
        }
        //cout << "Moving left" << endl;
        if(templ.left()){
            if(templ.isSolved() && !tempr.isequal(temp.parent)){
                return templ.getlastmove();
            }
            pq.push(queueNode(temp.arena,templ,temp.dept+1,temp.dept+1+templ.getheur()));
        }
        //cout << "Moving down" << endl;
        if(tempd.down()){
            if(tempd.isSolved() && !tempr.isequal(temp.parent)){
                return tempd.getlastmove();
            }
            pq.push(queueNode(temp.arena,tempd,temp.dept+1,temp.dept+1+tempd.getheur()));
        }
        //cout << "Moving up" << endl;
        if(tempu.up()){
            if(tempu.isSolved() && !tempr.isequal(temp.parent)){
                return tempu.getlastmove();
            }
            pq.push(queueNode(temp.arena,tempu,temp.dept+1,temp.dept+1+tempu.getheur()));
        }
    }
    //TODO: generate some kind of backtrace


}



void printBt(vector<int> bt){
    ofstream rfil;
    rfil.open("../bin/results.txt");
    for(auto &e: bt){
        switch(e){
            case rht: rfil << "Right" << endl;
                break;
            case lft: rfil << "Left" << endl;
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
    auto result = findSolution(arena);
    printBt(result);
}
