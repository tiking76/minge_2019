//
// Created by KohsukeTanaka on 2019-07-02.
//

#ifndef INC_2019_MINGE_CHARACTER_H
#define INC_2019_MINGE_CHARACTER_H
#include"string"
#include "SDL2_DxLib.h"


class Character {
protected:
    int graph;
    int posX;
    int posY;
public:
    Character(const std::string &graphName,int posX,int posY):posX(posX),posY(posY) {
        this->graph = LoadGraph(graphName.c_str());
        printf("%d\n",graph);
    }
    virtual ~Character(){
        DeleteGraph(this->graph);
    }
    int getposX(){
        return this->posX;
    }
    int getposY(){
        return this->posY;
    }
};


#endif //INC_2019_MINGE_CHARACTER_H
