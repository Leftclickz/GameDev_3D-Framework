#include "GamePCH.h"
#include "DebugDraw.h"
#include "../GameObjects/Camera.h"
#include "../Mesh/Mesh.h"
#include "../Mesh/Material.h"

DebugDraw::DebugDraw()
{
	m_pCamera = nullptr;
	m_pMaterial = nullptr;
}

DebugDraw::~DebugDraw()
{

}

//used for AABB
void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	GLuint shader = m_pMaterial->GetShader()->GetProgram();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLint loc = glGetAttribLocation(shader, "a_Position");
	if (loc != -1)
	{
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 8, &vertices[0].x);
		glEnableVertexAttribArray(loc);
	}

	glDisable(GL_DEPTH_TEST);

	mat4 matrix;
	matrix.SetIdentity();

	mat4 matrix2;
	matrix2.SetIdentity();

	Mesh::SetupUniforms(matrix, matrix2, m_pCamera, m_pMaterial);

	//draw lines
	glLineWidth(3); //fat lines
	glColor4f(1, 0, 1, 1);
	glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
}

//used for shape
void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	GLuint shader = m_pMaterial->GetShader()->GetProgram();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLint loc = glGetAttribLocation(shader, "a_Position");
	if (loc != -1)
	{
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 8, &vertices[0].x);
		glEnableVertexAttribArray(loc);
	}

	glDisable(GL_DEPTH_TEST);

	mat4 matrix;
	matrix.SetIdentity();
	mat4 matrix2;
	matrix2.SetIdentity();

	Mesh::SetupUniforms(matrix, matrix2, m_pCamera, m_pMaterial);

	glColor4f(1, 0, 0, 0.5f);
	glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
}

//used for AABB
void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	GLuint shader = m_pMaterial->GetShader()->GetProgram();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	int vertcount = 360;
	std::vector<VertexFormat> format;
	//Center Position
	format.push_back(VertexFormat(vec3(center.x, center.y, 0)));

	//The main body vertices
	float referenceAngle = 2 * PI / vertcount;
	float angle = 0;
	for (int x = 0; x < vertcount; x++)
	{
		float vertx = center.x + (radius * cosf(angle));
		float verty = center.y + (radius * sinf(angle));

		format.push_back(VertexFormat(vec3(vertx, verty, 0)));

		angle += referenceAngle;
	}

	//The last vertex which is in the same place as the first, non-center, vertex to complete the object
	format.push_back(format.at(1));

	//Add the 2 extra points so the buffer can know about them
	vertcount += 2;
	// Generate and fill buffer with our attributes.


	GLint loc = glGetAttribLocation(shader, "a_Position");
	if (loc != -1)
	{
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), &format[0].m_Pos.x);
		glEnableVertexAttribArray(loc);
	}

	glDisable(GL_DEPTH_TEST);

	mat4 matrix;
	matrix.SetIdentity();
	mat4 matrix2;
	matrix2.SetIdentity();

	Mesh::SetupUniforms(matrix, matrix2, m_pCamera, m_pMaterial);

	glLineWidth(3); //fat lines
	glColor4f(1, 0, 1, 1);
	glDrawArrays(GL_LINE_LOOP, 0, vertcount);
}

//used for shape
void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	GLuint shader = m_pMaterial->GetShader()->GetProgram();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	int vertcount = 360;
	std::vector<VertexFormat> format;
	//Center Position
	format.push_back(VertexFormat(vec3(center.x, center.y, 0)));

	//The main body vertices
	float referenceAngle = 2 * PI / vertcount;
	float angle = 0;
	for (int x = 0; x < vertcount; x++)
	{
		float vertx = center.x + (radius * cosf(angle));
		float verty = center.y + (radius * sinf(angle));

		format.push_back(VertexFormat(vec3(vertx, verty, 0)));

		angle += referenceAngle;
	}

	//The last vertex which is in the same place as the first, non-center, vertex to complete the object
	format.push_back(format.at(1));

	//Add the 2 extra points so the buffer can know about them
	vertcount += 2;
	// Generate and fill buffer with our attributes.


	GLint loc = glGetAttribLocation(shader, "a_Position");
	if (loc != -1)
	{
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), &format[0].m_Pos.x);
		glEnableVertexAttribArray(loc);
	}

	glDisable(GL_DEPTH_TEST);

	mat4 matrix;
	matrix.SetIdentity();
	mat4 matrix2;
	matrix2.SetIdentity();

	Mesh::SetupUniforms(matrix, matrix2, m_pCamera, m_pMaterial);
	glColor4f(1, 0, 0, 0.5f);

	glDrawArrays(GL_TRIANGLE_FAN, 0, vertcount);	
}

//used for joint
void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	GLuint shader = m_pMaterial->GetShader()->GetProgram();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	b2Vec2 vertices[2];
	vertices[0] = p1;
	vertices[1] = p2;

	GLint loc = glGetAttribLocation(shader, "a_Position");
	if (loc != -1)
	{
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 8, &vertices[0].x);
		glEnableVertexAttribArray(loc);
	}

	glDisable(GL_DEPTH_TEST);

	mat4 matrix;
	matrix.SetIdentity();
	mat4 matrix2;
	matrix2.SetIdentity();

	Mesh::SetupUniforms(matrix, matrix2, m_pCamera, m_pMaterial);

	//glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

	//draw lines
	glLineWidth(3); //fat lines
	glColor4f(0, 1, 1, 1);
	glDrawArrays(GL_LINE_LOOP, 0, 2);
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
}

//idk if this works we don't have any points
void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	GLuint shader = m_pMaterial->GetShader()->GetProgram();
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	GLint loc = glGetAttribLocation(shader, "a_Position");
	if (loc != -1)
	{
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 2, &p.x);
		glEnableVertexAttribArray(loc);
	}

	glDisable(GL_DEPTH_TEST);

	mat4 matrix;
	matrix.SetIdentity();
	mat4 matrix2;
	matrix2.SetIdentity();

	Mesh::SetupUniforms(matrix, matrix2, m_pCamera, m_pMaterial);

	glColor4f(1, 0, 1, 1);

	glPointSize(size);
	glDrawArrays(GL_POINT, 0, 1);
}



void DebugDraw::SetCamera(Camera* cam)
{
	m_pCamera = cam;
}

void DebugDraw::SetMaterial(Material* mat)
{
	m_pMaterial = mat;
}
