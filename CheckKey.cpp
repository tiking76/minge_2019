//
// Created by KohsukeTanaka on 2019-07-01.
//

#include "CheckKey.h"
#include "SDL2_DxLib.h"

CheckKey::CheckKey() {
    for (int i = 0; i < 256; i++) {
        this->key[i] = 0;
        this->tmpKey[i] = 0;
    }
}

void CheckKey::Update() {
    GetHitKeyStateAll(this->tmpKey);
    for (int i = 0; i < 256; i++) {
        if (tmpKey[i] != 0) {
            key[i]++;
        } else {
            key[i] = 0;
        }
    }
}

int CheckKey::CheckHitKey(int index) {
    return this->key[index];
}

CheckKey checkKey;