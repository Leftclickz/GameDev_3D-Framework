#include "GamePCH.h"
#include "DebugDraw3D.h"
#include "Mesh/Mesh.h"
#include "GameObjects/Camera.h"
#include "Mesh/Material.h"

DebugDraw3D::DebugDraw3D()
	:m_debugMode(0)
{
	m_pMaterial = nullptr;
	m_pCamera = nullptr;
}

void    DebugDraw3D::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

int DebugDraw3D::getDebugMode() const
{
	return m_debugMode;
}

void DebugDraw3D::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	GLuint shader = m_pMaterial->GetShader()->GetProgram();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	vec3 vertices[2];
	vertices[0] = vec3(from.x(), from.y(), from.z());
	vertices[1] = vec3(to.x(), to.y(), to.z());

	GLint loc = glGetAttribLocation(shader, "a_Position");
	if (loc != -1)
	{
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 12, &vertices[0].x);
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
	glColor4f(color.x(), color.y(), color.z(), 1);
	glDrawArrays(GL_LINE_LOOP, 0, 2);

	glEnable(GL_DEPTH_TEST);

}

void    DebugDraw3D::draw3dText(const btVector3& location, const char* textString)
{
	//glRasterPos3f(location.x(),  location.y(),  location.z());
	//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);
}

void    DebugDraw3D::reportErrorWarning(const char* warningString)
{
	printf(warningString);
}

void    DebugDraw3D::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	{
		//btVector3 to=pointOnB+normalOnB*distance;
		//const btVector3&from = pointOnB;
		//glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);   

		//GLDebugDrawer::drawLine(from, to, color);

		//glRasterPos3f(from.x(),  from.y(),  from.z());
		//char buf[12];
		//sprintf(buf," %d",lifeTime);
		//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);
	}
}