#pragma once

#include<graphics.h>
#include"bullet.h"
#include"camera.h"
#include"platform.h"
#include"player_id.h"
#include"vector2.h"

extern std::vector<Bullet*> bullet_list;
extern std::vector<Platform> platform_list;

class Player
{
public:
	Player()
	{
		current_animation = &animation_idel_right;

		timer_attack_cd.set_wait_time(attack_cd);
		timer_attack_cd.set_one_shot(true);
		timer_attack_cd.set_callback([&]() { can_attack = true; });
	}
	~Player() = default;

	virtual void on_update(int delta)
	{
		int direction = is_right_key_down - is_left_key_down;
		if (direction != 0)
		{
			is_facing_right = direction > 0;
			current_animation = is_facing_right ? &animation_run_right : &animation_run_left;
			float distence = direction * run_velocity * delta;
			on_run(distence);
		}
		else
		{
			current_animation = is_facing_right? &animation_idel_right : &animation_idel_left;
		}
		current_animation->on_update(delta);
		timer_attack_cd.on_update(delta);
		move_and_collide(delta);
	}

	// 处理玩家的移动
	virtual void on_run(float distance)
	{
		if (is_attacking_ex)
			return;
		position.x += distance;
	}

	// 处理玩家的跳跃
	virtual void on_jump()
	{
		if(velocity.y != 0 || is_attacking_ex)
			return;
		velocity.y += jump_velocity;
	}

	// 绘制玩家
	virtual void on_draw(const Camera& camera)
	{
		current_animation->on_draw(camera, (int)position.x, (int)position.y);
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

	// 处理玩家的攻击
	virtual void on_attack() { }
	// 处理玩家的技能攻击
	virtual void on_attack_ex() { }
	const int get_hp() { return hp; }
	const int get_mp() { return mp; }

protected:
	// 移动碰撞检测
	void move_and_collide(float delta)
	{
		velocity.y += gravity * delta;
		position += velocity * (float)delta;

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
						break;
					}
				}
			}
		}

		for (Bullet* bullet : bullet_list) // 遍历画面中的每一个子弹
		{
			if(!bullet->get_valide() || bullet->get_collide_target() != id) // 无效的子弹或不是自己的子弹
				continue;
			if (bullet->check_collision(position, size)) // 子弹与玩家碰撞
			{
				bullet->on_collide(); // 执行碰撞逻辑
				bullet->set_valide(false); // 子弹失效
				hp -= bullet->get_damage(); // 玩家受到伤害
				using namespace std;
				cout << (id == PlayerID::P1 ? "P1" : "P2") << " HP: " << hp << endl;
			}
		}
	}

protected:
	const float run_velocity = 0.55f;
	const float gravity = 1.6e-3f;
	const float jump_velocity = -0.85f;

protected:
	int mp = 0; // 玩家的魔法值
	int hp = 100; // 玩家的血量

	Vector2 position; // 玩家的位置
	Vector2 velocity; // 玩家的速度
	Vector2 size;	  // 玩家的大小
	
	// 定义玩家不同状态下的动画
	Animation animation_idel_left;
	Animation animation_idel_right;
	Animation animation_run_left;
	Animation animation_run_right;
	Animation animation_attack_ex_left;
	Animation animation_attack_ex_right;

	Animation* current_animation = nullptr;
	
	// 定义玩家的ID
	PlayerID id = PlayerID::P1;
	
	// 定义玩家左右方向按键的状态
	bool is_left_key_down = false;
	bool is_right_key_down = false;

	bool is_facing_right = true;

	int attack_cd = 500; // 定义玩家的攻击冷却时间(ms)
	bool can_attack = true; // 定义玩家是否可以攻击
	Timer timer_attack_cd; // 定义玩家的攻击冷却时间

	bool is_attacking_ex = false; // 定义玩家是否使用技能
};

