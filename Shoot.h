//
// Created by KohsukeTanaka on 2019-07-01.
//

#ifndef INC_2019_MINGE_SHOOT_H
#define INC_2019_MINGE_SHOOT_H

#include"SceneMgr.h"
#include "Player.h"
class Shoot : BaseScene {
private:
    Player *player;
public:
    explicit Shoot(ISceneChanger *changer) : BaseScene(changer) {};

    void Initialize() override;

    void Finalize() override;

    void Update() override;

    void Draw() override;
};


#endif //INC_2019_MINGE_SHOOT_H
