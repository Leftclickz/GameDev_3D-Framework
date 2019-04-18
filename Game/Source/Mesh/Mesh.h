#ifndef __Mesh_H__
#define __Mesh_H__

class Texture;
class Camera;
class Material;
class LightObject;

struct VertexFormat
{
    vec3 m_Pos;
	vec3 m_Norm;
    MyColor m_Color;
    vec2 m_UV;
	VertexFormat() {}

    VertexFormat(vec3 pos, MyColor color, vec2 uv)
    {
        m_Pos = pos;
        m_Color = color;
        m_UV = uv;
		m_Norm = vec3(0.0f, 0.0f, 0.0f);
    }

	VertexFormat(vec3 pos, vec2 uv)
	{
		m_Pos = pos;
		m_Color = MyColor(255,0,0,255);
		m_UV = uv;
		m_Norm = vec3(0.0f, 0.0f, 0.0f);
	}

	VertexFormat(vec3 pos)
	{
		m_Pos = pos;
		m_Color = MyColor(255, 0, 0, 255);
		m_UV = vec2(0,0);
		m_Norm = vec3(0.0f, 0.0f, 0.0f);
	}

	VertexFormat(vec3 pos, vec3 norm, MyColor color, vec2 uv)
	{
		m_Pos = pos;
		m_Color = color;
		m_UV = uv;
		m_Norm = norm;
	}

};

class Mesh
{
protected:
    GLuint m_VBO;
    GLuint m_IBO;

    unsigned int m_NumVerts;
    unsigned int m_NumIndices;

	std::vector<vec3> m_OBJVerts;
	std::vector<VertexFormat> m_OBJvertexAttributes;

    GLenum m_PrimitiveType;

public:
    Mesh();
    virtual ~Mesh();

    void Init(VertexFormat* verts, int numVerts, unsigned int* indices, int numIndices, GLenum primitiveType, GLenum usage);
    void Init(std::vector<VertexFormat> verts, int numVerts, GLenum primitiveType, GLenum usage);

    void SetupAttributes(ShaderProgram* pShaderProgram);
    static void SetupUniforms(mat4 matrix, mat4 normalmatrix, Camera* camera, Material* material, std::vector<LightObject*> *Lights = nullptr);


    void Draw(ShaderProgram* pShaderProgram);

    // The following functions are defined in MeshShapes.cpp:
    void CreateBox(vec2 size, vec2 offset = vec2(0));
	void CreateCube(vec3 size, vec3 offset = vec3(0));
	void CreatePlane(vec2 worldsize, ivec2 vertcount);
	void GenerateOBJ(const char* objfilename, float scale = 1.0f, vec3 vertoffset = vec3(0));

	std::vector<vec3> GetOBJVerts() { return m_OBJVerts; }
	unsigned int GetNumVerts() { return m_NumVerts; }
};

#endif //__Mesh_H__
