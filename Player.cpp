//
// Created by KohsukeTanaka on 2019-07-02.
//

#include "Player.h"
#include "SDL2_DxLib.h"
#include "string"
#include "CheckKey.h"

Player::Player(const std::string &graphName, int posX, int posY) : Character(graphName, posX, posY) {
}

void Player::Update() {
    if (checkKey.CheckHitKey(KEY_INPUT_LEFT)) {
        this->posX -= 1;
    }
    if (checkKey.CheckHitKey(KEY_INPUT_RIGHT)) {
        this->posX += 1;
    }
    if (checkKey.CheckHitKey(KEY_INPUT_UP)) {
        this->posY -= 1;
    }
    if (checkKey.CheckHitKey(KEY_INPUT_DOWN)) {
        this->posY += 1;
    }
}

void Player::Draw() {
    if (graph == -1) {
        DrawBox(0, 0, 100, 100, GetColor(255, 255, 255), true);
    }
    DrawGraph(posX, posY, graph, true);

}
