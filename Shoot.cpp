//
// Created by KohsukeTanaka on 2019-07-01.
//

#include "Shoot.h"
#include "preferense.h"
#include"SDL2_DxLib.h"
#include "CheckKey.h"

using namespace preferense;

void Shoot::Update(){
    if (checkKey.CheckHitKey(KEY_INPUT_1)) {
        this->mSceneChanger->ChangeScene(eScene_Menu);
    }
}

void Shoot::Draw(){
    DrawString(WINDOW_WIDTH/2,WINDOW_HEIGHT/2,"Shoot",GetColor(255,255,255));
}