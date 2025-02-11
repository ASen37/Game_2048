#pragma once

#include"SceneManager.h"
#include"util.h"
#include"camera.h"
#include"scene.h"
#include"platform.h"
#include"player.h"


extern IMAGE img_sky; // 天空图片
extern IMAGE img_hills; // 山脉图片
extern IMAGE img_platform_large; //大型平台图片
extern IMAGE img_platform_small; //小型平台图片

extern Camera main_camera;
extern SceneManager scene_manager;
extern std::vector<Platform> platform_list;

extern Player* player_1;
extern Player* player_2;

class GameScene : public Scene
{
public:
	GameScene() = default;
	~GameScene() = default;
	void on_enter() {
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
	}
	
	void on_exit() {
		
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
			else if(msg.vkcode == VK_ESCAPE) // ESC 键退出
			{
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
	}



private:
	POINT pos_img_sky = { 0 }; // 天空图片位置
	POINT pos_img_hills = { 0 }; // 山脉图片位置


};
