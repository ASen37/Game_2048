#pragma once

#include<functional>
#include<graphics.h>
#include"vector2.h"
#include"camera.h"
#include"player_id.h"


class Bullet
{
public:
	Bullet() = default;
	~Bullet() = default;
	// 子弹伤害的get set方法
	void set_damage(int damage) { this->damage = damage; }
	int get_damage() const { return damage; }
	// 子弹位置的get set方法
	const Vector2& get_position() const
	{ 
		return position;
	}
	void set_position(float x, float y)
	{
		position.x = x;
		position.y = y;
	}
	// 获取子弹大小
	const Vector2& get_size() const
	{
		return size;
	}
	// 设置子弹速度
	void set_velocity(float x, float y)
	{
		velocity.x = x;
		velocity.y = y;
	}
	// 子弹碰撞目标的get set方法
	void set_collide_target(PlayerID target)
	{
		target_id = target;
	}
	PlayerID get_collide_target() const
	{
		return target_id;
	}
	// 设置子弹销毁时的回调函数
	void set_callback(std::function<void()> callback)
	{
		this->callback = callback;
	}
	// 子弹有效状态的get set方法
	void set_valide(bool valide)
	{
		this->valide = valide;
	}
	bool get_valide() const
	{
		return valide;
	}
	// 获取子弹是否可被移除状态
	bool get_can_remove() const
	{
		return can_remove;
	}
	
	// 碰撞时的回调函数
	virtual void on_collide()
	{
		if (callback)
			callback();
	}
	// 碰撞检测
	virtual bool check_collision(const Vector2& position, const Vector2& size)
	{
		return this->position.x + this->size.x / 2 >= position.x
			&& this->position.x + this->size.x / 2 <= position.x + size.x
			&& this->position.y + this->size.y / 2 >= position.y
			&& this->position.y + this->size.y / 2 <= position.y + size.y;
	}

	virtual void on_update(int delta) { }
	virtual void on_draw(const Camera& camera) const { }

protected:
	Vector2 size;	        //子弹大小
	Vector2 position;       //子弹位置
	Vector2 velocity;       //子弹速度
	int damage = 10;        //子弹伤害
	bool valide = true;     //子弹是否有效
	bool can_remove = false;   //子弹是否可以被移除
	std::function<void()> callback; //子弹销毁时的回调函数
	PlayerID target_id = PlayerID::P1; //子弹目标玩家
protected:
	bool check_if_exceeds_screen()
	{
		return (position.x + size.x <= 0 || position.x >= getwidth()
			|| position.y + size.y <= 0 || position.y >= getheight());
	}
};