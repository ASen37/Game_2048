#pragma once

#include"vector2.h"
#include"atlas.h"
#include"util.h"
#include"camera.h"

class Particle
{
public:
	Particle() = default;
	Particle(Vector2& pos, Atlas* atlas, int lifespan):
		position(pos), atlas(atlas), lifespan(lifespan) { }
	~Particle() = default;

	void set_position(const Vector2& pos)
	{
		position = pos;
	}

	void set_atlas(Atlas* new_atlas)
	{
		atlas = new_atlas;
	}

	void set_lifespan(int ms)
	{
		lifespan = ms;
	}

	bool check_valid() const
	{
		return valid;
	}

	void on_update(int delta) 
	{
		timer += delta;
		if (timer >= lifespan)
		{
			timer = 0;
			idx_frame++;
			if (idx_frame >= atlas->get_size())
			{
				idx_frame = atlas->get_size() - 1;
				valid = false;
			}
		}
	}

	void on_draw(const Camera& camera) const
	{
		putimage_alpha(camera, (int)position.x, (int)position.y, atlas->get_image(idx_frame));
	}

private:
	int timer = 0;           // 粒子动画播放计时器
	int lifespan = 0;        // 单帧粒子动画持续时间
	int idx_frame = 0;       // 当前正在播放的粒子动画帧索引
	Vector2 position;        // 粒子位置
	bool valid = true;       // 粒子对象是否有效
	Atlas* atlas = nullptr;  // 粒子动画资源
};




