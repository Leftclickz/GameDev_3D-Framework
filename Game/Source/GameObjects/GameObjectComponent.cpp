#include "GamePCH.h"
#include "GameObjectComponent.h"

GameObjectComponent::GameObjectComponent(Scene * pScene, std::string name)
	: GameObject3D(pScene, name, Transform(), nullptr, nullptr)
{
}

GameObjectComponent::~GameObjectComponent()
{
}
