#include <iostream>
#include <unordered_map>
#include <map>
#include <functional>
#include <fstream>
#include <pthread.h>
#include <thread>
#include <vector>
#include <queue>
#include <string>
#include <set>
#include <mutex>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
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

int max_threads = thread::hardware_concurrency();
atomic_bool isfinished(false);
vector<int> finalbt;

pthread_rwlock_t asLock;
mutex pqMut;

condition_variable cpq;
mutex cpqMut;

condition_variable isdone;
mutex isdoneMut;

mutex btMut;

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

struct queueNode{
    gameState arena;
    int dept;
    int cost;

    queueNode(){}
    queueNode(gameState g, int d, int c)
        :arena(g), dept(d),cost(c)
    {}

    bool operator>(const queueNode &rhs)const{
        return cost > rhs.cost;
    }
};


typedef priority_queue<queueNode , vector<queueNode> ,greater<queueNode> > pqType;
typedef set<histNode> hist_cont;


gameState fetchArena(string fname){
    vector<vector<int> > arena;
    int player_exist = -1;
    pair<int,int> player;

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

bool isrepeat(const hist_cont &h, gameState c){
    auto temp = histNode(c);
    pthread_rwlock_rdlock(&asLock);
    auto ret1 = h.find(temp);
    auto ret2 = h.end();
    pthread_rwlock_unlock(&asLock);
    return ret1 != ret2;
    
}

void expandDir(gameState g, int parent_depth, hist_cont &alreadyseen, pqType &pq){
        if(!isrepeat(alreadyseen, g)){
            pthread_rwlock_wrlock(&asLock);
            alreadyseen.insert(histNode(g));
            pthread_rwlock_unlock(&asLock);
   
            if(g.isSolved()){
                if(!isfinished){
                    isfinished = true;
                    btMut.lock();
                    printBt(g.getlastmove());
                    btMut.unlock();
                    cpq.notify_all();
                }
                return;
            }
            int heur = parent_depth+1+g.getheur();
            pqMut.lock();
            pq.push(queueNode(g,parent_depth+1,heur));
            pqMut.unlock();
            
            cpq.notify_one();
        }
}

void expandNode(hist_cont &alreadyseen, pqType &pq){
    bool pqempty;
    queueNode temp;
    while(!isfinished){
        
        unique_lock<mutex> lk(pqMut);
        if(!pq.empty()){
            temp = pq.top();
            pq.pop();
        }
        else{
            while(pq.empty()){
                cpq.wait(lk);
                
                if(isfinished){
                    return;
                }
            }
            temp = pq.top();
            pq.pop();
        }
        lk.unlock();
        
        
        gameState tempr = temp.arena;
        gameState templ = tempr;
        gameState tempu = tempr;
        gameState tempd = tempr;
    
        if(tempr.right()){
            expandDir(tempr,temp.dept, alreadyseen,pq);
        }
        if(templ.left()){
            expandDir(templ,temp.dept, alreadyseen,pq);
        }
        if(tempd.down()){
            expandDir(tempd,temp.dept, alreadyseen,pq);
        }
        if(tempu.up()){
            expandDir(tempu,temp.dept, alreadyseen,pq);
        }
    }
}

void findSolution(gameState s){
    pqType pq;
    pq.push(queueNode(s, 0, s.getheur()));
    vector<thread> thd;
    gameState finishedState;
    hist_cont alreadyseen;
    
    while(thd.size() < max_threads){
        thd.push_back(thread(expandNode,ref(alreadyseen),ref(pq)));
    }
    
    for(auto &e: thd){
        e.join();
    }
}

int main(int argc, char* argv[]){
    gameState arena;
    if(argc > 1){
        arena = fetchArena(argv[1]);
    }
    else{
        arena = formArena();
    }
    if(argc > 2){
        max_threads = atoi(argv[2]);
    }
    
    
    pthread_rwlock_init(&asLock, NULL);
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_time;
    

    start= chrono::system_clock::now();
    
    findSolution(arena);

    end = chrono::system_clock::now();
    elapsed_time = end-start;
    cout << "Time elapsed: " << elapsed_time.count() << endl;
}
