//
// Created by KohsukeTanaka on 2019-07-01.
//

#ifndef INC_2019_MINGE_MENU_H
#define INC_2019_MINGE_MENU_H
#include"SceneMgr.h"

class Menu :public BaseScene{
private:

public:
    explicit Menu(ISceneChanger* changer);
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;
};


#endif //INC_2019_MINGE_MENU_H
