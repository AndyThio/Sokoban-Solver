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
#include <cmath>
#include <algorithm>
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
mutex printMut;

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
typedef vector<chrono::duration<double> > timecont;


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

bool isrepeat(const hist_cont &h, gameState c, chrono::duration<double> &astime_r,
        unsigned int &asrc, chrono::duration<double> &histtime, unsigned int &histc,
        timecont &asrv, timecont &hv, timecont &findv, chrono::duration<double> &findt,
        unsigned int &findc){
    chrono::time_point<chrono::system_clock> asstart, asend;
    chrono::time_point<chrono::system_clock> hstart, hend;
    chrono::time_point<chrono::system_clock> fstart, fend;
    ++asrc;
    ++histc;
    ++findc;
    hstart = chrono::system_clock::now();
    auto temp = histNode(c);
    hend = chrono::system_clock::now();
    asstart = chrono::system_clock::now();
    pthread_rwlock_rdlock(&asLock);
    asend = chrono::system_clock::now();
    fstart = chrono::system_clock::now();
    auto ret1 = h.find(temp);
    fend = chrono::system_clock::now();
    auto ret2 = h.end();
    pthread_rwlock_unlock(&asLock);
    astime_r += asend - asstart;
    histtime += hend - hstart;
    findt += fend - fstart;
    asrv.push_back(asend-asstart);
    hv.push_back(hend-hstart);
    findv.push_back(fend - fstart);
    return ret1 != ret2;
}

void expandDir(gameState g, int parent_depth, hist_cont &alreadyseen, pqType &pq,
        chrono::duration<double> &astime_w,chrono::duration<double> &astime_r,
        chrono::duration<double> &pqtime, unsigned int &asrc, unsigned int &aswc, unsigned int &pqc,
        chrono::duration<double> &histtime, unsigned int &histc,timecont &pqv, timecont &aswv,
        timecont &asrv, timecont &hv, timecont &findv, timecont &insertv,
        chrono::duration<double> &insertt, chrono::duration<double> &findt, unsigned int &insertc,
        unsigned int &findc){
    chrono::time_point<chrono::system_clock> pqstart, pqend;
    chrono::time_point<chrono::system_clock> asstart, asend;
    chrono::time_point<chrono::system_clock> hstart, hend;
    chrono::time_point<chrono::system_clock> istart, iend;

        if(!isrepeat(alreadyseen, g, astime_r, asrc, histtime, histc, asrv, hv, findv, findt, findc)){
            ++aswc;
            ++histc;
            ++insertc;
            asstart = chrono::system_clock::now();
            pthread_rwlock_wrlock(&asLock);
            asend = chrono::system_clock::now();
            hstart = chrono::system_clock::now();
            auto histtemp = histNode(g);
            hend = chrono::system_clock::now();

            istart = chrono::system_clock::now();
            alreadyseen.insert(histtemp);
            iend = chrono::system_clock::now();

            pthread_rwlock_unlock(&asLock);
            astime_w += asend - asstart;
            histtime += hend - hstart;
            insertt += iend - istart;
            aswv.push_back(asend-asstart);
            hv.push_back(hend- hstart);
            insertv.push_back(iend - istart);

            if(g.isSolved()){
                isfinished = true;
                btMut.lock();
                finalbt = g.getlastmove();
                btMut.unlock();
                isdone.notify_all();
                return;
            }
            int heur = parent_depth+1+g.getheur();
            ++pqc;
            pqstart = chrono::system_clock::now();
            pqMut.lock();
            pqend = chrono::system_clock::now();
            pq.push(queueNode(g,parent_depth+1,heur));
            pqMut.unlock();
            pqtime += pqend - pqstart;
            pqv.push_back(pqend-pqstart);

            cpq.notify_one();
        }
}

double stddev(vector<chrono::duration<double> > x, double mean){
    double meansum = 0;
    for(auto &e: x){
        meansum+= (e.count()-mean) * (e.count()-mean);
    }
    return sqrt(meansum/x.size());
}

double mini(const timecont &x){
    return min_element(x.begin(), x.end())->count();
}

double maxi(const timecont &x){
    return max_element(x.begin(), x.end())->count();
}

void expandNode(hist_cont &alreadyseen, pqType &pq){
    chrono::time_point<chrono::system_clock> start, end;
    chrono::time_point<chrono::system_clock> pqstart, pqend;
    chrono::time_point<chrono::system_clock> asstart, asend;
    chrono::duration<double> elapsed_time, astime_w, astime_r , pqtime, histtime,insertt,findt;
    timecont aswv, asrv, pqv, hv, insertv, findv;
    unsigned int asrc = 0,aswc = 0, pqc = 0, histc = 0, insertc =0, findc = 0;
    bool pqempty;
    queueNode temp;
    chrono::milliseconds ms{100};
    start = chrono::system_clock::now();
    while(!isfinished){

        ++pqc;
        pqstart = chrono::system_clock::now();
        pqMut.lock();
        pqend = chrono::system_clock::now();
        pqempty = pq.empty();
        if(!pqempty){
            temp = pq.top();
            pq.pop();
        }
        pqMut.unlock();
        pqtime += pqend - pqstart;
        pqv.push_back(pqend-pqstart);

        while(pqempty){
            unique_lock<mutex> lk(cpqMut);
            cpq.wait_for(lk,ms);

            if(isfinished){
                break;
            }
            ++pqc;
            pqstart = chrono::system_clock::now();
            pqMut.lock();
            pqend = chrono::system_clock::now();
            pqempty = pq.empty();
            if(!pqempty){
                temp = pq.top();
                pq.pop();
            }
            pqMut.unlock();
            pqtime += pqend - pqstart;
            pqv.push_back(pqend-pqstart);
        }
        if(isfinished){
            break;
        }

        gameState tempr = temp.arena;
        gameState templ = tempr;
        gameState tempu = tempr;
        gameState tempd = tempr;

        if(tempr.right()){
            expandDir(tempr,temp.dept, alreadyseen,pq,astime_w, astime_r, pqtime, asrc, aswc, pqc,
                    histtime, histc, pqv, aswv, asrv, hv, findv, insertv, insertt, findt, insertc,
                    findc);
        }
        if(templ.left()){
            expandDir(templ,temp.dept, alreadyseen,pq,astime_w, astime_r, pqtime, asrc, aswc, pqc,
                    histtime, histc, pqv, aswv, asrv, hv, findv, insertv, insertt, findt, insertc,
                    findc);
        }
        if(tempd.down()){
            expandDir(tempd,temp.dept, alreadyseen,pq,astime_w, astime_r, pqtime, asrc, aswc, pqc,
                    histtime, histc, pqv, aswv, asrv, hv, findv, insertv, insertt, findt, insertc,
                    findc);
        }
        if(tempu.up()){
            expandDir(tempu,temp.dept, alreadyseen,pq,astime_w, astime_r, pqtime, asrc, aswc, pqc,
                    histtime, histc, pqv, aswv, asrv, hv, findv, insertv, insertt, findt, insertc,
                    findc);
        }
    }
    end = chrono::system_clock::now();
    elapsed_time = end - start;

    printMut.lock();
    //thread time, pqtime, avgper pq, as write, avg as write, as read, avg asread
    cout << elapsed_time.count() << ", " << pqtime.count() << ", " << pqtime.count()/pqc << ", "
        << stddev(pqv, pqtime.count()/pqc) << ", " << mini(pqv) << ", " << maxi(pqv) << ", "
        << astime_w.count() << ", " << astime_w.count()/aswc << ", "
        << stddev(aswv, astime_w.count()/aswc) << ", " << mini(aswv) << ", " << maxi(aswv) << ", "
        << astime_r.count() << ", "  << astime_r.count()/asrc << ", "
        << stddev(asrv, astime_r.count()/asrc) << ", "  << mini(asrv) << ", " << maxi(asrv)
        << ", "<< histtime.count() << ", "
        << histtime.count()/histc << ", " << stddev(hv, histtime.count()/histc) << ","
        << mini(hv) << ", " << maxi(hv) << ", " << insertt.count() << ", " << insertt.count()/insertc << ", "
        << stddev(insertv, insertt.count()/insertc) << ", " << mini(insertv) << ", " << maxi(insertv) << ", " << findt.count() << ", "
        << findt.count()/findc << ", " << stddev(findv, findt.count()/findc) << ", " << mini(findv) << ", " << maxi(findv) << ", " ;
    printMut.unlock();
}

void findSolution(gameState s){
    pqType pq;
    pq.push(queueNode(s, 0, s.getheur()));
    vector<thread> thd;
    gameState finishedState;
    hist_cont alreadyseen;

    queueNode temp = pq.top();

    while(!isfinished){
        while(thd.size() < max_threads){
            thd.push_back(thread(expandNode,ref(alreadyseen),ref(pq)));
        }
        unique_lock<mutex> wlk(isdoneMut);
        isdone.wait(wlk);
    }

    cpq.notify_all();

    for(auto &e: thd){
        e.join();
    }

    btMut.lock();
    printBt(finalbt);
    btMut.unlock();
}

int main(int argc, char* argv[]){
    gameState arena;
    bool verb = false;
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
    for(int i = max_threads; i < 15; ++i){
        for( int j = 0; j < 31; ++j){
            cout << "-1, ";
        }
    }
    cout << elapsed_time.count() << endl;
}
