#ifndef _ECS_H_
#define _ECS_H_

#include "BoundingVolume.h"

class Entity {
private:
	std::list<std::unique_ptr<Entity>> children;
	Entity* parent = nullptr;
	Model* parentModel = nullptr;

	Transform transform;

public:
	Entity() = default;

	template<typename... Targs>
	void addChild(const Targs&... args) {
		children.emplace_back(std::make_unique<Entity>(args...));
		children.back()->parent = this;
	}

	~Entity() = default;
};

#endif