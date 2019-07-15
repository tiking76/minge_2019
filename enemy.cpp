//
// Created by 舘佳紀 on 2019-07-09.
//
#include  "SDL2_DxLib.h"
#include   <math.h>

const double pi = 3.1415926535;

struct enemy{
int e_x;
int e_y;
};


struct player{
    int p_x;
    int p_y;
};

struct sincarve{
    int cx = 100;
    int cy = 0;
    double dy;//y軸方向の加速度
    int phase;//波の位相(0 ～ 360)
    int phase_speed;//波の位相変化量(0 ～ 360)
    int amp;//波の振幅
};

enemy zako[100];
sincarve move;
player brave;
player v;

void init() {
    v.p_x = 0;
    v.p_y = 0;
    brave.p_x = 0;
    brave.p_y = 400;
    for (int i = 0; i < 100; i++) {
        zako[i].e_x = 640;//ここの値は合わして
        zako[i].e_y = 400;//ここも
        move.dy = 1.0;
        move.phase = 0;
        move.phase_speed = 10;
        move.amp = 50;
    }
}


void attack_gun(){
    int hoge;
    /*
     * 適当に作ってください
     */
}


player complicatevec(player a,enemy b){
    player vec;
    vec.p_x = b.e_x - a.p_x;
    vec.p_y = b.e_y - a.p_y;
    return vec;
}




void sinmove(int num_machine){
    int i = num_machine;
    move.phase += move.phase_speed;
    double d = sin(move.phase * pi/2);
    double x = move.cx - d * move.amp;
    move.cy += move.dy;
    zako[i].e_x = x;
    zako[i].e_y = move.cy;
}
void enemy_move(int num_machine){
    sinmove(num_machine);

}


void set_enemy(int graph_enemy) {
    //敵の画像を描画するときコレ呼んで！
    for (int i = 0; i < 100; i++) {
        int graph_flag = DrawGraph(0, 0, graph_enemy, TRUE);
        if(graph_flag == -1) {
            break;
        }
    }
}



int main(int args, const char *argv[]) {
    init();
    int graph_enemy = LoadGraph("hoge.png");
    set_enemy(graph_enemy);
    int count = 0;
    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
        enemy_move(count);
        count++;
    }
}






















