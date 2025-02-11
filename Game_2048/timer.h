#pragma once
#include<functional>

class Timer
{
public:
	Timer() = default;
	~Timer() = default;

	void restart() { //重置定时器状态
		pass_time = 0;
		shotted = false;
	}

	void set_wait_time(int swt) { //设置等待时间
		wait_time = swt;
	}

	void set_one_shot(bool flag) { //设置单次触发状态
		one_shot = flag;
	}

	void set_callback(std::function<void()> callback) { //设置回调函数
		this->callback = callback;
	}

	//对于暂停状态的设置
	void pause() { paused = true; }
	void resume() { paused = false; }

	void on_update(int delta) { //定时器更新
		if (paused) return;

		pass_time += delta;
		if (pass_time >= wait_time) {
			if ((!one_shot || (one_shot && !shotted)) && callback)
				callback(); //定时器触发逻辑：不是单次触发 或 是单次触发但没有被触发过 => 调用回调函数触发定时器
			shotted = true;
			pass_time = 0;
		}
	}

private:
	int pass_time = 0; //已过时间
	int wait_time = 0; //等待时间
	bool paused = false; //是否暂停
	bool shotted = false; //是否触发
	bool one_shot = false; //是否单次触发
	std::function<void()> callback; //回调函数
};

