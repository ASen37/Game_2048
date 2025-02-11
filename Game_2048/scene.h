#pragma once

#include <graphics.h>
#include "camera.h"

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	virtual void on_input(const ExMessage& mg) {} //玩家输入
	virtual void on_update(int delta) {}					  //处理输入
	virtual void on_draw(const Camera& camera) {}					  //渲染画面
	virtual void on_enter() {}					  //进入场景时初始化
	virtual void on_exit() {}					  //退出场景逻辑


private:

};





