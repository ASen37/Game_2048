#pragma once

#include"timer.h"
#include"vector2.h"

class Camera
{
public:
	//lambda函数如果想要修改成员函数变量，需将this作为引用传递进去：[&]
	//如果仅仅访问成员变量，则将显式捕获this指针即可：[this]
	//如果想要想要捕获更多的变量，则可床传递多个参数：[this, value]
	Camera()
	{
		timer_shake.set_one_shot(true);
		timer_shake.set_callback(
			[&]() {
				is_shaking = false;
				reset();
			}
		);
	}

	~Camera() = default;

	const Vector2& get_position() const {
		return position;
	}

	void reset() {
		position.x = 0;
		position.y = 0;
	}

	void on_update(int delta) {
		timer_shake.on_update(delta);
		if (is_shaking) {
			position.x = (-50 + rand() % 100) / 50.0f * shaking_strength;
			position.y = (-50 + rand() % 100) / 50.0f * shaking_strength;
		}
	}

	void shake(float strength, int duration) {
		is_shaking = true;
		shaking_strength = strength;
		timer_shake.set_wait_time(duration);
		timer_shake.restart();
	}


private:
	Timer timer_shake;
	Vector2 position;
	bool is_shaking = false;
	float shaking_strength = 0;
};
