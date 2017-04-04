#include <iostream>
#include <fstream>
#include <pthreads.h>
#include <vector>
#include <string>
using namespace std;

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

vector<vector<int> > formArena(){
    vector<vector<int> > arena;
    string fname;
    cout << "Enter text file name: ";
    cin >> fname;

    /*
     * Text File Requirements:
     * - All integers
     * - First number should be height
     * - Second number should be width
     * - numbers should be separated by whitespace
     * - See "Types of Spaces" for what numbers to put for different spaces
     */

    fstream fin(fname, fstream::in);
    int height, width,spot;
    fin >> height >> width;
    arena.resize(height);

    for (int i = 0; i < height ; ++i){
        for (int j = 0; j < width; ++j){
            fin >> spot;
            arena.at(i).push_back(spot);
        }
    }

    return arena;
}

int main(){
    vector<vector<int> > arena;

}
