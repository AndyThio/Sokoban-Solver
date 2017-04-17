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
    gameState arena;
    int dept;
    int cost;

    queueNode(){}
    queueNode(gameState g, int d, int c)
        :arena(g), dept(d),cost(c)
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
        return gameState(arena,player,width);
    }
    else{
        cerr << "Player doesn't exist or more than one player";
        return gameState();
    }
}

void printBt(vector<int> bt){
    /*
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
    */
    int i = 1;
    for(auto &e: bt){
        cout << i << ") ";
        switch(e){
            case rht: cout << "Right" << endl;
                break;
            case lft: cout << "Left" << endl;
                break;
            case up: cout << "Up" << endl;
                break;
            case down: cout << "Down" << endl;
                break;
            default: cerr << "Invalid move: " + e << endl;
                return;
                break;
        }
        ++i;
    }
}
bool isrepeat(vector<gameState> &h, gameState c){
    for(auto &e: h){
        if(e.isequal(c)){
            /*
            cout << "returned true" << endl;
            e.print();
            cout << "comaping to: " << endl;
            c.print();
            */
            return true;
        }
    }
            /*
    cout << "returned false" << endl;
    cout << "false comaping to: " << endl;
    c.print();
            */
    return false;
}


//TODO: need to generate the vector int
vector<int> findSolution(gameState s){
    auto mycmp = [](const queueNode& rhs, const queueNode& lhs)
        {return rhs.cost > lhs.cost;};
    priority_queue<queueNode , vector<queueNode> ,decltype(mycmp)> pq(mycmp);
    pq.push(queueNode(s, 0, s.getheur()));
    bool isfinished = false;
    gameState finishedState;
    //could be just tracking barrels + positi
    vector<gameState> alreadyseen;
    queueNode temp = pq.top();

    while(!pq.empty()){
        //cout << "iter start" << endl;
        //cout << "size of already: " << alreadyseen.size()<< endl;
        

        temp = pq.top();
        cout << "Current Depth: " << temp.dept << endl;
        cout << "Current Heur: " << temp.cost -temp.dept << endl;
        cout << "backtrace printout" << endl;
        printBt(temp.arena.getlastmove());
        cout << "End back Trace" << endl;
        temp.arena.print();
        //cout << "not top" << endl;
        pq.pop();

        //cout << "allocating stuff" << endl;
        gameState tempr = temp.arena;
        gameState templ = tempr;
        gameState tempu = tempr;
        gameState tempd = tempr;

        //cout << "current depth" << temp.dept << endl;
        //temp.arena.print();
        //cout << endl;

        //cout << "Moving right" << endl;
        if(tempr.right() && !isrepeat(alreadyseen, tempr)){
            if(tempr.isSolved()){
                return tempr.getlastmove();
            }
            alreadyseen.push_back(tempr);
            pq.push(queueNode(tempr,temp.dept+1,temp.dept+1+tempr.getheur()));
        }
        //cout << "Moving left" << endl;
        if(templ.left() && !isrepeat(alreadyseen, templ)){
            if(templ.isSolved()){
                return templ.getlastmove();
            }
            alreadyseen.push_back(templ);
            pq.push(queueNode(templ,temp.dept+1,temp.dept+1+templ.getheur()));
        }
        //cout << "Moving down" << endl;
        if(tempd.down() && !isrepeat(alreadyseen, tempd)){
            if(tempd.isSolved()){
                return tempd.getlastmove();
            }
            alreadyseen.push_back(tempd);
            pq.push(queueNode(tempd,temp.dept+1,temp.dept+1+tempd.getheur()));
        }
        //cout << "Moving up" << endl;
        if(tempu.up() && !isrepeat(alreadyseen, tempu)){
            if(tempu.isSolved()){
                return tempu.getlastmove();
            }
            alreadyseen.push_back(tempu);
            pq.push(queueNode(tempu,temp.dept+1,temp.dept+1+tempu.getheur()));
        }
    }
    //TODO: generate some kind of backtrace


}






int main(){
    gameState arena;
    arena = formArena();
    auto result = findSolution(arena);
    cout << "priting results" << endl;
    printBt(result);
}
