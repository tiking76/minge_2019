//
// Created by KohsukeTanaka on 2019-07-01.
//

#ifndef INC_2019_MINGE_CHECKKEY_H
#define INC_2019_MINGE_CHECKKEY_H


class CheckKey {
private:
    char key[256];
    char tmpKey[256];
public:
    CheckKey();

    void Update();

    int CheckHitKey(int index);
};

extern CheckKey checkKey;


#endif //INC_2019_MINGE_CHECKKEY_H
