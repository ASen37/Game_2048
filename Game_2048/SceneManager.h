#pragma once
#include"scene.h"
#include"camera.h"
extern Scene* menu_scene;
extern Scene* game_scene;
extern Scene* select_scene;

class SceneManager
{
public:
	enum class SceneType {
		Menue,
		Game,
		Selector
	};

public:
	SceneManager() = default;
	~SceneManager() = default;

	void set_current_scene(Scene* s) {
		current_scene = s;
		current_scene->on_enter();
	}

	void switch_to(SceneType type){
		current_scene->on_exit();
		switch (type) {
		case SceneType::Game:
			current_scene = game_scene;
			break;
		case SceneType::Menue:
			current_scene = menu_scene;
			break;
		case SceneType::Selector:
			current_scene = select_scene;
		default:
			break;
		}
		current_scene->on_enter();
	}

	void on_draw(const Camera& camera) {
		current_scene->on_draw(camera);
	}

	void on_update(int delta) {
		current_scene->on_update(delta);
	}

	void on_input(const ExMessage& msg) {
		current_scene->on_input(msg);
	}

private:
	Scene* current_scene = nullptr;
};


