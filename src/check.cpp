#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
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

vector<string> load_backtrace(string filname){
    fstream fin(filname, fstream::in);
    if(!fin.is_open()){
        cerr << "Invalid Input File" << endl;
        exit(1);
    }
    vector<string> ret;
    string temp;
    while(fin >> temp){
        ret.push_back(temp);
    }
    return ret;
}

int main(int argc, char* argv[]){
    gameState arena = fetchArena(argv[1]);
    vector<string> moves = load_backtrace(argv[2]);
    for(auto &e : moves){
        if(e.compare("Right") == 0){
            if(!arena.right()){
                cerr << "Error: Invalid move right" << endl;
                return 1;
            }
        }
        else if(e.compare("Left") == 0){
            if(!arena.left()){
                cerr << "Error: Invalid move left" << endl;
                return 1;
            }
        }
        else if(e.compare("Down") == 0){
            if(!arena.down()){
                cerr << "Error: Invalid move down" << endl;
                return 1;
            }
        }
        else if(e.compare("Up") == 0){
            if(!arena.up()){
                cerr << "Error: Invalid move up" << endl;
                return 1;
            }
        }
    }
    if(arena.isSolved()){
        cout << "Success" << endl;
    }
    else{
        cout << "Error: Not all barrels on target" << endl;
    }
}
