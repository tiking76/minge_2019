//
// Created by KohsukeTanaka on 2019-06-27.
//

#include "SDL2_DxLib.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>


#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>

#include <string>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstdarg>

using namespace std;

/**クラス**/
struct SCREEN_DATA {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width;
    int height;
    SDL_WindowFlags mode;
    string title;
    SDL_Color Back;
    int BlendMode;
    int Alpha;

    SCREEN_DATA() {
        window = NULL;
        renderer = NULL;
        width = 0;
        height = 0;
        mode = (SDL_WindowFlags) 0;
        title = "";
        Back.r = 0;
        Back.g = 0;
        Back.b = 0;
        Back.a = 0;
        BlendMode = DX_BLENDMODE_NOBLEND;
        Alpha = 0;
    }
};

struct MOUSE_STATE {
    int X;
    int Y;
    bool left;
    bool right;
    bool middle;
    bool b4;
    bool b5;
    int wheel;

    MOUSE_STATE() {
        X = 0;
        Y = 0;
        left = false;
        right = false;
        middle = false;
        b4 = false;
        b5 = false;
        wheel = 0;
    }
};

/**グローバル変数**/
static SCREEN_DATA Screen;
static bool KeyState[256];
static MOUSE_STATE MouseState;
static FILE *tmpfp;
static const double PI = 4 * atan(1.0);

struct GRAPH_SAVER {
    SDL_Surface *surface;
    SDL_Texture *texture;
    bool success;

    GRAPH_SAVER() {
        surface = NULL;
        texture = NULL;
        success = false;
    }

    GRAPH_SAVER(SDL_Surface *load) {
        success = true;
        if (load == NULL) {
            success = false;
            texture = NULL;
            surface = NULL;
        } else {
            surface = load;
            texture = SDL_CreateTextureFromSurface(Screen.renderer, surface);
            if (texture == NULL)
                success = false;
        }
    }

    ~GRAPH_SAVER() {
        SDL_FreeSurface(surface);
        if (texture != NULL)
            SDL_DestroyTexture(texture);
    }
};

struct SOUND_SAVER {
    Mix_Chunk *chunk;
    int channel;
    bool pause;
    bool success;

    SOUND_SAVER() {
        chunk = NULL;
        channel = 0;
        success = false;
        pause = false;
    }

    SOUND_SAVER(const char *path, int number) {
        chunk = Mix_LoadWAV(path);
        channel = number;
        success = chunk != NULL;
        pause = false;
    }

    ~SOUND_SAVER() {
        if (chunk != NULL)
            Mix_FreeChunk(chunk);
    }
};

struct FONT_SAVER {
    TTF_Font *font;
    string fontPath;
    bool success;
    int SIZE;
    bool AA;
    bool it;

    FONT_SAVER() {
        SIZE = 30;
        AA = true;
        it = false;
        fontPath = "/Library/Fonts/Arial Unicode.ttf";
        font = TTF_OpenFont(fontPath.c_str(), SIZE);
        success = font != NULL;
    }

    FONT_SAVER(string fontName, int size, bool anti, bool italic = false) {
        SIZE = size;
        AA = anti;
        it = italic;
        fontPath = fontName;
        font = TTF_OpenFont(fontPath.c_str(), SIZE);

        if (font == NULL) {
            fontPath = "/Library/Fonts/" + fontName;
            font = TTF_OpenFont(fontPath.c_str(), SIZE);

            if (font == NULL) {
                fontPath = "/System/Library/Fonts/" + fontName;
                font = TTF_OpenFont(fontPath.c_str(), SIZE);
            }
        }

        success = font != NULL;
    }

    ~FONT_SAVER() {
        TTF_CloseFont(font);
        font = NULL;
    }
};

static int GraphNumber;
static map<int, GRAPH_SAVER *> GraphDataBase;
static int SoundNumber;
static map<int, SOUND_SAVER *> SoundDataBase;
static int FontNumber;
static map<int, FONT_SAVER *> FontDataBase;
static FONT_SAVER *DefaultFont;


/**内部関数**/
static void SetBlend2DFigure(Uint8 r, Uint8 g, Uint8 b) {
    switch (Screen.BlendMode) {
        case DX_BLENDMODE_MULA:
        case DX_BLENDMODE_NOBLEND:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4ub(r, g, b, 255);
            break;

        case DX_BLENDMODE_ALPHA:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4ub(r, g, b, Screen.Alpha);
            break;

        case DX_BLENDMODE_ADD:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4ub(r, g, b, Screen.Alpha);
            break;

        case DX_BLENDMODE_SUB:
            glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4ub(r, g, b, Screen.Alpha);
            break;

        case DX_BLENDMODE_INVSRC:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4ub(255 - r, 255 - g, 255 - b, Screen.Alpha);
            break;
    }
}

static void SetBlendGraphics() {
    switch (Screen.BlendMode) {
        case DX_BLENDMODE_MULA:
        case DX_BLENDMODE_INVSRC:
        case DX_BLENDMODE_NOBLEND:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4ub(255, 255, 255, 255);
            break;

        case DX_BLENDMODE_ALPHA:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4ub(255, 255, 255, Screen.Alpha);
            break;

        case DX_BLENDMODE_ADD:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4ub(255, 255, 255, Screen.Alpha);
            break;

        case DX_BLENDMODE_SUB:
            glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4ub(255, 255, 255, Screen.Alpha);
            break;
    }
}

/**使用必須関数**/
int DxLib_Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return -1;
    if (TTF_Init() == -1)
        return -1;

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF | IMG_INIT_WEBP);
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_FLAC | MIX_INIT_MOD);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    if ((Screen.window = SDL_CreateWindow(Screen.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          Screen.width, Screen.height, Screen.mode)) == NULL)
        return -1;

    for (int i = 0; i < SDL_GetNumRenderDrivers(); i++) {
        SDL_RendererInfo info;
        if (SDL_GetRenderDriverInfo(i, &info) != 0)
            return -1;
        if (strcmp(info.name, "opengl") == 0) {
            Screen.renderer = SDL_CreateRenderer(Screen.window, i, 0);
            break;
        }
    }

    if (Screen.renderer == NULL)
        return -1;
    if ((tmpfp = tmpfile()) == NULL)
        return -1;

    if (Screen.mode == SDL_WINDOW_FULLSCREEN)
        SDL_ShowCursor(SDL_DISABLE);
    else
        SDL_ShowCursor(SDL_ENABLE);

    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_AllocateChannels(0xffff);

    for (int i = 0; i < 256; i++)
        KeyState[i] = false;

    srand((unsigned) time(NULL));

    InitGraph();
    InitSoundMem();
    InitFontToHandle();

    DefaultFont = new FONT_SAVER();

    return 0;
}

int DxLib_End() {
    delete DefaultFont;

    InitGraph();
    InitSoundMem();
    InitFontToHandle();

    SDL_DestroyRenderer(Screen.renderer);
    SDL_DestroyWindow(Screen.window);
    fclose(tmpfp);

    Mix_CloseAudio();
    IMG_Quit();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}

int ProcessMessage() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return -1;

            case SDL_KEYUP:
                KeyState[event.key.keysym.scancode] = false;
                break;

            case SDL_KEYDOWN:
                KeyState[event.key.keysym.scancode] = true;
                break;

            case SDL_MOUSEBUTTONDOWN:
                MouseState.X = event.button.x;
                MouseState.Y = event.button.y;

                if (event.button.button == SDL_BUTTON_LEFT)
                    MouseState.left = true;
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    MouseState.right = true;
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                    MouseState.middle = true;
                else if (event.button.button == SDL_BUTTON_X1)
                    MouseState.b4 = true;
                else if (event.button.button == SDL_BUTTON_X2)
                    MouseState.b5 = true;
                break;

            case SDL_MOUSEBUTTONUP:
                MouseState.X = event.button.x;
                MouseState.Y = event.button.y;

                if (event.button.button == SDL_BUTTON_LEFT)
                    MouseState.left = false;
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    MouseState.right = false;
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                    MouseState.middle = false;
                else if (event.button.button == SDL_BUTTON_X1)
                    MouseState.b4 = false;
                else if (event.button.button == SDL_BUTTON_X2)
                    MouseState.b5 = false;
                break;

            case SDL_MOUSEMOTION:
                MouseState.X = event.motion.x;
                MouseState.Y = event.motion.y;
                break;

            case SDL_MOUSEWHEEL:
                MouseState.wheel += -event.wheel.y;
                break;
        }
    }

    return 0;
}


/**画面関連関数**/
int SetGraphMode(int width, int height, int bit) {
    GraphDataBase.clear();

    Screen.width = width;
    Screen.height = height;

    if (Screen.window != NULL && Screen.renderer != NULL) {
        SDL_DestroyRenderer(Screen.renderer);
        SDL_DestroyWindow(Screen.window);

        if ((Screen.window = SDL_CreateWindow("DxLib", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Screen.width,
                                              Screen.height, Screen.mode)) == NULL)
            return -1;
        if ((Screen.renderer = SDL_CreateRenderer(Screen.window, -1, 0)) == NULL)
            return -1;
    }

    return 0;
}

int ChangeWindowMode(int flag) {
    if (Screen.window != NULL || (flag != TRUE && flag != FALSE))
        return -1;

    Screen.mode = flag == TRUE ? (SDL_WindowFlags) 0 : SDL_WINDOW_FULLSCREEN;

    return 0;
}

int SetWindowSizeChangeEnableFlag(int flag) {
    if (flag != TRUE && flag != FALSE)
        return -1;

    if (Screen.window == NULL)
        Screen.mode = flag ? SDL_WINDOW_RESIZABLE : Screen.mode;
    else {
        if (flag && Screen.mode != SDL_WINDOW_FULLSCREEN) {
            Screen.mode = SDL_WINDOW_RESIZABLE;
            SDL_SetWindowResizable(Screen.window, SDL_TRUE);
        } else {
            SDL_SetWindowResizable(Screen.window, SDL_FALSE);
        }
    }

    return 0;
}

int SetMainWindowText(const char *title) {
    if (Screen.window == NULL)
        Screen.title = title;
    else {
        Screen.title = title;
        SDL_SetWindowTitle(Screen.window, title);
    }

    return 0;
}

int SetWindowSize(int width, int height) {
    if (Screen.window == NULL)
        return -1;

    Screen.width = width;
    Screen.height = height;
    SDL_SetWindowSize(Screen.window, width, height);
    return 0;
}

int GetWindowSize(int *width, int *height) {
    if (Screen.window == NULL)
        return -1;

    SDL_GetWindowSize(Screen.window, width, height);
    return 0;
}

int ClearDrawScreen() {
    int result = 0;
    if (Screen.renderer == NULL)
        return -1;

    result |= SDL_SetRenderDrawColor(Screen.renderer, Screen.Back.r, Screen.Back.g, Screen.Back.b, Screen.Back.a);
    result |= SDL_RenderClear(Screen.renderer);

    return result == 0 ? 0 : -1;
}

int SetBackgroundColor(int r, int g, int b) {
    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    Screen.Back.r = r;
    Screen.Back.g = g;
    Screen.Back.b = b;
    Screen.Back.a = 0;

    return 0;
}

int ScreenFlip() {
    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    SDL_RenderPresent(Screen.renderer);

    return 0;
}

int SetDrawBlendMode(int mode, int pal) {
    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    Screen.BlendMode = mode;
    Screen.Alpha = pal;
    return 0;
}


/**キーボード系関数**/
int CheckHitKeyAll() {
    for (int i = 0; i < 256; i++)
        if (KeyState[i])
            return i;

    return 0;
}

int CheckHitKey(int KeyCode) {
    if (0 <= KeyCode && KeyCode < 256)
        return KeyState[KeyCode];

    return 0;
}

int GetHitKeyStateAll(char *KeyArray) {
    if (KeyArray == NULL)
        return -1;

    for (int i = 0; i < 256; i++)
        KeyArray[i] = KeyState[i] ? 1 : 0;

    return 0;
}


/**マウス入力系関数**/
int SetMouseDispFlag(int flag) {
    int result = 0;

    if (flag == TRUE)
        result = SDL_ShowCursor(SDL_ENABLE);
    else
        result = SDL_ShowCursor(SDL_DISABLE);

    return result < 0 ? -1 : 0;
}

int GetMousePoint(int *X, int *Y) {
    if (X == NULL || Y == NULL)
        return -1;

    *X = MouseState.X;
    *Y = MouseState.Y;
    return 0;
}

int SetMousePoint(int X, int Y) {
    if (Screen.window == NULL)
        return -1;

    SDL_WarpMouseInWindow(Screen.window, X, Y);
    MouseState.X = X;
    MouseState.Y = Y;

    return 0;
}

int GetMouseInput() {
    int state = 0;

    state |= MouseState.left ? MOUSE_INPUT_LEFT : 0;
    state |= MouseState.middle ? MOUSE_INPUT_MIDDLE : 0;
    state |= MouseState.right ? MOUSE_INPUT_RIGHT : 0;
    state |= MouseState.b4 ? MOUSE_INPUT_4 : 0;
    state |= MouseState.b5 ? MOUSE_INPUT_5 : 0;

    return state;
}

int GetMouseWheelRotVol() {
    int rot;

    rot = MouseState.wheel;
    MouseState.wheel = 0;

    return rot;
}


/**図形描画関数**/
unsigned int GetColor(int r, int g, int b) {
    unsigned int color;
    r <<= 16;
    g <<= 8;
    color = r | g | b;
    return color;
}

int DrawPixel(int x, int y, unsigned int color) {
    unsigned char *c = (unsigned char *) &color;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glBegin(GL_POINT);
    glVertex2i(x, y);
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawLine(int x1, int y1, int x2, int y2, unsigned int color, int thick) {
    unsigned char *c = (unsigned char *) &color;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(thick);
    glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawLineAA(float x1, float y1, float x2, float y2, unsigned int color, float thick) {
    unsigned char *c = (unsigned char *) &color;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(thick);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawBox(int x1, int y1, int x2, int y2, unsigned int color, int fill) {
    unsigned char *c = (unsigned char *) &color;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glLineWidth(1);
    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(1);
    fill ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);
    glVertex2i(x1, y1);
    glVertex2i(x2, y1);
    glVertex2i(x2, y2);
    glVertex2i(x1, y2);
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawBoxAA(float x1, float y1, float x2, float y2, unsigned int color, int fill, float thick) {
    unsigned char *c = (unsigned char *) &color;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(thick);
    fill ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawCircle(int x, int y, int r, unsigned int color, int fill, int thick) {
    unsigned char *c = (unsigned char *) &color;
    double dx, dy;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(thick);
    fill ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        dx = r * cos(i * PI / 180) + x;
        dy = r * sin(i * PI / 180) + y;
        glVertex2d(dx, dy);
    }
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawCircleAA(float x, float y, float r, int posnum, unsigned int color, int fill, float thick) {
    unsigned char *c = (unsigned char *) &color;
    double dx, dy;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(thick);
    fill ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);
    for (int i = 0; i < posnum; i++) {
        dx = r * cos(i * PI / (posnum * 0.5)) + x;
        dy = r * sin(i * PI / (posnum * 0.5)) + y;
        glVertex2d(dx, dy);
    }
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned int color, int fill) {
    unsigned char *c = (unsigned char *) &color;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(1);
    fill ? glBegin(GL_TRIANGLES) : glBegin(GL_LINE_LOOP);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glVertex2i(x3, y3);
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int
DrawTriangleAA(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, int fill, float thick) {
    unsigned char *c = (unsigned char *) &color;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(thick);
    fill ? glBegin(GL_TRIANGLES) : glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawOval(int x, int y, int rx, int ry, unsigned int color, int fill, int thick) {
    unsigned char *c = (unsigned char *) &color;
    double dx, dy;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(thick);
    fill ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        dx = rx * cos(i * PI / 180) + x;
        dy = ry * sin(i * PI / 180) + y;
        glVertex2d(dx, dy);
    }
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawOvalAA(float x, float y, float rx, float ry, int posnum, unsigned int color, int fill, float thick) {
    unsigned char *c = (unsigned char *) &color;
    double dx, dy;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(thick);
    fill ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);
    for (int i = 0; i < posnum; i++) {
        dx = rx * cos(i * PI / (posnum * 0.5)) + x;
        dy = ry * sin(i * PI / (posnum * 0.5)) + y;
        glVertex2d(dx, dy);
    }
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawOval_Rect(int x1, int y1, int x2, int y2, unsigned int color, int fill) {
    unsigned char *c = (unsigned char *) &color;
    double rx, ry, dx, dy;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    rx = (x2 - x1) / 2;
    ry = (y2 - y1) / 2;

    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(1);
    fill ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        dx = rx * cos(i * PI / 180) + (x1 + rx);
        dy = ry * sin(i * PI / 180) + (y1 + ry);
        glVertex2d(dx, dy);
    }
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawRoundRect(int x1, int y1, int x2, int y2, int rx, int ry, unsigned int color, int fill) {
    unsigned char *c = (unsigned char *) &color;
    double dx, dy;
    bool need = true;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(1);
    fill ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);

    for (int i = 0; i < 90; i++) {
        dx = rx * cos(i * PI / 180) + (x2 - rx);
        dy = ry * sin(i * PI / 180) + (y2 - ry);
        if (dx < (x2 - x1) / 2 + x1) {
            need = false;
            break;
        }
        if (dy < (y2 - y1) / 2 + y1)
            continue;
        glVertex2d(dx, dy);
    }

    if (need)
        glVertex2i(x1 + rx, y2);
    need = true;


    for (int i = 90; i < 180; i++) {
        dx = rx * cos(i * PI / 180) + (x1 + rx);
        dy = ry * sin(i * PI / 180) + (y2 - ry);
        if (dx > (x2 - x1) / 2 + x1)
            continue;
        if (dy < (y2 - y1) / 2 + y1) {
            need = false;
            break;
        }
        glVertex2d(dx, dy);
    }

    if (need)
        glVertex2i(x1, y1 + ry);
    need = true;

    for (int i = 180; i < 270; i++) {
        dx = rx * cos(i * PI / 180) + (x1 + rx);
        dy = ry * sin(i * PI / 180) + (y1 + ry);
        if (dx > (x2 - x1) / 2 + x1) {
            need = false;
            break;
        }
        if (dy > (y2 - y1) / 2 + y1)
            continue;
        glVertex2d(dx, dy);
    }

    if (need)
        glVertex2i(x2 - rx, y1);
    need = true;

    for (int i = 270; i < 360; i++) {
        dx = rx * cos(i * PI / 180) + (x2 - rx);
        dy = ry * sin(i * PI / 180) + (y1 + ry);
        if (dx < (x2 - x1) / 2 + x1)
            continue;
        if (dy > (y2 - y1) / 2 + y1) {
            need = false;
            break;
        }
        glVertex2d(dx, dy);
    }

    if (need)
        glVertex2i(x2, y2 - ry);
    need = true;

    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int
DrawRoundRectAA(float x1, float y1, float x2, float y2, float rx, float ry, int posnum, unsigned int color, int fill,
                float thick) {
    unsigned char *c = (unsigned char *) &color;
    double dx, dy;
    bool need = true;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(thick);
    fill ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);

    for (int i = 0; i < 90; i++) {
        dx = rx * cos(i * PI / 180) + (x2 - rx);
        dy = ry * sin(i * PI / 180) + (y2 - ry);
        if (dx < (x2 - x1) / 2 + x1) {
            need = false;
            break;
        }
        if (dy < (y2 - y1) / 2 + y1)
            continue;
        glVertex2d(dx, dy);
    }

    if (need)
        glVertex2f(x1 + rx, y2);
    need = true;

    for (int i = 90; i < 180; i++) {
        dx = rx * cos(i * PI / 180) + (x1 + rx);
        dy = ry * sin(i * PI / 180) + (y2 - ry);
        if (dx > (x2 - x1) / 2 + x1)
            continue;
        if (dy < (y2 - y1) / 2 + y1) {
            need = false;
            break;
        }
        glVertex2d(dx, dy);
    }

    if (need)
        glVertex2f(x1, y1 + ry);
    need = true;

    for (int i = 180; i < 270; i++) {
        dx = rx * cos(i * PI / 180) + (x1 + rx);
        dy = ry * sin(i * PI / 180) + (y1 + ry);
        if (dx > (x2 - x1) / 2 + x1) {
            need = false;
            break;
        }
        if (dy > (y2 - y1) / 2 + y1)
            continue;
        glVertex2d(dx, dy);
    }

    if (need)
        glVertex2f(x2 - rx, y1);
    need = true;

    for (int i = 270; i < 360; i++) {
        dx = rx * cos(i * PI / 180) + (x2 - rx);
        dy = ry * sin(i * PI / 180) + (y1 + ry);
        if (dx < (x2 - x1) / 2 + x1)
            continue;
        if (dy > (y2 - y1) / 2 + y1) {
            need = false;
            break;
        }
        glVertex2d(dx, dy);
    }

    if (need)
        glVertex2f(x2, y2 - ry);
    need = true;

    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawQuadrangle(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int color, int fill) {
    unsigned char *c = (unsigned char *) &color;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(1);
    fill ? glBegin(GL_QUADS) : glBegin(GL_LINE_LOOP);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glVertex2i(x3, y3);
    glVertex2i(x4, y4);
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

int DrawQuadrangleAA(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int color,
                     int fill, float thick) {
    unsigned char *c = (unsigned char *) &color;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    SetBlend2DFigure(c[2], c[1], c[0]);

    glLineWidth(thick);
    fill ? glBegin(GL_QUADS) : glBegin(GL_LINE_LOOP);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glVertex2i(x3, y3);
    glVertex2i(x4, y4);
    glEnd();

    glDisable(GL_BLEND);

    return 0;
}

unsigned int GetPixel(int x, int y) {
    unsigned int pix;
    int width;
    SDL_Rect rect = {x, y, 1, 1};

    SDL_GetWindowSize(Screen.window, &width, NULL);
    SDL_RenderReadPixels(Screen.renderer, &rect, SDL_PIXELFORMAT_ABGR8888, &pix, width * 4);

    return pix & 0x00ffffff;
}


/**画像制御関数**/
int LoadGraph(const char *path) {
    GRAPH_SAVER *temp = new GRAPH_SAVER(IMG_Load(path));

    if (temp->success) {
        GraphDataBase[GraphNumber] = temp;
        return GraphNumber--;
    } else {
        delete temp;
        return -1;
    }
}

int LoadDivGraph(const char *path, int AllNum, int Xnum, int Ynum, int Xsize, int Ysize, int *graphBuf) {
    GRAPH_SAVER *image;
    SDL_Surface *base, *tmp;
    SDL_Rect rect;
    int index = 0;

    base = IMG_Load(path);
    if (base == NULL)
        return -1;

    rect.w = Xsize;
    rect.h = Ysize;

    for (int i = 0; i < Ynum; i++) {
        for (int j = 0; j < Xnum; j++) {
            if (index == AllNum)
                return 0;

            image = new GRAPH_SAVER();
            tmp = SDL_CreateRGBSurface(0, Xsize, Ysize, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
            rect.x = j * Xsize;
            rect.y = i * Ysize;

            SDL_BlitSurface(base, &rect, tmp, NULL);
            image = new GRAPH_SAVER(tmp);
            if (image->success) {
                GraphDataBase[GraphNumber] = image;
                graphBuf[index++] = GraphNumber--;
            } else {
                delete image;
                graphBuf[index++] = -1;
            }
        }
    }

    return 0;
}

int MakeGraph(int w, int h) {
    SDL_Surface *NewGraph = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    GRAPH_SAVER *image = new GRAPH_SAVER(NewGraph);

    if (image->success) {
        GraphDataBase[GraphNumber] = image;
        return GraphNumber--;
    } else {
        delete image;
        return -1;
    }
}

int DerivationGraph(int tx, int ty, int w, int h, int graph) {
    SDL_Surface *NewGraph = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_Rect rect = {tx, ty, w, h};
    GRAPH_SAVER *image;
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_BlitSurface(target->second->surface, &rect, NewGraph, NULL);
    image = new GRAPH_SAVER(NewGraph);

    if (image->success) {
        GraphDataBase[GraphNumber] = image;
        return GraphNumber--;
    } else {
        delete image;
        return -1;
    }
}

int DrawGraph(int x, int y, int graph, int trans) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(x, y);
    glTexCoord2i(1, 0);
    glVertex2i(x + target->second->surface->w, y);
    glTexCoord2i(1, 1);
    glVertex2i(x + target->second->surface->w, y + target->second->surface->h);
    glTexCoord2i(0, 1);
    glVertex2i(x, y + target->second->surface->h);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawGraphF(float x, float y, int graph, int trans) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2f(x, y);
    glTexCoord2i(1, 0);
    glVertex2f(x + target->second->surface->w, y);
    glTexCoord2i(1, 1);
    glVertex2f(x + target->second->surface->w, y + target->second->surface->h);
    glTexCoord2i(0, 1);
    glVertex2f(x, y + target->second->surface->h);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawRotaGraph(int x, int y, double Ex, double angle, int graph, int trans, int turnX, int turnY) {
    SDL_Rect rect;
    SDL_RendererFlip flip;
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glTranslated(x, y, 0);
    glRotated(angle * 180 / PI, 0, 0, 1);
    glTranslated(-x, -y, 0);

    SetBlendGraphics();

    if (turnX && turnY) {
        glBegin(GL_QUADS);
        glTexCoord2i(1, 1);
        glVertex2i(x - Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 1);
        glVertex2i(x + Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 0);
        glVertex2i(x + Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 0);
        glVertex2i(x - Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glEnd();
    } else if (turnX) {
        glBegin(GL_QUADS);
        glTexCoord2i(1, 0);
        glVertex2i(x - Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 0);
        glVertex2i(x + Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 1);
        glVertex2i(x + Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 1);
        glVertex2i(x - Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glEnd();
    } else if (turnY) {
        glBegin(GL_QUADS);
        glTexCoord2i(0, 1);
        glVertex2i(x - Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 1);
        glVertex2i(x + Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 0);
        glVertex2i(x + Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 0);
        glVertex2i(x - Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glEnd();
    } else {
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0);
        glVertex2i(x - Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 0);
        glVertex2i(x + Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 1);
        glVertex2i(x + Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 1);
        glVertex2i(x - Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glEnd();
    }

    glTranslated(x, y, 0);
    glRotated(-angle * 180 / PI, 0, 0, 1);
    glTranslated(-x, -y, 0);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawRotaGraphF(float x, float y, double Ex, double angle, int graph, int trans, int turnX, int turnY) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glTranslated(x, y, 0);
    glRotated(angle * 180 / PI, 0, 0, 1);
    glTranslated(-x, -y, 0);

    SetBlendGraphics();

    if (turnX && turnY) {
        glBegin(GL_QUADS);
        glTexCoord2i(1, 1);
        glVertex2f(x - Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 1);
        glVertex2f(x + Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 0);
        glVertex2f(x + Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 0);
        glVertex2f(x - Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glEnd();
    } else if (turnX) {
        glBegin(GL_QUADS);
        glTexCoord2i(1, 0);
        glVertex2f(x - Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 0);
        glVertex2f(x + Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 1);
        glVertex2f(x + Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 1);
        glVertex2f(x - Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glEnd();
    } else if (turnY) {
        glBegin(GL_QUADS);
        glTexCoord2i(0, 1);
        glVertex2f(x - Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 1);
        glVertex2f(x + Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 0);
        glVertex2f(x + Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 0);
        glVertex2f(x - Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glEnd();
    } else {
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0);
        glVertex2f(x - Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 0);
        glVertex2f(x + Ex * target->second->surface->w / 2, y - Ex * target->second->surface->h / 2);
        glTexCoord2i(1, 1);
        glVertex2f(x + Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glTexCoord2i(0, 1);
        glVertex2f(x - Ex * target->second->surface->w / 2, y + Ex * target->second->surface->h / 2);
        glEnd();
    }

    glTranslated(x, y, 0);
    glRotated(-angle * 180 / PI, 0, 0, 1);
    glTranslated(-x, -y, 0);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawTurnGraph(int x, int y, int graph, int trans) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2i(1, 0);
    glVertex2i(x, y);
    glTexCoord2i(0, 0);
    glVertex2i(x + target->second->surface->w, y);
    glTexCoord2i(0, 1);
    glVertex2i(x + target->second->surface->w, y + target->second->surface->h);
    glTexCoord2i(1, 1);
    glVertex2i(x, y + target->second->surface->h);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawTurnGraphF(float x, float y, int graph, int trans) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2i(1, 0);
    glVertex2f(x, y);
    glTexCoord2i(0, 0);
    glVertex2f(x + target->second->surface->w, y);
    glTexCoord2i(0, 1);
    glVertex2f(x + target->second->surface->w, y + target->second->surface->h);
    glTexCoord2i(1, 1);
    glVertex2f(x, y + target->second->surface->h);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawReverseGraph(int x, int y, int graph, int trans, int turnX, int turnY) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    if (turnX && turnY) {
        glBegin(GL_QUADS);
        glTexCoord2i(1, 1);
        glVertex2i(x, y);
        glTexCoord2i(0, 1);
        glVertex2i(x + target->second->surface->w, y);
        glTexCoord2i(0, 0);
        glVertex2i(x + target->second->surface->w, y + target->second->surface->h);
        glTexCoord2i(1, 0);
        glVertex2i(x, y + target->second->surface->h);
        glEnd();
    } else if (turnX) {
        glBegin(GL_QUADS);
        glTexCoord2i(1, 0);
        glVertex2i(x, y);
        glTexCoord2i(0, 0);
        glVertex2i(x + target->second->surface->w, y);
        glTexCoord2i(0, 1);
        glVertex2i(x + target->second->surface->w, y + target->second->surface->h);
        glTexCoord2i(1, 1);
        glVertex2i(x, y + target->second->surface->h);
        glEnd();
    } else if (turnY) {
        glBegin(GL_QUADS);
        glTexCoord2i(0, 1);
        glVertex2i(x, y);
        glTexCoord2i(1, 1);
        glVertex2i(x + target->second->surface->w, y);
        glTexCoord2i(1, 0);
        glVertex2i(x + target->second->surface->w, y + target->second->surface->h);
        glTexCoord2i(0, 0);
        glVertex2i(x, y + target->second->surface->h);
        glEnd();
    } else {
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0);
        glVertex2i(x, y);
        glTexCoord2i(1, 0);
        glVertex2i(x + target->second->surface->w, y);
        glTexCoord2i(1, 1);
        glVertex2i(x + target->second->surface->w, y + target->second->surface->h);
        glTexCoord2i(0, 1);
        glVertex2i(x, y + target->second->surface->h);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawReverseGraphF(float x, float y, int graph, int trans, int turnX, int turnY) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    if (turnX && turnY) {
        glBegin(GL_QUADS);
        glTexCoord2i(1, 1);
        glVertex2f(x, y);
        glTexCoord2i(0, 1);
        glVertex2f(x + target->second->surface->w, y);
        glTexCoord2i(0, 0);
        glVertex2f(x + target->second->surface->w, y + target->second->surface->h);
        glTexCoord2i(1, 0);
        glVertex2f(x, y + target->second->surface->h);
        glEnd();
    } else if (turnX) {
        glBegin(GL_QUADS);
        glTexCoord2i(1, 0);
        glVertex2f(x, y);
        glTexCoord2i(0, 0);
        glVertex2f(x + target->second->surface->w, y);
        glTexCoord2i(0, 1);
        glVertex2f(x + target->second->surface->w, y + target->second->surface->h);
        glTexCoord2i(1, 1);
        glVertex2f(x, y + target->second->surface->h);
        glEnd();
    } else if (turnY) {
        glBegin(GL_QUADS);
        glTexCoord2i(0, 1);
        glVertex2f(x, y);
        glTexCoord2i(1, 1);
        glVertex2f(x + target->second->surface->w, y);
        glTexCoord2i(1, 0);
        glVertex2f(x + target->second->surface->w, y + target->second->surface->h);
        glTexCoord2i(0, 0);
        glVertex2f(x, y + target->second->surface->h);
        glEnd();
    } else {
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0);
        glVertex2f(x, y);
        glTexCoord2i(1, 0);
        glVertex2f(x + target->second->surface->w, y);
        glTexCoord2i(1, 1);
        glVertex2f(x + target->second->surface->w, y + target->second->surface->h);
        glTexCoord2i(0, 1);
        glVertex2f(x, y + target->second->surface->h);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawExtendGraph(int x1, int y1, int x2, int y2, int graph, int trans) {
    SDL_Rect rect;
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(x1, y1);
    glTexCoord2i(1, 0);
    glVertex2i(x2, y1);
    glTexCoord2i(1, 1);
    glVertex2i(x2, y2);
    glTexCoord2i(0, 1);
    glVertex2i(x1, y2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    /*rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;

    SDL_RenderCopy(Screen.renderer, target->second->texture, NULL, &rect);*/

    return 0;
}

int DrawExtendGraphF(float x1, float y1, float x2, float y2, int graph, int trans) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2f(x1, y1);
    glTexCoord2i(1, 0);
    glVertex2f(x2, y1);
    glTexCoord2i(1, 1);
    glVertex2f(x2, y2);
    glTexCoord2i(0, 1);
    glVertex2f(x1, y2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawModiGraph(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int graph, int trans) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(x1, y1);
    glTexCoord2i(1, 0);
    glVertex2i(x2, y2);
    glTexCoord2i(1, 1);
    glVertex2i(x4, y4);
    glTexCoord2i(0, 1);
    glVertex2i(x3, y3);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int
DrawModiGraphF(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int graph, int trans) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2f(x1, y1);
    glTexCoord2i(1, 0);
    glVertex2f(x2, y2);
    glTexCoord2i(1, 1);
    glVertex2f(x4, y4);
    glTexCoord2i(0, 1);
    glVertex2f(x3, y3);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawRectGraph(int x, int y, int tx, int ty, int w, int h, int graph, int trans, int turnX, int turnY) {
    double gw, gh;
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    gw = target->second->surface->w;
    gh = target->second->surface->h;

    w = w > target->second->surface->w ? target->second->surface->w : w;
    h = h > target->second->surface->h ? target->second->surface->h : h;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    if (turnX && turnY) {
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2i(x, y);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2i(x + w, y);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2i(x + w, y + h);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2i(x, y + h);
    } else if (turnX) {
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2i(x, y);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2i(x + w, y);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2i(x + w, y + h);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2i(x, y + h);
    } else if (turnY) {
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2i(x, y);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2i(x + w, y);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2i(x + w, y + h);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2i(x, y + h);
    } else {
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2i(x, y);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2i(x + w, y);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2i(x + w, y + h);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2i(x, y + h);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawRectGraphF(float x, float y, int tx, int ty, int w, int h, int graph, int trans, int turnX, int turnY) {
    double gw, gh;
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    gw = target->second->surface->w;
    gh = target->second->surface->h;

    w = w > target->second->surface->w ? target->second->surface->w : w;
    h = h > target->second->surface->h ? target->second->surface->h : h;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    if (turnX && turnY) {
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2f(x, y);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2f(x + w, y);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2f(x + w, y + h);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2f(x, y + h);
    } else if (turnX) {
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2f(x, y);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2f(x + w, y);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2f(x + w, y + h);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2f(x, y + h);
    } else if (turnY) {
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2f(x, y);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2f(x + w, y);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2f(x + w, y + h);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2f(x, y + h);
    } else {
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2f(x, y);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2f(x + w, y);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2f(x + w, y + h);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2f(x, y + h);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int
DrawRectRotaGraph(int x, int y, int tx, int ty, int w, int h, double Ex, double angle, int graph, int trans, int turnX,
                  int turnY) {
    map<int, GRAPH_SAVER *>::iterator target;
    double gw, gh;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    gw = target->second->surface->w;
    gh = target->second->surface->h;

    w = w > target->second->surface->w ? target->second->surface->w : w;
    h = h > target->second->surface->h ? target->second->surface->h : h;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glTranslated(x, y, 0);
    glRotated(angle * 180 / PI, 0, 0, 1);
    glTranslated(-x, -y, 0);

    glBegin(GL_QUADS);
    if (turnX && turnY) {
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2i(x - w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2i(x + w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2i(x + w * Ex / 2, y + h * Ex / 2);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2i(x - w * Ex / 2, y + h * Ex / 2);
    } else if (turnX) {
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2i(x - w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2i(x + w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2i(x + w * Ex / 2, y + h * Ex / 2);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2i(x - w * Ex / 2, y + h * Ex / 2);
    } else if (turnY) {
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2i(x - w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2i(x + w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2i(x + w * Ex / 2, y + h * Ex / 2);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2i(x - w * Ex / 2, y + h * Ex / 2);
    } else {
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2i(x - w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2i(x + w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2i(x + w * Ex / 2, y + h * Ex / 2);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2i(x - w * Ex / 2, y + h * Ex / 2);
    }
    glEnd();

    glTranslated(x, y, 0);
    glRotated(-angle * 180 / PI, 0, 0, 1);
    glTranslated(-x, -y, 0);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawRectRotaGraphF(float x, float y, int tx, int ty, int w, int h, double Ex, double angle, int graph, int trans,
                       int turnX, int turnY) {
    double gw, gh;
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    gw = target->second->surface->w;
    gh = target->second->surface->h;

    w = w > target->second->surface->w ? target->second->surface->w : w;
    h = h > target->second->surface->h ? target->second->surface->h : h;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glTranslated(x, y, 0);
    glRotated(angle * 180 / PI, 0, 0, 1);
    glTranslated(-x, -y, 0);

    glBegin(GL_QUADS);
    if (turnX && turnY) {
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2f(x - w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2f(x + w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2f(x + w * Ex / 2, y + h * Ex / 2);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2f(x - w * Ex / 2, y + h * Ex / 2);
    } else if (turnX) {
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2f(x - w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2f(x + w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2f(x + w * Ex / 2, y + h * Ex / 2);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2f(x - w * Ex / 2, y + h * Ex / 2);
    } else if (turnY) {
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2f(x - w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2f(x + w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2f(x + w * Ex / 2, y + h * Ex / 2);
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2f(x - w * Ex / 2, y + h * Ex / 2);
    } else {
        glTexCoord2f(tx / gw, ty / gh);
        glVertex2f(x - w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f((tx + w) / gw, ty / gh);
        glVertex2f(x + w * Ex / 2, y - h * Ex / 2);
        glTexCoord2f((tx + w) / gw, (ty + h) / gh);
        glVertex2f(x + w * Ex / 2, y + h * Ex / 2);
        glTexCoord2f(tx / gw, (ty + h) / gh);
        glVertex2f(x - w * Ex / 2, y + h * Ex / 2);
    }
    glEnd();

    glTranslated(x, y, 0);
    glRotated(-angle * 180 / PI, 0, 0, 1);
    glTranslated(-x, -y, 0);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawRectExtendGraph(int x1, int y1, int x2, int y2, int tx, int ty, int w, int h, int graph, int trans) {
    double gw, gh;
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    gw = target->second->surface->w;
    gh = target->second->surface->h;

    w = w > target->second->surface->w ? target->second->surface->w : w;
    h = h > target->second->surface->h ? target->second->surface->h : h;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2f(tx / gw, ty / gh);
    glVertex2i(x1, y1);
    glTexCoord2f((tx + w) / gw, ty / gh);
    glVertex2i(x2, y1);
    glTexCoord2f((tx + w) / gw, (ty + h) / gh);
    glVertex2i(x2, y2);
    glTexCoord2f(tx / gw, (ty + h) / gh);
    glVertex2i(x1, y2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    SDL_GL_UnbindTexture(target->second->texture);

    return 0;
}

int DrawRectExtendGraphF(float x1, float y1, float x2, float y2, int tx, int ty, int w, int h, int graph, int trans) {
    double gw, gh;
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    gw = target->second->surface->w;
    gh = target->second->surface->h;

    w = w > target->second->surface->w ? target->second->surface->w : w;
    h = h > target->second->surface->h ? target->second->surface->h : h;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2f(tx / gw, ty / gh);
    glVertex2f(x1, y1);
    glTexCoord2f((tx + w) / gw, ty / gh);
    glVertex2f(x2, y1);
    glTexCoord2f((tx + w) / gw, (ty + h) / gh);
    glVertex2f(x2, y2);
    glTexCoord2f(tx / gw, (ty + h) / gh);
    glVertex2f(x1, y2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawRectModiGraph(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int tx, int ty, int w, int h,
                      int graph, int trans) {
    double gw, gh;
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    gw = target->second->surface->w;
    gh = target->second->surface->h;

    w = w > target->second->surface->w ? target->second->surface->w : w;
    h = h > target->second->surface->h ? target->second->surface->h : h;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2f(tx / gw, ty / gh);
    glVertex2i(x1, y1);
    glTexCoord2f((tx + w) / gw, ty / gh);
    glVertex2i(x2, y2);
    glTexCoord2f((tx + w) / gw, (ty + h) / gh);
    glVertex2i(x4, y4);
    glTexCoord2f(tx / gw, (ty + h) / gh);
    glVertex2i(x3, y3);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DrawRectModiGraphF(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int tx, int ty,
                       int w, int h, int graph, int trans) {
    double gw, gh;
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    gw = target->second->surface->w;
    gh = target->second->surface->h;

    w = w > target->second->surface->w ? target->second->surface->w : w;
    h = h > target->second->surface->h ? target->second->surface->h : h;

    SDL_GL_BindTexture(target->second->texture, NULL, NULL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    SetBlendGraphics();

    glBegin(GL_QUADS);
    glTexCoord2f(tx / gw, ty / gh);
    glVertex2f(x1, y1);
    glTexCoord2f((tx + w) / gw, ty / gh);
    glVertex2f(x2, y2);
    glTexCoord2f((tx + w) / gw, (ty + h) / gh);
    glVertex2f(x4, y4);
    glTexCoord2f(tx / gw, (ty + h) / gh);
    glVertex2f(x3, y3);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

int DeleteGraph(int graph) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    delete target->second;
    return GraphDataBase.erase(graph) == 0 ? -1 : 0;
}

int InitGraph() {
    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    for (map<int, GRAPH_SAVER *>::iterator i = GraphDataBase.begin(); i != GraphDataBase.end(); i++)
        delete i->second;

    GraphDataBase.clear();
    GraphNumber = 0xffff - 1;
    return 0;
}

int GetGraphSize(int graph, int *width, int *height) {
    map<int, GRAPH_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = GraphDataBase.find(graph)) == GraphDataBase.end())
        return -1;
    if (target->second->surface == NULL || target->second->texture == NULL)
        return -1;

    if (width != NULL)
        *width = target->second->surface->w;
    if (height != NULL)
        *height = target->second->surface->h;

    return 0;
}

int SaveDrawScreen(int x1, int y1, int x2, int y2, const char *name) {
    SDL_Rect rect;
    SDL_Surface *sshot = SDL_CreateRGBSurface(0, x2 - x1, y2 - y1, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);
    int flag = 0;

    if (sshot == NULL)
        return -1;

    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;

    flag |= SDL_RenderReadPixels(Screen.renderer, &rect, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
    flag |= SDL_SaveBMP(sshot, name);

    SDL_FreeSurface(sshot);

    return flag == 0 ? 0 : -1;
}

int SaveDrawScreenToBMP(int x1, int y1, int x2, int y2, const char *name) {
    SDL_Rect rect;
    SDL_Surface *sshot = SDL_CreateRGBSurface(0, x2 - x1, y2 - y1, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);
    int flag = 0;

    if (sshot == NULL)
        return -1;

    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;

    flag |= SDL_RenderReadPixels(Screen.renderer, &rect, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
    flag |= SDL_SaveBMP(sshot, name);

    SDL_FreeSurface(sshot);

    return flag == 0 ? 0 : -1;
}

int SaveDrawScreenToJPEG(int x1, int y1, int x2, int y2, const char *name, int quality) {
    SDL_Rect rect;
    SDL_Surface *sshot = SDL_CreateRGBSurface(0, x2 - x1, y2 - y1, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);
    int flag = 0;

    if (sshot == NULL)
        return -1;

    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;

    flag |= SDL_RenderReadPixels(Screen.renderer, &rect, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
    flag |= IMG_SaveJPG(sshot, name, quality);

    SDL_FreeSurface(sshot);

    return flag == 0 ? 0 : -1;
}

int SaveDrawScreenToPNG(int x1, int y1, int x2, int y2, const char *name) {
    SDL_Rect rect;
    SDL_Surface *sshot = SDL_CreateRGBSurface(0, x2 - x1, y2 - y1, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    int flag = 0;

    if (sshot == NULL)
        return -1;

    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;

    flag |= SDL_RenderReadPixels(Screen.renderer, &rect, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);

    flag |= IMG_SavePNG(sshot, name);
    SDL_FreeSurface(sshot);

    return flag == 0 ? 0 : -1;
}


/**音声制御関数**/
int InitSoundMem() {
    for (map<int, SOUND_SAVER *>::iterator i = SoundDataBase.begin(); i != SoundDataBase.end(); i++)
        delete i->second;

    SoundDataBase.clear();
    SoundNumber = 0xffff - 1;
    return 0;
}

int LoadSoundMem(const char *path) {
    SOUND_SAVER *sound = new SOUND_SAVER(path, SoundNumber);

    if (sound->success) {
        SoundDataBase[SoundNumber] = sound;
        return SoundNumber--;
    } else {
        delete sound;
        return -1;
    }
}

int PlaySoundMem(int sound, int type, int TopPosFlag) {
    int result;
    map<int, SOUND_SAVER *>::iterator target;
    if ((target = SoundDataBase.find(sound)) == SoundDataBase.end())
        return -1;
    if (target->second->chunk == NULL)
        return -1;

    if (type == DX_PLAYTYPE_BACK) {
        if (TopPosFlag)
            result = Mix_PlayChannel(SoundDataBase[sound]->channel, SoundDataBase[sound]->chunk, 0);
        else if (Mix_Paused(SoundDataBase[sound]->channel)) {
            Mix_Resume(SoundDataBase[sound]->channel);
            result = 0;
        } else
            result = -1;
    } else if (type == DX_PLAYTYPE_LOOP) {
        if (TopPosFlag)
            result = Mix_PlayChannel(SoundDataBase[sound]->channel, SoundDataBase[sound]->chunk, -1);
        else if (Mix_Paused(SoundDataBase[sound]->channel)) {
            Mix_Resume(SoundDataBase[sound]->channel);
            result = 0;
        } else
            result = -1;
    } else {
        result = Mix_PlayChannel(SoundDataBase[sound]->channel, SoundDataBase[sound]->chunk, 0);

        while (result != -1) {
            if (ProcessMessage() != 0) {
                DxLib_End();
                exit(0);
            }
            if (Mix_Playing(SoundDataBase[sound]->channel) == 0)
                break;
        }
    }

    return result == -1 ? -1 : 0;
}

int CheckSoundMem(int sound) {
    map<int, SOUND_SAVER *>::iterator target;
    if ((target = SoundDataBase.find(sound)) == SoundDataBase.end())
        return -1;
    if (target->second->chunk == NULL)
        return -1;

    return Mix_Playing(target->second->channel);
}

int StopSoundMem(int sound) {
    map<int, SOUND_SAVER *>::iterator target;
    if ((target = SoundDataBase.find(sound)) == SoundDataBase.end())
        return -1;
    if (target->second->chunk == NULL)
        return -1;

    Mix_Pause(SoundDataBase[sound]->channel);
    return 0;
}

int ChangeVolumeSoundMem(int volume, int sound) {
    map<int, SOUND_SAVER *>::iterator target;
    if ((target = SoundDataBase.find(sound)) == SoundDataBase.end())
        return -1;
    if (target->second->chunk == NULL)
        return -1;

    if (volume < 0)
        volume = 0;
    else if (volume > 255)
        volume = 255;

    Mix_Volume(SoundDataBase[sound]->channel, volume & 1 ? volume / 2 + 1 : volume / 2);
    return 0;
}

int DeleteSoundMem(int sound) {
    map<int, SOUND_SAVER *>::iterator target;
    if ((target = SoundDataBase.find(sound)) == SoundDataBase.end())
        return -1;
    if (target->second->chunk == NULL)
        return -1;

    delete target->second;
    return SoundDataBase.erase(sound) == 0 ? -1 : 0;
}


/**共通使用関数**/
int GetNowCount() {
    return (int) SDL_GetTicks();
}

int GetRand(int N) {
    return rand() % (abs(N) + 1);
}

/**文字列描画関数**/
int ChangeFont(const char *fontName) {
    if (DefaultFont == NULL)
        return -1;

    FONT_SAVER *tmp = DefaultFont;
    DefaultFont = new FONT_SAVER(fontName, tmp->SIZE, tmp->AA, tmp->it);

    if (!DefaultFont->success) {
        delete DefaultFont;
        DefaultFont = tmp;
        return -1;
    }

    delete tmp;
    return 0;
}

int SetFontSize(int size) {
    if (DefaultFont == NULL)
        return -1;

    FONT_SAVER *tmp = DefaultFont;
    DefaultFont = new FONT_SAVER(tmp->fontPath, size, tmp->AA, tmp->it);

    if (!DefaultFont->success) {
        delete DefaultFont;
        DefaultFont = tmp;
        return -1;
    }

    delete tmp;
    return 0;
}

int SetAAstate(bool state) {
    if (DefaultFont == NULL)
        return -1;

    DefaultFont->AA = state;
    return 0;
}

int DrawString(int x, int y, const char *Str, Uint32 color) {
    SDL_Surface *text;
    Uint32 *data;
    Uint32 p = 0;
    Uint8 ca;
    Uint8 *c = (Uint8 *) &color;
    SDL_Color FontColor = {c[2], c[1], c[0], 255};

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if (DefaultFont == NULL)
        return -1;
    if (DefaultFont->font == NULL)
        return -1;

    if (DefaultFont->AA)
        text = TTF_RenderUTF8_Blended(DefaultFont->font, Str, FontColor);
    else
        text = TTF_RenderUTF8_Solid(DefaultFont->font, Str, FontColor);

    if (text == NULL)
        return -1;

    data = (Uint32 *) text->pixels;

    for (int i = 0; i < text->h; i++) {
        for (int j = 0; j < text->w; j++) {
            ca = data[p] >> 24;
            if (ca > 0) {
                SDL_SetRenderDrawColor(Screen.renderer, FontColor.r, FontColor.g, FontColor.b, ca);
                SDL_RenderDrawPoint(Screen.renderer, x + j, y + i);
            }
            p++;
        }
    }

    SDL_FreeSurface(text);
    return 0;
}

int DrawFormatString(int x, int y, Uint32 color, const char *Str, ...) {
    SDL_Surface *text = NULL;
    char *result;
    Uint32 *data;
    Uint32 p = 0;
    Uint8 ca;
    Uint8 *c = (Uint8 *) &color;
    va_list ArgPoint;
    SDL_Color FontColor = {c[2], c[1], c[0], 255};

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if (DefaultFont == NULL)
        return -1;
    if (DefaultFont->font == NULL)
        return -1;

    va_start(ArgPoint, Str);
    result = (char *) malloc(sizeof(char) * (vfprintf(tmpfp, Str, ArgPoint) + 16));
    va_end(ArgPoint);

    va_start(ArgPoint, Str);
    vsprintf(result, Str, ArgPoint);
    va_end(ArgPoint);

    if (DefaultFont->AA)
        text = TTF_RenderUTF8_Blended(DefaultFont->font, result, FontColor);
    else
        text = TTF_RenderUTF8_Solid(DefaultFont->font, result, FontColor);

    if (text == NULL)
        return -1;

    data = (Uint32 *) text->pixels;
    for (int i = 0; i < text->h; i++) {
        for (int j = 0; j < text->w; j++) {
            ca = data[p] >> 24;
            if (ca > 0) {
                SDL_SetRenderDrawColor(Screen.renderer, FontColor.r, FontColor.g, FontColor.b, ca);
                SDL_RenderDrawPoint(Screen.renderer, x + j, y + i);
            }
            p++;
        }
    }

    SDL_FreeSurface(text);
    free(result);
    return 0;
}

int CreateFontToHandle(const char *fontName, int size, bool alias, bool italic) {
    FONT_SAVER *font = new FONT_SAVER(fontName, size, alias, italic);

    if (font->success) {
        FontDataBase[FontNumber] = font;
        return FontNumber--;
    } else {
        delete font;
        return -1;
    }
}

int DeleteFontToHandle(int font) {
    map<int, FONT_SAVER *>::iterator target;
    if ((target = FontDataBase.find(font)) == FontDataBase.end())
        return -1;

    delete target->second;
    return FontDataBase.erase(font) == 0 ? -1 : 0;
}

int DrawStringToHandle(int x, int y, const char *Str, Uint32 color, int FontHandle) {
    SDL_Surface *text;
    Uint32 *data;
    Uint32 p = 0;
    Uint8 ca;
    Uint8 *c = (Uint8 *) &color;
    SDL_Color FontColor = {c[2], c[1], c[0], 255};
    map<int, FONT_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;

    if ((target = FontDataBase.find(FontHandle)) == FontDataBase.end())
        return -1;
    if (target->second->font == NULL)
        return -1;

    if (target->second->AA)
        text = TTF_RenderUTF8_Blended(target->second->font, Str, FontColor);
    else
        text = TTF_RenderUTF8_Solid(target->second->font, Str, FontColor);

    if (text == NULL)
        return -1;

    data = (Uint32 *) text->pixels;

    for (int i = 0; i < text->h; i++) {
        for (int j = 0; j < text->w; j++) {
            ca = data[p] >> 24;
            if (ca > 0) {
                SDL_SetRenderDrawColor(Screen.renderer, FontColor.r, FontColor.g, FontColor.b, ca);
                SDL_RenderDrawPoint(Screen.renderer, x + j, y + i);
            }
            p++;
        }
    }

    SDL_FreeSurface(text);
    return 0;
}

int DrawFormatStringToHandle(int x, int y, Uint32 color, int FontHandle, const char *Str, ...) {
    SDL_Surface *text = NULL;
    char *result;
    Uint32 *data;
    Uint32 p = 0;
    Uint8 ca;
    Uint8 *c = (Uint8 *) &color;
    SDL_Color FontColor = {c[2], c[1], c[0], 255};
    va_list ArgPoint;
    map<int, FONT_SAVER *>::iterator target;

    if (Screen.window == NULL || Screen.renderer == NULL)
        return -1;
    if ((target = FontDataBase.find(FontHandle)) == FontDataBase.end())
        return -1;
    if (target->second->font == NULL)
        return -1;

    va_start(ArgPoint, Str);
    result = (char *) malloc(sizeof(char) * (vfprintf(tmpfp, Str, ArgPoint) + 16));
    va_end(ArgPoint);

    va_start(ArgPoint, Str);
    vsprintf(result, Str, ArgPoint);
    va_end(ArgPoint);

    if (target->second->AA)
        text = TTF_RenderUTF8_Blended(target->second->font, result, FontColor);
    else
        text = TTF_RenderUTF8_Solid(target->second->font, result, FontColor);

    if (text == NULL)
        return -1;

    data = (Uint32 *) text->pixels;
    for (int i = 0; i < text->h; i++) {
        for (int j = 0; j < text->w; j++) {
            ca = data[p] >> 24;
            if (ca > 0) {
                SDL_SetRenderDrawColor(Screen.renderer, FontColor.r, FontColor.g, FontColor.b, ca);
                SDL_RenderDrawPoint(Screen.renderer, x + j, y + i);
            }
            p++;
        }
    }

    SDL_FreeSurface(text);
    free(result);
    return 0;
    return 0;
}

int InitFontToHandle() {
    for (map<int, FONT_SAVER *>::iterator i = FontDataBase.begin(); i != FontDataBase.end(); i++)
        delete i->second;

    FontDataBase.clear();
    FontNumber = 0xffff - 1;
    return 0;
}


