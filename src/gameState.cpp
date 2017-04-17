#include <iostream>
#include <limits>

#include "gameState.h"


gameState::gameState()
{}

gameState::gameState(std::vector<std::vector<int> > field, std::pair<int,int> p, int w)
{
    for(int i = 0; i < field.size(); ++i){
        for(int j = 0; j < field.at(i).size(); ++j){
            switch(field.at(i).at(j)){
                case 0:
                    barrels.push_back(false);
                    targets.push_back(false);
                    walls.push_back(false);
                    break;
                case 1:
                    barrels.push_back(true);
                    targets.push_back(false);
                    walls.push_back(false);
                    break;
                case 2:
                    barrels.push_back(false);
                    targets.push_back(true);
                    walls.push_back(false);
                    break;
                case 3:
                    barrels.push_back(true);
                    targets.push_back(true);
                    walls.push_back(false);
                    break;
                case 4:
                    barrels.push_back(false);
                    targets.push_back(false);
                    walls.push_back(true);
                    break;
                case 5:
                    barrels.push_back(false);
                    targets.push_back(false);
                    walls.push_back(false);
                    break;
                case 6:
                    barrels.push_back(false);
                    targets.push_back(true);
                    walls.push_back(false);
                    break;
                default:
                    std::cerr << "No matching symbol" << std::endl;
                    exit(0);
            }
        }
    }
    position = p;
    width = w;
}

bool gameState::updateArena(std::pair<int,int> move){
    //print();
    //std::cout << position.first << ", " << position.second << std::endl;
    int arenaSizeX = barrels.size()/width;
    int arenaSizeY = width;
    int dir, temp;
    int moves = move.first * width + move.second;
    int move2;

    if(arenaSizeX <= position.first || arenaSizeX <= move.first || arenaSizeY <= position.second
            || arenaSizeY <= move.second || 0 > position.first || 0 > position.second
            || 0 > move.first || 0 > move.second){
        return false;
    }
    //std::cout << "a  found " << moves << " , "<< barrels.size() << std::endl;
    barrels.at(moves);

    //std::cout << "going to check the moves" <<  std::endl;
    if(walls.at(moves)){
        //std::cout << "a wall was hit" << std::endl;
        return false;
    }

    else if(barrels.at(moves)){
        //std::cout << "a Barrel was found" << std::endl;
        if(position.first == move.first){
            dir = move.second - position.second;
            temp = move.second+dir;
            if(temp < 0 || temp >= arenaSizeY){
                return false;
            }
            move2 = move.first*width + temp;
        }
        else{
            dir = move.first-position.first;
            temp = move.first+dir;
            if(temp < 0 || temp >= arenaSizeX){
                return false;
            }
            move2 = temp * width + move.second;
        }

        //std::cout << "second move was calculated" << std::endl;
        if(walls.at(move2)
                || barrels.at(move2)){
            return false;
        }
        else{
            position = move;
            barrels.at(moves) = false;
            barrels.at(move2) = true;
            return true;
        }
    }
    else{
        //std::cout << "Spot was empty" << std::endl;
        position = move;
        return true;
    }


}

bool gameState::down(){
    //std::cout << "pushing: Down" << std::endl;
    int update = position.first+1;
    lastmove.push_back(9);
    return updateArena(std::make_pair(update, position.second));
}

bool gameState::up(){
    //std::cout << "pushing: Up" << std::endl;
    int update = position.first-1;
    lastmove.push_back(10);
    return updateArena(std::make_pair(update, position.second));
}

bool gameState::right(){
    //std::cout << "pushing: Right" << std::endl;
    int update = position.second+1;
    lastmove.push_back(7);
    return updateArena(std::make_pair(position.first, update));
}

bool gameState::left(){
    //std::cout << "pushing: Left" << std::endl;
    int update = position.second-1;
    lastmove.push_back(8);
    return updateArena(std::make_pair(position.first, update));
}

bool gameState::isSolved(){
    if(barrels == targets){
        return true;
    }
    return false;
}

void gameState::print(){
    int pos = position.first * width + position.second;
    int i,j;
    for(int k = 0; k < barrels.size(); ++k){
        i = k/width;
        j = k%width;
        if(barrels.at(k) && targets.at(k)){
            std::cout << 3;
        }
        else if(targets.at(k) && k == pos){
            std::cout << 6;
        }
        else if(barrels.at(k)){
            std::cout << 1;
        }
        else if(targets.at(k)){
            std::cout << 2;
        }
        else if(walls.at(k)){
            std::cout << 4;
        }
        else if (pos == k){
            std::cout << 5;
        }
        else{
            std::cout << 0;
        }
        if((k+1)%width == 0){
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

int gameState::getheur(){
    //std::cout << "start" << std::endl;
    std::vector<std::pair<int,int> > barrel;
    std::vector<std::pair<int,int> > target;
    int h = 0;
    int i, j;

    for(int k = 0; k < barrels.size(); ++k){
        i = k/width;
        j = k%width;
        if(!(barrels.at(k) && targets.at(k))){
            if(barrels.at(k)){
                barrel.push_back(std::make_pair(i,j));
            }
            if(targets.at(k)){
                target.push_back(std::make_pair(i,j));
            }
        }
    }

    //std::cout << "inmid" << std::endl;
    std::pair<int,int> curr = position;
    while(!barrel.empty() || !target.empty()){
        //std::cout << "First for" << std::endl;
        int min = std::numeric_limits<int>::max();
        auto iter = barrel.begin();
        for(auto e = barrel.begin(); e != barrel.end();++e){
            int dist = abs((*e).first-curr.first) + abs((*e).second - curr.second);
            if( dist < min){
                min = dist;
                iter = e;
            }
        }
        curr = *iter;
        barrel.erase(iter);
        h+= min;

        //std::cout << "second for" << std::endl;
        min = std::numeric_limits<int>::max();
        iter = target.begin();
        for(auto e = target.begin(); e != target.end();++e){
            int dist = abs((*e).first-curr.first) + abs((*e).second - curr.second);
            if( dist < min){
                min = dist;
                iter = e;
            }
        }
        curr = *iter;
        target.erase(iter);
        h+= min;
    }
    //std::cout << "end" << std::endl << std::endl;
    return h;
}

std::vector<int> gameState::getlastmove(){
    return lastmove;
}

bool gameState::isequal(const gameState m){
    if(barrels != m.barrels || position != m.position){
        return false;
    }
    return true;
}
