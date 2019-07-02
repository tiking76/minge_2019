//
// Created by KohsukeTanaka on 2019-07-02.
//

#ifndef INC_2019_MINGE_PLAYER_H
#define INC_2019_MINGE_PLAYER_H

#include "Character.h"
#include"string"

class Player : public Character {
protected:
public:
    Player(const std::string &graphName,int posX,int posY);
    void Update();
    void Draw();
    ~Player() override = default;
};


#endif //INC_2019_MINGE_PLAYER_H
