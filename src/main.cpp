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
pthread_rwlock_t pqLock;

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
                //cout << "Player Coordinates: " << player.first <<", " << player.second << endl;
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
                //cout << "Player Coordinates: " << player.first <<", " << player.second << endl;
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
    pthread_rwlock_rdlock(&asLock);
    bool ret = h.find(histNode(c)) != h.end();
    pthread_rwlock_unlock(&asLock);
    return ret;
}

void expandNode(const queueNode g, hist_cont &alreadyseen, pqType &pq){
    //cout << "allocating stuff" << endl;
    gameState tempr = g.arena;
    gameState templ = tempr;
    gameState tempu = tempr;
    gameState tempd = tempr;

    //cout << "current depth" << temp.dept << endl;
    //temp.arena.print();
    //cout << endl;

    //cout << "Moving right" << endl;
    if(tempr.right() && !isrepeat(alreadyseen, tempr)){
        pthread_rwlock_wrlock(&asLock);
        alreadyseen.insert(histNode(tempr));
        pthread_rwlock_unlock(&asLock);

        if(tempr.isSolved()){
            isfinished = true;
            finalbt = tempr.getlastmove();
            return;
        }
        int heurR = g.dept+1+tempr.getheur();
        pthread_rwlock_wrlock(&pqLock);
        pq.push(queueNode(tempr,g.dept+1,heurR));
        pthread_rwlock_unlock(&pqLock);
    }
    //cout << "Moving left" << endl;
    if(templ.left() && !isrepeat(alreadyseen, templ)){
        pthread_rwlock_wrlock(&asLock);
        alreadyseen.insert(histNode(templ));
        pthread_rwlock_unlock(&asLock);

        if(templ.isSolved()){
            isfinished = true;
            finalbt = templ.getlastmove();
            return;
        }

        int heurL = g.dept+1+templ.getheur();
        pthread_rwlock_wrlock(&pqLock);
        pq.push(queueNode(templ,g.dept+1,heurL));
        pthread_rwlock_unlock(&pqLock);
    }
    //cout << "Moving down" << endl;
    if(tempd.down() && !isrepeat(alreadyseen, tempd)){
        pthread_rwlock_wrlock(&asLock);
        alreadyseen.insert(histNode(tempd));
        pthread_rwlock_unlock(&asLock);

        if(tempd.isSolved()){
            isfinished = true;
            finalbt = tempd.getlastmove();
            return;
        }

        int heurD = g.dept+1+tempd.getheur();
        pthread_rwlock_wrlock(&pqLock);
        pq.push(queueNode(tempd,g.dept+1,heurD));
        pthread_rwlock_unlock(&pqLock);
    }
    //cout << "Moving up" << endl;
    if(tempu.up() && !isrepeat(alreadyseen, tempu)){
        pthread_rwlock_wrlock(&asLock);
        alreadyseen.insert(histNode(tempu));
        pthread_rwlock_unlock(&asLock);

        if(tempu.isSolved()){
            isfinished = true;
            finalbt = tempu.getlastmove();
            return;
        }

        int heurU = g.dept+1+tempu.getheur();
        pthread_rwlock_wrlock(&pqLock);
        pq.push(queueNode(tempu,g.dept+1,heurU));
        pthread_rwlock_unlock(&pqLock);
    }
    //cout << "adding " << toAddpq.size() << endl;
    //TODO: Check speed if you make it so that all nodes are added at once, reducing 3 locks to 1

    return;
}

//TODO: need to generate the vector int
void findSolution(gameState s){
    pqType pq;
    pq.push(queueNode(s, 0, s.getheur()));
    vector<thread> thd;
    gameState finishedState;
    //could be just tracking barrels + positi
    hist_cont alreadyseen;
    queueNode temp = pq.top();
    bool pqempty = false;
    //thd.push_back(thread(expandNode,cref(temp),ref(alreadyseen),ref(pq)));
    //expandNode(temp,alreadyseen,pq);
    while(!isfinished){
        //cout << "iter start" << endl;
        //cout << "size of already: " << alreadyseen.size()<< endl;

        //cout << thd.size() << endl;
        //cout << "Number of max threds: " <<  max_threads << endl;

        //while(pq.empty() || thd.size() > max_threads){
        while(pqempty || thd.size() > max_threads){
            for(auto it = thd.begin(); it < thd.end(); ++it){
                if(it->joinable()){
                    //cout << "stopping at join" << endl;
                    it->join();
                    //cout << "thread has been joined" << endl;
                    thd.erase(it);
                    //cout << "thread has been erased from vector, new size: " << thd.size() << endl;
                }
            }
            pthread_rwlock_rdlock(&pqLock);
            pqempty = pq.empty();
            pthread_rwlock_unlock(&pqLock);
        }
        //cout << "checking pq lock" << endl;

        pthread_rwlock_wrlock(&pqLock);
        temp = pq.top();
        pq.pop();
        pqempty = pq.empty();
        pthread_rwlock_unlock(&pqLock);
        thd.push_back(thread(expandNode,temp,ref(alreadyseen),ref(pq)));
        //expandNode(temp,alreadyseen,pq);
    }
    //cout << "priting results" << endl;
    printBt(finalbt);
    //cout << "waiting for all threads to end" << endl;
    for(auto &e: thd){
        e.join();
    }
    //TODO: generate some kind of backtrace


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
    

    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_time;

    start= chrono::system_clock::now();
    
    findSolution(arena);
    //cout << "ending the program" << endl;

    end = chrono::system_clock::now();
    elapsed_time = end-start;
    cout << "Time elapsed: " << elapsed_time.count() << endl;
}
