#include <iostream>

#include "gameState.h"

gameState::gameState(vector<vector<int> > field, pair<int,int> p, int b, int t)
    :arena(field),position(p), barrels(b), targets(t)
{}

void update1step(pair<int, int> move){
    arena.at(position.first).at(position.second) = 0;
    arena.at(move.first).at(move.second)

vector<vector<int> > updateArena(pair<int,int> move){
    int arenaSizeX = arena.size();
    int arenaSizeY = arena.at(0).size();
    int ahead, dir, temp;

    if(arenaSizeX <= position.first || arenaSizeX <= move.first || arenaSizeY <= position.second
            || arenaSizeY <= move.second || 0 > position.first || 0 > position.second ||
            || 0 > move.first || 0 > move.second){
        return NULL;
    }

    switch(arena.at(move.first).at(move.second)) {
        case 0:
            arena.at(position =
            return true;
            break;
        case 1:
            if(position.first == move.first){
                dir = move.second - position.second;
                temp = move.second+dir;
                if(temp < 0 || temp >= arenaSizeY){
                    return NULL;
                }
                ahead = arena.at(move.first).at(temp);
            }
            else{
                dir = move.first-position.first;
                temp = move.first-dir;
                if(temp < 0 || temp >= arenaSizeX){
                    return NULL;
                }
                ahead = arena.at(move.first - dir).at(move.second);
            }


            switch(ahead){
                case 0: return true;
                    break;
                case 1: return NULL;
                    break;
                case 2: return true;
                    break;
                case 3: return NULL;
                    break;
                case 4: return NULL;
                    break;
                case 5: cerr << "Cannot have more than one player" << endl;
                        return NULL;
                    break;
                case default: cerr << "Invalid Space Type" << endl;
                        return NULL;
                    break;
            }
            break;
        case 2: return true;
            break;
        case 3:
            if(position.first == move.first){
                dir = move.second - position.second;
                temp = move.second+dir;
                if(temp < 0 || temp >= arenaSizeY){
                    return NULL;
                }
                ahead = arena.at(move.first).at(temp);
            }
            else{
                dir = move.first-position.first;
                temp = move.first-dir;
                if(temp < 0 || temp >= arenaSizeX){
                    return NULL;
                }
                ahead = arena.at(move.first - dir).at(move.second);
            }


            switch(ahead){
                case 0: return true;
                    break;
                case 1: return NULL;
                    break;
                case 2: return true;
                    break;
                case 3: return NULL;
                    break;
                case 4: return NULL;
                    break;
                case 5: cerr << "Cannot have more than one player" << endl;
                        return NULL;
                    break;
                case default: cerr << "Invalid Space Type" << endl;
                        return NULL;
                    break;
            }
            break;
        case 4: return NULL;
            break;
        case 5: cerr << "Cannot have more than one player" << endl;
                return NULL;
            break;
        case default: cerr << "Invalid Space Type" << endl;
                return NULL;
            break;
    }

}

vector<vector<int> > gameState::updateArena(pair<int, int> move){
    if(!isValid(move)){
        return NULL;
    }
    int ahead =

bool gameState::down(){
    int update = position.first+1;
