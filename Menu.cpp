//
// Created by KohsukeTanaka on 2019-07-01.
//

#include"Menu.h"
#include"SceneMgr.h"
#include"SDL2_DxLib.h"
#include"preferense.h"
#include"stdlib.h"
#include"CheckKey.h"

using namespace preferense;

Menu::Menu(ISceneChanger *changer) : BaseScene(changer) {

};

void Menu::Initialize() {};

void Menu::Finalize() {};

void Menu::Update() {
    if (checkKey.CheckHitKey(KEY_INPUT_ESCAPE)) {
        exit(1);
    }
    if (checkKey.CheckHitKey(KEY_INPUT_2)) {
        this->mSceneChanger->ChangeScene(eScene_Shoot);
    }
}

void Menu::Draw() {
    DrawString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "Here is Menu", GetColor(255, 255, 255));
}