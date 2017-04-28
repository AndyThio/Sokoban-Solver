#include <iostream>
#include <unordered_map>
#include <map>
#include <functional>
#include <fstream>
#include <pthread.h>
#include <vector>
#include <queue>
#include <string>
#include <set>
#include <chrono>
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

struct histNode{
    vector<bool> barrels;
    pair<int, int> position;

    histNode(gameState g){
        barrels = g.barrels;
        position = g.position;
    }
    bool operator<(const histNode &rhs)const{
        if(barrels == rhs.barrels){
            return position < rhs.position;
        }
        return barrels < rhs.barrels;
    }
    bool operator==(const histNode &rhs)const{
        return barrels == rhs.barrels && position == rhs.position;
    }
};

bool cmpLambda(const gameState &lhs, const gameState &rhs) {
    if(lhs.barrels != rhs.barrels){
        return lhs.barrels > rhs.barrels;
    }else{
        return lhs.position > rhs.position;
    }
};

typedef set<histNode> hist_cont;


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
bool isrepeat(hist_cont &h, gameState c){
    return h.find(histNode(c)) != h.end();
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
    hist_cont alreadyseen;
    queueNode temp = pq.top();

    while(!pq.empty()){
        //cout << "iter start" << endl;
        //cout << "size of already: " << alreadyseen.size()<< endl;


        temp = pq.top();
        /*
        cout << "Current Depth: " << temp.dept << endl;
        cout << "Current Heur: " << temp.cost -temp.dept << endl;
        cout << "backtrace printout" << endl;
        printBt(temp.arena.getlastmove());
        cout << "End back Trace" << endl;
        temp.arena.print();
        */
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
            alreadyseen.insert(histNode(tempr));
            pq.push(queueNode(tempr,temp.dept+1,temp.dept+1+tempr.getheur()));
        }
        //cout << "Moving left" << endl;
        if(templ.left() && !isrepeat(alreadyseen, templ)){
            if(templ.isSolved()){
                return templ.getlastmove();
            }
            alreadyseen.insert(histNode(templ));
            pq.push(queueNode(templ,temp.dept+1,temp.dept+1+templ.getheur()));
        }
        //cout << "Moving down" << endl;
        if(tempd.down() && !isrepeat(alreadyseen, tempd)){
            if(tempd.isSolved()){
                return tempd.getlastmove();
            }
            alreadyseen.insert(histNode(tempd));
            pq.push(queueNode(tempd,temp.dept+1,temp.dept+1+tempd.getheur()));
        }
        //cout << "Moving up" << endl;
        if(tempu.up() && !isrepeat(alreadyseen, tempu)){
            if(tempu.isSolved()){
                return tempu.getlastmove();
            }
            alreadyseen.insert(histNode(tempu));
            pq.push(queueNode(tempu,temp.dept+1,temp.dept+1+tempu.getheur()));
        }
    }
    //TODO: generate some kind of backtrace


}






int main(){
    gameState arena;
    arena = formArena();
    
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_time;
    
    start= chrono::system_clock::now();
    
    auto result = findSolution(arena);
    cout << "priting results" << endl;
    printBt(result);
    
    end = chrono::system_clock::now();
    elapsed_time = end-start;
    cout << endl << "Time elapsed: " << elapsed_time.count() << endl << endl;
}
