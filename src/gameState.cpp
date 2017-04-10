#include <iostream>
#include <limits>

#include "gameState.h"


gameState::gameState()
{}

gameState::gameState(std::vector<std::vector<int> > field, std::pair<int,int> p)
    :arena(field),position(p),lastmove(0)
{}

gameState::gameState(std::vector<std::vector<int> > field, std::pair<int,int> p, int l)
    :arena(field),position(p),lastmove(l)
{}

void gameState::updateplayer(){
    if( arena.at(position.first).at(position.second) == 6){
        arena.at(position.first).at(position.second) = 2;
    }
    else{
        arena.at(position.first).at(position.second) = 0;
    }
}

void gameState::updatespot(std::pair<int,int> spot, int new_space){
    arena.at(spot.first).at(spot.second) = new_space;
}

bool gameState::updateArena(std::pair<int,int> move){
    int arenaSizeX = arena.size();
    int arenaSizeY = arena.at(0).size();
    int ahead, dir, temp;
    std::pair<int,int> second_move;

    if(arenaSizeX <= position.first || arenaSizeX <= move.first || arenaSizeY <= position.second
            || arenaSizeY <= move.second || 0 > position.first || 0 > position.second
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
                second_move = std::make_pair(move.first,temp);
            }
            else{
                dir = move.first-position.first;
                temp = move.first+dir;
                if(temp < 0 || temp >= arenaSizeX){
                    return false;
                }
                ahead = arena.at(temp).at(move.second);
                second_move = std::make_pair(temp,move.second);
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
                case 5: std::cerr << "Cannot have more than one player" << std::endl;
                        return false;
                    break;
                default: std::cerr << "Invalid Space Type" << std::endl;
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
                second_move = std::make_pair(move.first,temp);
            }
            else{
                dir = move.first-position.first;
                temp = move.first+dir;
                if(temp < 0 || temp >= arenaSizeX){
                    return false;
                }
                ahead = arena.at(temp).at(move.second);
                second_move = std::make_pair(temp,move.second);
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
                case 5: std::cerr << "Cannot have more than one player" << std::endl;
                        return false;
                    break;
                default: std::cerr << "Invalid Space Type" << std::endl;
                        return false;
                    break;
            }
            break;
        case 4: return false;
            break;
        case 5: std::cerr << "Cannot have more than one player" << std::endl;
                return false;
            break;
        default: std::cerr << "Invalid Space Type" << std::endl;
                return false;
            break;
    }

}

bool gameState::down(){
    int update = position.first+1;
    lastmove = 10;
    return updateArena(std::make_pair(update, position.second));
}

bool gameState::up(){
    int update = position.first-1;
    lastmove = 9;
    return updateArena(std::make_pair(update, position.second));
}

bool gameState::right(){
    int update = position.second+1;
    lastmove = 7;
    return updateArena(std::make_pair(position.first, update));
}

bool gameState::left(){
    int update = position.second-1;
    lastmove = 8;
    return updateArena(std::make_pair(position.first, update));
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
                default: break;
            }
        }
    }
    return true;
}

std::vector<std::vector<int> > gameState::getArena(){
    return arena;
}

void gameState::print(){
    for(int i = 0; i < arena.size(); ++i){
        for(int j = 0; j < arena.at(i).size(); ++j){
            std::cout << arena.at(i).at(j);
        }
        std::cout << std::endl;
    }
}

int gameState::getheur(){
    std::vector<std::pair<int,int> > barrels;
    std::vector<std::pair<int,int> > targets;
    int h = 0;

    for(int i = 0; i < arena.size(); ++i){
        for(int j = 0; j < arena.at(i).size(); ++j){
            switch(arena.at(i).at(j)){
                case 1: barrels.push_back(std::make_pair(i,j));
                    break;
                case 2: targets.push_back(std::make_pair(i,j));
                    break;
                default: break;
            }
        }
    }

    std::pair<int,int> curr = position;
    while(!barrels.empty() || !targets.empty()){
        int min = std::numeric_limits<int>::max();
        auto iter = barrels.begin();
        for(auto e = barrels.begin(); e != barrels.end();++e){
            int dist = abs((*e).first-curr.first) + abs((*e).second - curr.second);
            if( dist < min){
                min = dist;
                curr = *e;
                iter = e;
            }
        }
        barrels.erase(iter);
        h+= min;

        min = std::numeric_limits<int>::max();
        iter = targets.begin();
        for(auto e = targets.begin(); e != targets.end();++e){
            int dist = abs((*e).first-curr.first) + abs((*e).second - curr.second);
            if( dist < min){
                min = dist;
                curr = *e;
                iter = e;
            }
        }
        targets.erase(iter);
        h+= min;
    }
    return h;
}

int gameState::getlastmove(){
    return lastmove;
}

void gameState::setlm(int lm){
    lastmove = lm;
}
