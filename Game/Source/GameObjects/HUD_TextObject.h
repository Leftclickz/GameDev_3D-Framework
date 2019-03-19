#pragma once
#include "GameObject.h"

class TextMesh;

/*
Base class for an object using a dynamic-sized mesh and a font texture
*/
class HUD_TextObject : public GameObject
{
public:

	HUD_TextObject(Scene* pScene, std::string name, Transform transform, TextMesh* pMesh, Material* pMaterial);
	virtual ~HUD_TextObject();

	//Set the display string
	void SetString(std::string string);

	//Get the display string
	std::string GetString();

	virtual void Draw(Camera* camera) override;

	//Set whether or not the object is rendered
	void SetIsVisible(bool visibility) { m_IsVisible = visibility; }
	
	//Does this object render?
	bool IsVisible() { return m_IsVisible; }

protected:

	TextMesh* m_TextMesh;
	bool m_IsVisible;
};