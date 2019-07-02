//
// Created by KohsukeTanaka on 2019-06-28.
//

#ifndef INC_2019_MINGE_SCENEMGR_H
#define INC_2019_MINGE_SCENEMGR_H

typedef enum{
    eScene_Menu,
    eScene_Shoot,

    eScene_None,
}eScene;

class Task{
public:
    virtual ~Task() = default;
    virtual void Initialize(){};
    virtual void Finalize(){};
    virtual void Update() = 0;
    virtual void Draw() = 0;
};

class ISceneChanger{
public:
    virtual ~ISceneChanger() = default;
    virtual void ChangeScene(eScene NextScene)= 0;
};

class BaseScene : public Task{
protected:
    ISceneChanger *mSceneChanger;
public:
    explicit BaseScene(ISceneChanger* changer):mSceneChanger(changer){};
    ~BaseScene()override = default;
    void Initialize() override {};
    void Finalize() override {};
    void Update() override = 0;
    void Draw() override = 0;

};

class SceneMgr:public Task,ISceneChanger{
protected:
    BaseScene *mScene;
    eScene mNextScene;
public:
    SceneMgr();
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;

    void ChangeScene(eScene NextScene) override;
};

#endif //INC_2019_MINGE_SCENEMGR_H
