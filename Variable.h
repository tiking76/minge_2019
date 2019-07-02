//
// Created by KohsukeTanaka on 2019-07-02.
//

#ifndef INC_2019_MINGE_VARIABLE_H
#define INC_2019_MINGE_VARIABLE_H


typedef struct{
    int posX1;
    int posY1;
    int posX2;
    int posY2;
    int graph;
    int MP;
    int HP;
}Character;

typedef struct {
    int posX1;
    int posY1;
    int posX2;
    int posY2;
    int graph;
}Enemy;

typedef struct{
    int posX1;
    int posY1;
    int posX2;
    int posY2;
    int graph;
    int HP;
}Boss;

typedef struct {
    int posX;
    int posY;
    int r;
    int graph;
}Bullet;

#endif //INC_2019_MINGE_VARIABLE_H
