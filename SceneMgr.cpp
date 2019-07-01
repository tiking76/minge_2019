//
// Created by KohsukeTanaka on 2019-06-28.
//

#include "SceneMgr.h"
#include "Menu.h"
#include "Shoot.h"


SceneMgr::SceneMgr() :mNextScene(eScene_None) {
    mScene = (BaseScene *) new Menu(this);
}

void SceneMgr::Finalize() {
    this->mScene->Finalize();
}

void SceneMgr::Initialize() {
    this->mScene->Initialize();
}

void SceneMgr::Update(){
    if(mNextScene!=eScene_None){
        this->mScene->Finalize();
        delete mScene;
        switch(mNextScene){
            case eScene_Menu:
                mScene = (BaseScene *)new Menu(this);
                break;
            case eScene_Shoot:
                mScene = (BaseScene *)new Shoot(this);
                break;
            case eScene_None:
                break;
        }
        mNextScene = eScene_None;
        this->mScene->Initialize();
    }
    this->mScene->Update();
}

void SceneMgr::Draw() {
    mScene->Draw();
}

void SceneMgr::ChangeScene(eScene NextScene) {
    mNextScene = NextScene;
}