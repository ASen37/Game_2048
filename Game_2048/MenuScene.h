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

class MenuScene : public Scene
{
public:
	MenuScene() = default;
	~MenuScene() = default;

	void on_enter() {
		mciSendString(_T("play bgm_menu repeat from 0"), NULL, 0, NULL);
	}

	void on_exit() {
		mciSendString(_T("stop bgm_menu"), NULL, 0, NULL);

		// 模拟按键CTRL + SPACE 切换键盘到英文模式
		keybd_event(VK_CONTROL, 0, 0, 0);
		keybd_event(VK_SPACE, 0, 0, 0);
		keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
		keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
	}

	void on_input(const ExMessage& msg){
		if (msg.message == WM_KEYUP)
		{
			mciSendString(_T("play ui_confirm from 0"), NULL, 0, NULL);
			scene_manager.switch_to(SceneManager::SceneType::Selector);
		}
	}

	void on_update(int delta){
		
	}

	void on_draw(const Camera& camera){
		putimage(0, 0, &img_menu_background);
	}

private:
	
};