#pragma once

#include<functional>
#include<graphics.h>
#include"atlas.h"
#include"camera.h"
#include"util.h"

class Animation
{
public:
	Animation() = default;                
	~Animation() = default;

	void reset() { //重置当前播放状态
		timer = 0;
		interval = 0;
	}

	void set_atlas(Atlas* new_atlas) { //设置当前图集
		reset();
		atlas = new_atlas;
	}

	void set_loop(bool flag) { //设置是否循环
		is_loop = flag;
	}

	void set_interval(int ms) { //设置帧间隔
		interval = ms;
	}

	int get_idx_frame() { //获取帧索引
		return idx_frame;
	}

	IMAGE* get_frame() { //获取当前帧
		atlas->get_image(idx_frame);
		return atlas->get_image(idx_frame);
	}

	bool check_finished() { //检查动画是否播放完毕
		if (is_loop) return false;
		return (idx_frame == atlas->get_size() - 1);
	}

	void on_update(int delta) { //动画帧更新播放
		timer += delta;
		if (timer >= interval){
			timer = 0;
			idx_frame++;
			if (idx_frame >= atlas->get_size()) {
				idx_frame = is_loop ? 0 : atlas->get_size() - 1;
				if (!is_loop && callback)
					callback();
			}
		}
	}

	void on_draw(const Camera& camera, int x, int y) const{ //渲染透明度图片
		putimage_alpha(camera, x, y, atlas->get_image(idx_frame));
	}

	void set_callback(std::function<void()> callback) { //设置类内回调函数
		this->callback = callback;
	}
private:
	int timer = 0; //计时器
	int interval = 0; //帧间隔
	int idx_frame = 0; //帧索引
	bool is_loop = true;
	Atlas* atlas = nullptr;
	std::function<void()> callback; //回调函数
};

