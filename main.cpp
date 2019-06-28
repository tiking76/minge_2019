#include"SDL2_DxLib.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc,const char *argv[]){
    ChangeWindowMode(true);
    SetMainWindowText("test");
    SetWindowSizeChangeEnableFlag(false);
    SetGraphMode(WINDOW_WIDTH,WINDOW_HEIGHT,32);

    if(DxLib_Init()==-1){
        return -1;
    }

    int t0;
    while(ProcessMessage()==0&&CheckHitKey(KEY_INPUT_ESCAPE)==0){
        t0 = GetNowCount();
        ClearDrawScreen();
        DrawFormatString(60,200,GetColor(255,255,255),"Hello",32);

        while ((GetNowCount()-t0)<=(1000/60));
        ScreenFlip();
    }

    DxLib_End();

    return 0;
}