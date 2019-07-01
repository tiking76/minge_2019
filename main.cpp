#include"SDL2_DxLib.h"
#include"preferense.h"
#include"SceneMgr.h"
#include"CheckKey.h"
using namespace preferense;

int main(int argc,const char *argv[]){
    ChangeWindowMode(true);
    SetMainWindowText("test");
    SetWindowSizeChangeEnableFlag(false);
    SetGraphMode(WINDOW_WIDTH,WINDOW_HEIGHT,32);

    if(DxLib_Init()==-1){
        return -1;
    }

    SceneMgr sceneMgr;
    sceneMgr.Initialize();
    sceneMgr.Finalize();

    while(ProcessMessage()==0){
        checkKey.Update();
        ClearDrawScreen();

        sceneMgr.Update();
        sceneMgr.Draw();

        ScreenFlip();
    }

    DxLib_End();

    return 0;
}