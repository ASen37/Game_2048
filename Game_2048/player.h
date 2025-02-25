#pragma once

#include<graphics.h>
#include"bullet.h"
#include"camera.h"
#include"platform.h"
#include"player_id.h"
#include"vector2.h"
#include"particle.h"

extern Atlas atlas_run_effect;
extern Atlas atlas_jump_effect;
extern Atlas atlas_land_effect;

extern IMAGE img_1P_cursor;
extern IMAGE img_2P_cursor;

extern std::vector<Bullet*> bullet_list;
extern std::vector<Platform> platform_list;

class Player
{
public:
	Player(bool facing_right = true) : is_facing_right(facing_right)
	{
		current_animation = is_facing_right ? &animation_idel_right : &animation_idel_left;
		//* 游戏开始是动画对象的初始化，依次为：
		//* 站立动画、跑动动画
		animation_jump_effect.set_atlas(&atlas_jump_effect);
		animation_jump_effect.set_interval(25);
		animation_jump_effect.set_loop(false);
		animation_jump_effect.set_callback([&]()
			{
				is_jump_effect_visible = false;
			});

		animation_land_effect.set_atlas(&atlas_land_effect);
		animation_land_effect.set_interval(50);
		animation_land_effect.set_loop(false);
		animation_land_effect.set_callback([&]()
			{
				is_land_effect_visible = false;
			});
		//* 定时器对象初始化，依次为：
		//* 攻击间隔
		//* 无敌时间、无敌闪烁间隔
		//* 跑动效果生成间隔、死亡效果生成间隔
		//* 玩家光标可见持续时间
		timer_attack_cd.set_wait_time(attack_cd);
		timer_attack_cd.set_one_shot(true);
		timer_attack_cd.set_callback([&]() { can_attack = true; });

		timer_invulnerable.set_wait_time(750);
		timer_invulnerable.set_one_shot(true);
		timer_invulnerable.set_callback([&]() { is_invulnerable = false; });

		timer_invulnerable_blink.set_wait_time(75);
		timer_invulnerable_blink.set_callback([&]() {
			is_showing_sketch_frame = !is_showing_sketch_frame;
		});

		timer_run_effect_generation.set_wait_time(75);
		timer_run_effect_generation.set_callback([&]()
			{
				Vector2 particle_pos;
				IMAGE* frame = atlas_run_effect.get_image(0);
				particle_pos.x = position.x + (size.x - frame->getwidth()) / 2;
				particle_pos.y = position.y + size.y - frame->getheight();
				particle_list.emplace_back(particle_pos, &atlas_run_effect, 45);
			});

		timer_die_effect_generation.set_wait_time(35);
		timer_die_effect_generation.set_callback([&]()
			{
				Vector2 particle_pos;
				IMAGE* frame = atlas_run_effect.get_image(0);
				particle_pos.x = position.x + (size.x - frame->getwidth()) / 2;
				particle_pos.y = position.y + size.y - frame->getheight();
				particle_list.emplace_back(particle_pos, &atlas_run_effect, 150);
			});

		timer_cursor_visible.set_wait_time(2500);
		timer_cursor_visible.set_one_shot(true);
		timer_cursor_visible.set_callback([&]()
			{
				is_cursor_visible = false;
			});
	}
	~Player() = default;

	virtual void on_update(int delta)
	{
		// direction 判断玩家移动状态：0表示不动，1表示向右移动，-1表示向左移动
		int direction = is_right_key_down - is_left_key_down;
		if (direction != 0)
		{
			if(!is_attacking_ex) // 限制放技能状态下不能转向
				is_facing_right = direction > 0;
			current_animation = is_facing_right ? &animation_run_right : &animation_run_left;
			float distence = direction * run_velocity * delta;
			on_run(distence);
		}
		else
		{
			current_animation = is_facing_right? &animation_idel_right : &animation_idel_left;
			timer_run_effect_generation.pause();
		}

		if(is_attacking_ex)
			current_animation = is_facing_right ? &animation_attack_ex_right : &animation_attack_ex_left;
		if(hp <= 0)
			current_animation = last_hurt_direction.x < 0 ? &animation_die_left : &animation_die_right;
		// 动画对象执行更新
		current_animation->on_update(delta);
		animation_jump_effect.on_update(delta);
		animation_land_effect.on_update(delta);
		// 定时器对象执行更新
		timer_attack_cd.on_update(delta);
		timer_invulnerable.on_update(delta);
		timer_invulnerable_blink.on_update(delta);
		timer_run_effect_generation.on_update(delta);
		timer_cursor_visible.on_update(delta);

		if (hp <= 0) // 死亡粒子动效
			timer_die_effect_generation.on_update(delta);
		// 粒子动效更新及清除
		particle_list.erase(std::remove_if(particle_list.begin(), particle_list.end(), 
			[](const Particle& particle) 
			{ 
				return !particle.check_valid(); 
			}), particle_list.end());
		for (Particle& particle : particle_list)
			particle.on_update(delta);

		// 无敌帧中播放玩家纯白剪影
		if (is_showing_sketch_frame)
			sketch_image(current_animation->get_frame(), &img_sketh);

		move_and_collide(delta);
	}

	// 处理玩家的移动
	virtual void on_run(float distance)
	{
		if (is_attacking_ex) // 限制放技能状态下不能移动
			return;
		position.x += distance;
		timer_run_effect_generation.resume();
	}

	// 处理玩家的跳跃
	virtual void on_jump()
	{
		if(velocity.y != 0 || is_attacking_ex)
			return;
		velocity.y += jump_velocity;

		is_jump_effect_visible = true;
		animation_jump_effect.reset();
		IMAGE* effect_frame = animation_jump_effect.get_frame();
		position_jump_effect.x = position.x + (size.x - effect_frame->getwidth()) / 2;
		position_jump_effect.y = position.y + size.y - effect_frame->getheight();
	}

	// 处理玩家的落地
	virtual void on_land()
	{
		is_land_effect_visible = true;
		animation_land_effect.reset();
		IMAGE* effect_frame = animation_land_effect.get_frame();
		position_land_effect.x = position.x + (size.x - effect_frame->getwidth()) / 2;
		position_land_effect.y = position.y + size.y - effect_frame->getheight();
	}

	// 绘制
	virtual void on_draw(const Camera& camera)
	{
		if (is_jump_effect_visible)
			animation_jump_effect.on_draw(camera, (int)position_jump_effect.x, (int)position_jump_effect.y);

		if (is_land_effect_visible)
			animation_land_effect.on_draw(camera, (int)position_land_effect.x, (int)position_land_effect.y);
		
		for(const Particle& particle : particle_list)
			particle.on_draw(camera);

		if (hp > 0 && is_invulnerable && is_showing_sketch_frame)
			putimage_alpha(camera, (int)position.x, (int)position.y, &img_sketh);
		else
			current_animation->on_draw(camera, (int)position.x, (int)position.y);

		if (is_cursor_visible)
		{
			switch (id)
			{
			case PlayerID::P1:
				putimage_alpha(camera, (int)position.x + (size.x - img_1P_cursor.getwidth()) / 2,
					(int)(position.y - img_1P_cursor.getheight()), &img_1P_cursor);
				break;
			case PlayerID::P2:
				putimage_alpha(camera, (int)position.x + (size.x - img_2P_cursor.getwidth()) / 2,
					(int)(position.y - img_2P_cursor.getheight()), &img_2P_cursor);
				break;
			}
		}
	}

	// 处理玩家的输入
	virtual void on_input(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_KEYDOWN: 
			switch (id)
			{
			case PlayerID::P1:
				switch (msg.vkcode)
				{
				case 0x41: // A
					is_left_key_down = true;
					break;
				case 0x44: // D
					is_right_key_down = true;
					break;
				case 0x57: // W
					on_jump();
					break;
				case 0x46: // F
					if (can_attack)
					{
						on_attack();
						can_attack = false;
						timer_attack_cd.restart();
					}
					break;
				case 0x47: // G
					if (mp >= 100)
					{
						on_attack_ex();
						mp = 0;
					}
					break;
				}
				break;
			case PlayerID::P2:
				switch (msg.vkcode)
				{
				case VK_LEFT: // ←
					is_left_key_down = true;
					break;
				case VK_RIGHT: // →
					is_right_key_down = true;
					break;
				case VK_UP: // ↑
					on_jump();
					break;
				case VK_OEM_PERIOD: // .
					if (can_attack)
					{
						on_attack();
						can_attack = false;
						timer_attack_cd.restart();
					}
					break;
				case VK_OEM_2: // '/'
					if (mp >= 100)
					{
						on_attack_ex();
						mp = 0;
					}
					break;
				}
				break;
			}
			break;
		case WM_KEYUP:
			switch (id)
			{
			case PlayerID::P1:
				switch (msg.vkcode)
				{
				case 0x41: // A
					is_left_key_down = false;
					break;
				case 0x44: // D
					is_right_key_down = false;
					break;
				}
				break;
			case PlayerID::P2:
				switch (msg.vkcode)
				{
				case VK_LEFT: // ←
					is_left_key_down = false;
					break;
				case VK_RIGHT: // →
					is_right_key_down = false;
					break;
				}
				break;
			}
			break;
		default:
			break;
		}
	}

	// 设置玩家的ID
	void set_id(PlayerID id)
	{
		this->id = id;
	}

	// 设置玩家的位置
	void set_position(float x, float y)
	{
		position.x = x;
		position.y = y;
	}

	const Vector2& get_position() const
	{
		return position;
	}

	const Vector2& get_size() const
	{
		return size;
	}

	// 设置玩家进入无敌状态
	void make_invulnerable()
	{
		is_invulnerable = true;
		timer_invulnerable.restart();
	}

	// 处理玩家的攻击
	virtual void on_attack() { }
	// 处理玩家的技能攻击
	virtual void on_attack_ex() { }
	int get_hp() const { return hp; }
	int get_mp() const { return mp; }
	void set_hp(int val){ hp = val; }

protected:
	// 移动碰撞检测
	void move_and_collide(float delta)
	{
		float last_velocity_y = velocity.y;
		
		// 重力模拟
		velocity.y += gravity * delta;
		position += velocity * (float)delta;

		if(hp <= 0)
			return;

		if (velocity.y > 0)
		{
			for (const Platform& platform : platform_list)
			{
				const Platform::CollisionShape& shape = platform.shape;
				bool is_collide_x = (max(position.x + size.x, shape.right)- min(position.x, shape.left)
					<= size.x + (shape.right - shape.left));
				bool is_collide_y = (shape.y >= position.y && shape.y <= position.y + size.y);
				
				if (is_collide_x && is_collide_y)
				{
					float delta_pos_y = velocity.y * delta;
					float last_tick_foot_pos_y = position.y + size.y - delta_pos_y;
					if (last_tick_foot_pos_y <= shape.y)
					{
						position.y = shape.y - size.y;
						velocity.y = 0;
						if(last_velocity_y != 0)
							on_land();

						break;
					}
				}
			}
		}

		if (!is_invulnerable)
		{
			for (Bullet* bullet : bullet_list) // 遍历画面中的每一个子弹
			{
				if (!bullet->get_valide() || bullet->get_collide_target() != id) // 无效的子弹或不是自己的子弹
					continue;
				if (bullet->check_collision(position, size)) // 子弹与玩家碰撞
				{
					make_invulnerable();
					bullet->on_collide(); // 执行碰撞逻辑
					bullet->set_valide(false); // 子弹失效
					hp -= bullet->get_damage(); // 玩家受到伤害
					last_hurt_direction = bullet->get_position() - position;
					if (hp <= 0)
					{
						velocity.x = last_hurt_direction.x < 0 ? 0.35f : -0.35f;
						velocity.y = -1.0f;
					}
				}
			}
		}
	}

protected:
	const float run_velocity = 0.55f;
	const float gravity = 1.6e-3f;
	const float jump_velocity = -0.85f;

protected:
	int mp = 0;                  // 玩家的魔法值
	int hp = 100;                // 玩家的血量

	Vector2 position;            // 玩家的位置
	Vector2 velocity;            // 玩家的速度
	Vector2 size;	             // 玩家的大小
	
	// 定义玩家不同状态下的动画
	Animation animation_idel_left;
	Animation animation_idel_right;
	Animation animation_run_left;
	Animation animation_run_right;
	Animation animation_attack_ex_left;
	Animation animation_attack_ex_right;
	Animation animation_jump_effect;
	Animation animation_land_effect;
	Animation animation_die_left;
	Animation animation_die_right;

	bool is_jump_effect_visible = false; // 跳跃动效是否可见
	bool is_land_effect_visible = false; // 落地动效是否可见

	Vector2 position_jump_effect; // 跳跃动效位置
	Vector2 position_land_effect; // 落地动效位置

	Animation* current_animation = nullptr;
	
	// 定义玩家的ID
	PlayerID id = PlayerID::P1;
	
	// 定义玩家左右方向按键的状态
	bool is_left_key_down = false;
	bool is_right_key_down = false;

	bool is_facing_right = true;

	int attack_cd = 500;                   // 定义玩家的攻击冷却时间(ms)
	bool can_attack = true;                // 定义玩家是否可以攻击
	Timer timer_attack_cd;                 // 定义玩家的攻击冷却时间
									       
	bool is_attacking_ex = false;          // 定义玩家是否使用技能

	bool is_invulnerable = false;          // 定义玩家是否无敌
	bool is_showing_sketch_frame = false;  // 是否播放玩家剪影动画
	Timer timer_invulnerable;              // 无敌帧定时器
	Timer timer_invulnerable_blink;        // 无敌帧闪烁定时器
	IMAGE img_sketh;                       // 玩家剪影动画图片

	std::vector<Particle> particle_list;   // 玩家粒子列表
	Timer timer_run_effect_generation;     // 玩家奔跑状态粒子发射定时器
	Timer timer_die_effect_generation;     // 玩家死亡状态粒子发射定时器

	bool is_cursor_visible = true;         // 玩家是否显示光标
	Timer timer_cursor_visible;            // 光标显示定时器

	Vector2 last_hurt_direction;           // 玩家最后受伤方向
};

