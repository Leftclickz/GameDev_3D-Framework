#pragma once
#include "Mesh.h"

class Material;
/*
Mesh that requires a material to function properly (USE MY FONT PNG)
Automatically adjusts the IBO whenever text is adjusted giving a tight-fit sized mesh
*/
class TextMesh : public Mesh
{

public:

	TextMesh();
	virtual ~TextMesh() {}

	//Text material required for mesh to function
	void SetMaterial(Material* pMaterial) { m_pMaterial = pMaterial; }

	//should return a text material or null
	Material* GetMaterial() { return m_pMaterial;}

	//Set new text
	void SetText(std::string text);

	//Get current text
	std::string GetText() { return m_DisplayText; }

protected:

	//Generate IBO for text.
	void GenerateTextMesh();

	std::string m_DisplayText;
	Material* m_pMaterial;
};