//
// Created by KohsukeTanaka on 2019-06-27.
//

#ifndef INC_2019_MINGE_SDL2_DXLIB_H
#define INC_2019_MINGE_SDL2_DXLIB_H

#ifndef _SDL2_DXLIB_H_
#define _SDL2_DXLIB_H_

#define TRUE 1
#define FALSE 0

/*キーボードマクロ*/
#define KEY_INPUT_BACK 0x02A
#define KEY_INPUT_TAB 0x02B
#define KEY_INPUT_RETURN 0x028
#define KEY_INPUT_LSHIFT 0x0E1
#define KEY_INPUT_RSHIFT 0x0E5
#define KEY_INPUT_LCONTROL 0x0E3
#define KEY_INPUT_RCONTROL 0x0E7
#define KEY_INPUT_ESCAPE 0x029
#define KEY_INPUT_SPACE 0x02C
#define KEY_INPUT_LEFT 0x050
#define KEY_INPUT_UP 0x052
#define KEY_INPUT_RIGHT 0x04F
#define KEY_INPUT_DOWN 0x051
#define KEY_INPUT_MINUS 0x02D
#define KEY_INPUT_YEN 0x031
#define KEY_INPUT_PREVTRACK 0x02E
#define KEY_INPUT_PERIOD 0x037
#define KEY_INPUT_SLASH 0x038
#define KEY_INPUT_LALT 0x0E2
#define KEY_INPUT_RALT 0x0E6
#define KEY_INPUT_SCROLL 0x047
#define KEY_INPUT_SEMICOLON 0x033
#define KEY_INPUT_COLON 0x034
#define KEY_INPUT_LBRACKET 0x030
#define KEY_INPUT_RBRACKET 0x031
#define KEY_INPUT_AT 0x02F
#define KEY_INPUT_BACKSLASH 0x031
#define KEY_INPUT_COMMA 0x036
#define KEY_INPUT_CAPSLOCK 0x039
#define KEY_INPUT_F1 0x03A
#define KEY_INPUT_F2 0x03B
#define KEY_INPUT_F3 0x03C
#define KEY_INPUT_F4 0x03D
#define KEY_INPUT_F5 0x03E
#define KEY_INPUT_F6 0x03F
#define KEY_INPUT_F7 0x040
#define KEY_INPUT_F8 0x041
#define KEY_INPUT_F9 0x042
#define KEY_INPUT_F10 0x043
#define KEY_INPUT_F11 0x044
#define KEY_INPUT_F12 0x045
#define KEY_INPUT_A 0x004
#define KEY_INPUT_B 0x005
#define KEY_INPUT_C 0x006
#define KEY_INPUT_D 0x007
#define KEY_INPUT_E 0x008
#define KEY_INPUT_F 0x009
#define KEY_INPUT_G 0x00A
#define KEY_INPUT_H 0x00B
#define KEY_INPUT_I 0x00C
#define KEY_INPUT_J 0x00D
#define KEY_INPUT_K 0x00E
#define KEY_INPUT_L 0x00F
#define KEY_INPUT_M 0x010
#define KEY_INPUT_N 0x011
#define KEY_INPUT_O 0x012
#define KEY_INPUT_P 0x013
#define KEY_INPUT_Q 0x014
#define KEY_INPUT_R 0x015
#define KEY_INPUT_S 0x016
#define KEY_INPUT_T 0x017
#define KEY_INPUT_U 0x018
#define KEY_INPUT_V 0x019
#define KEY_INPUT_W 0x01A
#define KEY_INPUT_X 0x01B
#define KEY_INPUT_Y 0x01C
#define KEY_INPUT_Z 0x01D
#define KEY_INPUT_1 0x01E
#define KEY_INPUT_2 0x01F
#define KEY_INPUT_3 0x020
#define KEY_INPUT_4 0x021
#define KEY_INPUT_5 0x022
#define KEY_INPUT_6 0x023
#define KEY_INPUT_7 0x024
#define KEY_INPUT_8 0x025
#define KEY_INPUT_9 0x026
#define KEY_INPUT_0 0x027

/*音声再生形式*/
#define DX_PLAYTYPE_NORMAL -1
#define DX_PLAYTYPE_BACK 0
#define DX_PLAYTYPE_LOOP 1

/*Draw系列ブレンドモード*/
#define DX_BLENDMODE_NOBLEND 0
#define DX_BLENDMODE_ALPHA 1
#define DX_BLENDMODE_ADD 2
#define DX_BLENDMODE_SUB 3
#define DX_BLENDMODE_MULA 4
#define DX_BLENDMODE_INVSRC 5

/*マウスマクロ*/
#define MOUSE_INPUT_LEFT 1
#define MOUSE_INPUT_MIDDLE 2
#define MOUSE_INPUT_RIGHT 4
#define MOUSE_INPUT_4 8
#define MOUSE_INPUT_5 16

/**使用必須関数**/
int DxLib_Init();
int DxLib_End();
int ProcessMessage();

/**画面関連関数**/
int SetGraphMode(int width, int height, int bit);
int ChangeWindowMode(int flag);
int SetWindowSizeChangeEnableFlag(int flag);
int SetMainWindowText(const char *title);
int SetWindowSize(int width, int height);
int GetWindowSize(int *width, int *height);
int ClearDrawScreen();
int SetBackgroundColor(int r, int g, int b);
int ScreenFlip();
int SetDrawBlendMode(int mode, int pal);

/**キーボード系関数**/
int CheckHitKeyAll();
int CheckHitKey(int KeyCode);
int GetHitKeyStateAll(char *KeyState);

/**マウス系関数**/
int SetMouseDispFlag(int flag);
int GetMousePoint(int *X, int *Y);
int SetMousePoint(int X, int Y);
int GetMouseInput();
int GetMouseWheelRotVol();

/**図形描画関数**/
int DrawPixel(int x, int y, unsigned int color);
int DrawLine(int x1, int y1, int x2, int y2, unsigned int color, int thick = 1);
int DrawLineAA(float x1, float y1, float x2, float y2, unsigned int color, float thick = 1.0);
int DrawBox(int x1, int y1, int x2, int y2, unsigned int color, int fill);
int DrawBoxAA(float x1, float y1, float x2, float y2, unsigned int color, int fill, float thick = 1.0);
int DrawCircle(int x, int y, int r, unsigned int color, int fill = TRUE, int thick = 1);
int DrawCircleAA(float x, float y, float r, int posnum, unsigned int color, int fill = TRUE, float thick = 1.0);
int DrawTriangle(int x1, int y1, int x2, int y2 ,int x3, int y3, unsigned int color, int fill);
int DrawTriangleAA(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, int fill, float thick = 1.0);
int DrawOval(int x, int y, int rx, int ry, unsigned int color, int fill, int thick = 1);
int DrawOvalAA(float x, float y, float rx, float ry, int posnum, unsigned int color, int fill, float thick = 1.0);
int DrawOval_Rect(int x1, int y1, int x2, int y2, unsigned int color, int fill);
int DrawRoundRect(int x1, int y1, int x2, int y2, int rx, int ry, unsigned int color, int fill);
int DrawRoundRectAA(float x1, float y1, float x2, float y2, float rx, float ry, int posnum, unsigned int color, int fill, float thick = 1.0);

int DrawQuadrangle(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int color, int fill);
int DrawQuadrangleAA(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int color, int fill, float thick = 1.0);
unsigned int GetPixel(int x, int y);

/**画像関連関数**/
int InitGraph();
int LoadGraph(const char *path);
int LoadDivGraph(const char *path, int AllNum, int Xnum, int Ynum, int Xsize, int Ysize, int *graphBuf);
int MakeGraph(int w, int h);
int DerivationGraph(int tx, int ty, int w, int h, int graph);
int DrawGraph(int x, int y, int graph, int trans);
int DrawGraphF(float x, float y, int graph, int trans);
int DrawRotaGraph(int x, int y, double Ex, double angle, int graph, int trans, int turnX = FALSE, int turnY = FALSE);
int DrawRotaGraphF(float x, float y, double Ex, double angle, int graph, int trans, int turnX = FALSE, int turnY = FALSE);
int DrawTurnGraph(int x, int y, int graph, int trans);
int DrawTurnGraphF(float x, float y, int graph, int trans);
int DrawReverseGraph(int x, int y, int graph, int trans, int turnX = FALSE, int turnY = FALSE);
int DrawReverseGraphF(float x, float y, int graph, int trans, int turnX = FALSE, int turnY = FALSE);
int DrawExtendGraph(int x1, int y1, int x2, int y2, int graph, int trans);
int DrawExtendGraphF(float x1, float y1, float x2, float y2, int graph, int trans);
int DrawModiGraph(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int graph, int trans);
int DrawModiGraphF(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int graph, int trans);
int DrawRectGraph(int x, int y, int tx, int ty, int w, int h, int graph, int trans, int turnX = FALSE, int turnY = FALSE);
int DrawRectGraphF(float x, float y, int tx, int ty, int w, int h, int graph, int trans, int turnX = FALSE, int turnY = FALSE);
int DrawRectRotaGraph(int x, int y, int tx, int ty, int w, int h, double Ex, double angle, int graph, int trans, int turnX = FALSE, int turnY = FALSE);
int DrawRectRotaGraphF(float x, float y, int TX, int TY, int w, int h, double Ex, double angle, int graph, int trans, int turnX = FALSE, int turnY = FALSE);
int DrawRectExtendGraph(int x1, int y1, int x2, int y2, int tx, int ty, int w, int h, int graph, int trans);
int DrawRectExtendGraphF(float x1, float y1, float x2, float y2, int tx, int ty, int w, int h, int graph, int trans);
int DrawRectModiGraph(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int tx, int ty, int w, int h, int graph, int trans);
int DrawRectModiGraphF(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int tx, int ty, int w, int h, int graph, int trans);
int DeleteGraph(int graph);
int GetGraphSize(int graph, int *width, int *height);
int SaveDrawScreen(int x1, int y1, int x2, int y2, const char *name);
int SaveDrawScreenToBMP(int x1, int y1, int x2, int y2, const char *name);
int SaveDrawScreenToJPEG(int x1, int y1, int x2, int y2, const char *name, int quality = 80);
int SaveDrawScreenToPNG(int x1, int y1, int x2, int y2, const char *name);


/**音声関連関数**/
int InitSoundMem();
int LoadSoundMem(const char *path);
int DeleteSoundMem(int sound);
int PlaySoundMem(int sound, int type, int TopPosFlag = TRUE);
int CheckSoundMem(int sound);
int StopSoundMem(int sound);
int ChangeVolumeSoundMem(int volume, int sound);

/**文字列描画関数**/
int ChangeFont(const char *fontName);
int SetFontSize(int size);
int SetAAstate(bool state);
int DrawString(int x, int y, const char *str, unsigned int color);
int DrawFormatString(int x, int y, unsigned int color, const char *str, ...);
int CreateFontToHandle(const char *fontName, int size, bool alias, bool italic = false);
int DrawStringToHandle(int x, int y, const char *str, unsigned int color, int FontHandle);
int DrawFormatStringToHandle(int x, int y, unsigned int color, int FontHandle, const char *str, ...);
int DeleteFontToHandle(int FontHandle);
int InitFontToHandle();

/**共通使用関数**/
int GetNowCount();
unsigned int GetColor(int r, int g, int b);
int GetRand(int N);
#endif








#endif //INC_2019_MINGE_SDL2_DXLIB_H
