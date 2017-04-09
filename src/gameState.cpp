#include <iostream>

#include "gameState.h"

gameState::gameState(vector<vector<int> > field, pair<int,int> p)
    :arena(field),position(p)
{}

void gameState::updateplayer(){
    if( arena.at(position.first).at(position.second) == 6){
        arena.at(position.first).at(position.second) = 2;
    }
    else{
        arena.at(position.first).at(position.second) = 0;
    }
}

void gameState::updatespot(pair<int,int> spot, int new_space){
    arena.at(spot.first).at(spot.second) = new_space;
}

bool gameState::updateArena(pair<int,int> move){
    int arenaSizeX = arena.size();
    int arenaSizeY = arena.at(0).size();
    int ahead, dir, temp;
    pair<int,int> second_move;

    if(arenaSizeX <= position.first || arenaSizeX <= move.first || arenaSizeY <= position.second
            || arenaSizeY <= move.second || 0 > position.first || 0 > position.second ||
            || 0 > move.first || 0 > move.second){
        return false;
    }

    switch(arena.at(move.first).at(move.second)) {
        case 0:
            updateplayer();
            updatespot(move,5);
            return true;
            break;
        case 1:
            if(position.first == move.first){
                dir = move.second - position.second;
                temp = move.second+dir;
                if(temp < 0 || temp >= arenaSizeY){
                    return false;
                }
                ahead = arena.at(move.first).at(temp);
                second_move = make_pair(move.first,temp);
            }
            else{
                dir = move.first-position.first;
                temp = move.first-dir;
                if(temp < 0 || temp >= arenaSizeX){
                    return false;
                }
                ahead = arena.at(temp).at(move.second);
                second_move = make_pair(temp,move.second);
            }


            switch(ahead){
                case 0:
                    updateplayer();
                    updatespot(move,5);
                    updatespot(second_move,1);
                    return true;
                    break;
                case 1: return false;
                    break;
                case 2:
                    updateplayer();
                    updatespot(move,5);
                    updatespot(second_move,3);
                    return true;
                    break;
                case 3: return false;
                    break;
                case 4: return false;
                    break;
                case 5: cerr << "Cannot have more than one player" << endl;
                        return false;
                    break;
                case default: cerr << "Invalid Space Type" << endl;
                        return false;
                    break;
            }
            break;
        case 2:
            updateplayer();
            updatespot(move,6);
            return true;
            break;
        case 3:
            if(position.first == move.first){
                dir = move.second - position.second;
                temp = move.second+dir;
                if(temp < 0 || temp >= arenaSizeY){
                    return false;
                }
                ahead = arena.at(move.first).at(temp);
                second_move = make_pair(move.first,temp);
            }
            else{
                dir = move.first-position.first;
                temp = move.first-dir;
                if(temp < 0 || temp >= arenaSizeX){
                    return false;
                }
                ahead = arena.at(temp).at(move.second);
                second_move = make_pair(temp,move.second);
            }



            switch(ahead){
                case 0:
                    updateplayer();
                    updatespot(move, 6);
                    updatespot(second_move, 1);
                    return true;
                    break;
                case 1: return false;
                    break;
                case 2:
                    updateplayer();
                    updatespot(move,6);
                    updatespot(second_move, 3);
                    return true;
                    break;
                case 3: return false;
                    break;
                case 4: return false;
                    break;
                case 5: cerr << "Cannot have more than one player" << endl;
                        return false;
                    break;
                case default: cerr << "Invalid Space Type" << endl;
                        return false;
                    break;
            }
            break;
        case 4: return false;
            break;
        case 5: cerr << "Cannot have more than one player" << endl;
                return false;
            break;
        case default: cerr << "Invalid Space Type" << endl;
                return false;
            break;
    }

}

bool gameState::down(){
    int update = position.first+1;
    return updateArena(make_pair(update, position.second);
}

bool gameState::up(){
    int update = position.first-1;
    return updateArena(make_pair(update, position.second);
}

bool gameState::right(){
    int update = position.second+1;
    return updateArena(make_pair(position.first, update);
}

bool gameState::left(){
    int update = position.second-1;
    return udpateArena(make_pair(position.first, update);
}

bool gameState::isSolved(){
    for(int i = 0; i < arena.size(); ++i){
        for(int j = 0; j < arena.at(i).size(); ++j){
            switch(arena.at(i).at(j)){
                case 0: break;
                case 1: return false;
                case 2: return false;
                case 3: break;
                case 4: break;
                case 5: break;
                case 6: return false;
            }
        }
    }
    return true;
}

std::vector<vector<int> > gameState::getArena(){
    return arena;
}
