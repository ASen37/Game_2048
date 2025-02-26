#pragma once

#include"SceneManager.h"
#include"util.h"
#include"camera.h"
#include"scene.h"
#include"platform.h"
#include"player.h"
#include"status_bar.h"


extern IMAGE img_sky; // 天空图片
extern IMAGE img_hills; // 山脉图片
extern IMAGE img_platform_large; //大型平台图片
extern IMAGE img_platform_small; //小型平台图片

extern Camera main_camera; // 主摄像机
extern SceneManager scene_manager;
extern Scene* menu_scene;
extern std::vector<Platform> platform_list;

extern Player* player_1;
extern Player* player_2;

extern IMAGE img_1P_winner;
extern IMAGE img_2P_winner;
extern IMAGE img_winner_bar;

extern IMAGE* img_player_1_avatar;
extern IMAGE* img_player_2_avatar;

class GameScene : public Scene
{
public:
	GameScene() = default;
	~GameScene() = default;
	void on_enter() {
		is_game_over = false;
		is_slide_out_start = false;
		// 初始化获胜图片及文本，并在屏幕居中
		pos_img_winner_bar.x = -img_winner_bar.getwidth();
		pos_img_winner_bar.y = (getheight() - img_winner_bar.getheight()) / 2;
		pos_x_img_winner_bar_dst = (getwidth() - img_winner_bar.getwidth()) / 2;

		pos_img_winner_text.x = pos_img_winner_bar.x;
		pos_img_winner_text.y = (getheight() - img_1P_winner.getheight()) / 2;
		pos_x_img_winner_text_dst = (getwidth() - img_1P_winner.getwidth()) / 2;

		//* 初始化定时器，依次为：
		//* 胜利图片滑入定时器、胜利图片滑出定时器
		timer_winner_slide_in.restart();
		timer_winner_slide_in.set_wait_time(2500);
		timer_winner_slide_in.set_one_shot(true);
		timer_winner_slide_in.set_callback([&]()
			{
				is_slide_out_start = true;
			});

		timer_winner_slide_out.restart();
		timer_winner_slide_out.set_wait_time(1000);
		timer_winner_slide_out.set_one_shot(true);
		timer_winner_slide_out.set_callback([&]()
			{
				scene_manager.switch_to(SceneManager::SceneType::Menue);
			});

		// 设置状态框玩家头像（图片）、位置
		status_bar_1P.set_avatar(img_player_1_avatar);
		status_bar_2P.set_avatar(img_player_2_avatar);
		status_bar_1P.set_position(235, 625);
		status_bar_2P.set_position(675, 625);

		player_1->set_position(200, 50);
		player_2->set_position(975, 50);

		pos_img_sky.x = (getwidth() - img_sky.getwidth()) / 2;
		pos_img_sky.y = (getheight() - img_sky.getheight()) / 2;

		pos_img_hills.x = (getwidth() - img_hills.getwidth()) / 2;
		pos_img_hills.y = (getheight() - img_hills.getheight()) / 2;

		// 初始化平台
		platform_list.resize(4);
		Platform& larg_platform = platform_list[0];
		larg_platform.img = &img_platform_large;
		larg_platform.render_position.x = 122;
		larg_platform.render_position.y = 455;
		larg_platform.shape.left = (float)larg_platform.render_position.x + 30;
		larg_platform.shape.right = (float)larg_platform.render_position.x + img_platform_large.getwidth() - 30;
		larg_platform.shape.y = (float)larg_platform.render_position.y + 60;

		Platform& small_platform = platform_list[1];
		small_platform.img = &img_platform_small;
		small_platform.render_position.x = 175;
		small_platform.render_position.y = 360;
		small_platform.shape.left = (float)small_platform.render_position.x + 40;
		small_platform.shape.right = (float)small_platform.render_position.x + img_platform_small.getwidth() - 40;
		small_platform.shape.y = (float)small_platform.render_position.y + img_platform_small.getheight() / 2;

		Platform& small_platform2 = platform_list[2];
		small_platform2.img = &img_platform_small;
		small_platform2.render_position.x = 855;
		small_platform2.render_position.y = 360;
		small_platform2.shape.left = (float)small_platform2.render_position.x + 40;
		small_platform2.shape.right = (float)small_platform2.render_position.x + img_platform_small.getwidth() - 40;
		small_platform2.shape.y = (float)small_platform2.render_position.y + img_platform_small.getheight() / 2;

		Platform& small_platform3 = platform_list[3];
		small_platform3.img = &img_platform_small;
		small_platform3.render_position.x = 515;
		small_platform3.render_position.y = 235;
		small_platform3.shape.left = (float)small_platform3.render_position.x + 40;
		small_platform3.shape.right = (float)small_platform3.render_position.x + img_platform_small.getwidth() - 40;
		small_platform3.shape.y = (float)small_platform3.render_position.y + img_platform_small.getheight() / 2;
		mciSendString(_T("play bgm_game repeat from 0"), NULL, 0, NULL);
	}
	
	void on_exit() {
		// 释放内存，指针置空（避免野指针）
		delete player_1;
		delete player_2;
		player_1 = nullptr;
		player_2 = nullptr;
		is_debug = false;
		// 清空资源列表，主摄像机重置
		bullet_list.clear();
		platform_list.clear();
		main_camera.reset();
	}

	void on_input(const ExMessage& msg) {
		player_1->on_input(msg);
		player_2->on_input(msg);

		switch (msg.message)
		{
		case WM_KEYUP:
			if (msg.vkcode == 0x51) // Q 键调试
			{
				is_debug = !is_debug;
			}
			else if(msg.vkcode == VK_ESCAPE) // ESC 键返回
			{
				mciSendString(_T("stop bgm_game"), NULL, 0, NULL);
				menu_scene->on_enter();
				scene_manager.switch_to(SceneManager::SceneType::Selector);
			}
			break;
		default:
			break;
		}
	}

	void on_update(int delta) {
		player_1->on_update(delta);
		player_2->on_update(delta);

		main_camera.on_update(delta);

		bullet_list.erase(std::remove_if(bullet_list.begin(), bullet_list.end(), 
			[](const Bullet* bullet) 
			{
				bool deletable = bullet->get_can_remove();
				if(deletable) delete bullet;
				return deletable;
			}),
			bullet_list.end());

		for(Bullet* bullet : bullet_list)
			bullet->on_update(delta);

		const Vector2& player_1_pos = player_1->get_position();
		const Vector2& player_2_pos = player_2->get_position();
		if(player_1_pos.y >= getheight())
			player_1->set_hp(0);
		if(player_2_pos.y >= getheight())
			player_2->set_hp(0);

		if (player_1->get_hp() <= 0 || player_2->get_hp() <= 0)
		{
			if (!is_game_over)
			{
				mciSendString(_T("stop bgm_game"), NULL, 0, NULL);
				mciSendString(_T("play ui_win from 0"), NULL, 0, NULL);
			}
			is_game_over = true;
		}

		status_bar_1P.set_hp(player_1->get_hp());
		status_bar_1P.set_mp(player_1->get_mp());
		status_bar_2P.set_hp(player_2->get_hp());
		status_bar_2P.set_mp(player_2->get_mp());

		if (is_game_over)
		{
			pos_img_winner_bar.x += (int)(speed_winner_bar * delta);
			pos_img_winner_text.x += (int)(speed_winner_text * delta);
			if (!is_slide_out_start)
			{
				timer_winner_slide_in.on_update(delta);
				if(pos_img_winner_bar.x > pos_x_img_winner_bar_dst)
					pos_img_winner_bar.x = pos_x_img_winner_bar_dst;
				if(pos_img_winner_text.x > pos_x_img_winner_text_dst)
					pos_img_winner_text.x = pos_x_img_winner_text_dst;
			}
			else
				timer_winner_slide_out.on_update(delta);
		}
	}

	void on_draw(const Camera&  camera) {
		putimage_alpha(camera, pos_img_sky.x, pos_img_sky.y, &img_sky);
		putimage_alpha(camera, pos_img_hills.x, pos_img_hills.y, &img_hills);

		for (const Platform& platform : platform_list)
			platform.on_draw(camera);

		if (is_debug)
		{
			settextcolor(RGB(255, 0, 0));
			outtextxy(15, 15, _T("Debug Mode"));
		}
		player_1->on_draw(camera);
		player_2->on_draw(camera);

		for (const Bullet* bullet : bullet_list)
			bullet->on_draw(camera);

		if (is_game_over)
		{
			putimage_alpha(pos_img_winner_bar.x, pos_img_winner_bar.y, &img_winner_bar);
			putimage_alpha(pos_img_winner_text.x, pos_img_winner_text.y,
				player_1->get_hp() > 0 ? &img_1P_winner : &img_2P_winner);
		}
		else
		{
			status_bar_1P.on_draw();
			status_bar_2P.on_draw();
		}
	}

private:
	POINT pos_img_sky = { 0 }; // 天空图片位置
	POINT pos_img_hills = { 0 }; // 山脉图片位置

	StatusBar status_bar_1P;
	StatusBar status_bar_2P;
	bool is_game_over = false;

	POINT pos_img_winner_bar = { 0 };
	POINT pos_img_winner_text = { 0 };
	int pos_x_img_winner_bar_dst = 0;
	int pos_x_img_winner_text_dst = 0;
	Timer timer_winner_slide_in;
	Timer timer_winner_slide_out;
	bool is_slide_out_start = false;

	float speed_winner_bar = 3.0f;
	float speed_winner_text = 1.5f;
};
