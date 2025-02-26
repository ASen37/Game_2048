#pragma once

#include<iostream>
#include"atlas.h"
#include"animation.h"
#include"camera.h"
#include"timer.h"
#include "scene.h"
#include"SceneManager.h"

extern IMAGE img_menu_background;
extern SceneManager scene_manager;

extern int is_need_switch_keyboard;
class MenuScene : public Scene
{
public:
	MenuScene() = default;
	~MenuScene() = default;

	void on_enter() {
		mciSendString(_T("play bgm_menu repeat from 0"), NULL, 0, NULL);
	}

	void on_exit() {
		// 除了第一次进入场景需要切换键盘外，其他在程序运行时再次进入都不再需要切换键盘
		if(is_need_switch_keyboard == 0)
			switch_keyboard();
		is_need_switch_keyboard++;
	}

	void on_input(const ExMessage& msg){
		if (msg.message == WM_KEYUP)
		{
			if (msg.vkcode != VK_ESCAPE)
			{
				mciSendString(_T("play ui_confirm from 0"), NULL, 0, NULL);
				scene_manager.switch_to(SceneManager::SceneType::Selector);
			}
			else
			{
				exit(0);
			}
		}
	}

	void on_update(int delta){ }

	void on_draw(const Camera& camera){
		putimage(0, 0, &img_menu_background);
	}

private:
	
};