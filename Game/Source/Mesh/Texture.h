#ifndef __Texture_H__
#define __Texture_H__

class Texture
{
protected:
    GLuint m_TextureID;

public:
    Texture(const char* filename);
	Texture();
    virtual ~Texture();

    GLuint GetTextureID() { return m_TextureID; }
	void SetTectureID(GLuint id) { m_TextureID = id; }
};

#endif //__Texture_H__
