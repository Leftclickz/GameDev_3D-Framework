#include "GamePCH.h"
#include "Mesh.h"
#include "TextMesh.h"
#include "Material.h"
#include "SpriteSheet.h"

TextMesh::TextMesh() : Mesh()
{
	m_DisplayText = "";
	m_PrimitiveType = GL_TRIANGLES;
	m_pMaterial = nullptr;
}

void TextMesh::SetText(std::string text)
{
	//Set our display then generate a new IBO to fit.
	m_DisplayText = text;
	if (m_pMaterial != nullptr)
		GenerateTextMesh();
}


void TextMesh::GenerateTextMesh()
{
	//new verts
	std::vector<VertexFormat> Verts;

	//Get the length of our string
	int length = m_DisplayText.length();

	//If the string is empty just return out.
	if (length == 0)
		return;

	//Convert the length into verts
	m_NumVerts = length * 6;
	
	//unsigned int* indices = new unsigned int[m_NumVerts];
	std::vector<unsigned int> Indices;

	Indices.reserve(m_NumVerts);
	Verts.reserve(m_NumVerts);

	vec2 position(0.0f, 0.0f);

	for (int i = 0; i < length; i++)
	{
		std::string character(1,m_DisplayText[i]);

		//fucking files cant be saved using certain symbols... so here we are.
		if (character == "/")
			character = "_Backslash";
		else if (character == "?")
			character = "_Question";
		else if (character == ":")
			character = "_Colon";
		else if (character == "(")
			character = "_OpeningBrace";
		else if (character == ")")
			character = "_ClosingBrace";
		else if (character == "{")
			character = "_LessThan";
		else if (character == "}")
			character = "_GreaterThan";
		else if (character == ".")
			character = "_Period";
		else if (character == ",")
			character = "_Comma";
		else if (character == "-")
			character = "_Minus";
		else if (character == "=")
			character = "_Equals";
		else if (character == "+")
			character = "_Plus";
		else if (character == "'")
			character = "_SingleQuote";
		else if (character == "\"")
			character = "_DoubleQuote";
		else if (character == "!")
			character = "_Exclamation";

		vec2 sprite_size;
		float scale_factor = 0.25f;

		//If we have a supported character we'll pull it up and use it for verts. If we have a space we'll use a default size (4,18)
		if (character != " ")
		{
			//fetch our spritesheet
			SpriteSheet* spriteSheet = m_pMaterial->GetSpriteSheet();

			//Fetch our sprite
			std::string text_name = character + ".png";
			spriteSheet->SetSprite(text_name);

			//Get our scaling and offset
			vec2 UV_Scale = spriteSheet->GetScale();
			vec2 UV_Offset = spriteSheet->GetOffset();

			//Get our sprite size
			sprite_size = UV_Scale * spriteSheet->GetSheetSize() * scale_factor;

			//push our verts
			Verts.push_back(VertexFormat(position, (vec2(0.0f, 0.0f) * UV_Scale) + UV_Offset));
			Verts.push_back(VertexFormat(position + vec2(0.0f, sprite_size.y), (vec2(0.0f, 1.0f) * UV_Scale) + UV_Offset));
			Verts.push_back(VertexFormat(position + sprite_size, (vec2(1.0f, 1.0f) * UV_Scale) + UV_Offset));
			Verts.push_back(VertexFormat(position + sprite_size, (vec2(1.0f, 1.0f) * UV_Scale) + UV_Offset));
			Verts.push_back(VertexFormat(position + vec2(sprite_size.x, 0.0f), (vec2(1.0f, 0.0f) * UV_Scale) + UV_Offset));
			Verts.push_back(VertexFormat(position, (vec2(0.0f, 0.0f) * UV_Scale) + UV_Offset));

			//push our indices
			Indices.push_back(i * 6);
			Indices.push_back(i * 6 + 1);
			Indices.push_back(i * 6 + 2);
			Indices.push_back(i * 6 + 3);
			Indices.push_back(i * 6 + 4);
			Indices.push_back(i * 6 + 5);
		}
		else
		{
			//Hardcoded size for empty characters.
			sprite_size = vec2(4.0f, 18.0f) * scale_factor;

			//push our verts
			Verts.push_back(VertexFormat(position, vec2(0.0f, 0.0f)));
			Verts.push_back(VertexFormat(position + vec2(0.0f, sprite_size.y), vec2(0.0f, 0.0f)));
			Verts.push_back(VertexFormat(position + sprite_size, vec2(0.0f, 0.0f)));
			Verts.push_back(VertexFormat(position + sprite_size, vec2(0.0f, 0.0f)));
			Verts.push_back(VertexFormat(position + vec2(sprite_size.x, 0.0f), vec2(0.0f, 0.0f)));
			Verts.push_back(VertexFormat(position, vec2(0.0f, 0.0f)));

			//copy indices
			Indices.push_back(i * 6);
			Indices.push_back(i * 6 + 1);
			Indices.push_back(i * 6 + 2);
			Indices.push_back(i * 6 + 3);
			Indices.push_back(i * 6 + 4);
			Indices.push_back(i * 6 + 5);

		}

		//increment our position x for each letter.
		position.x += sprite_size.x;
	}

	//create our IBO
	Init(&Verts[0], Verts.size(), &Indices[0], Indices.size(), m_PrimitiveType, GL_STATIC_DRAW);

	// Check for errors.
	CheckForGLErrors();
}
