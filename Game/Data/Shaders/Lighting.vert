#version 120

attribute vec3 a_Position;
attribute vec3 a_Normal;
attribute vec2 a_UV;
attribute vec4 a_Color;

uniform mat4 u_WorldMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_NormalMatrix;

varying vec2 v_UV;
varying vec3 v_WorldPosition;

varying vec3 v_LightPos;
varying vec3 v_Surfacenormal;

void main()
{
	// Store object space position.
	vec4 objectpos = vec4(a_Position, 1);

	// Calculate world space position.
	vec4 worldpos = u_WorldMatrix * objectpos;

	// Calculate view space position.
	vec4 viewpos = u_ViewMatrix * worldpos;

	// Calculate clip space position.
	vec4 clippos = u_ProjectionMatrix * viewpos;

	// Store final position in gl_Position
    gl_Position = clippos;

	v_UV = a_UV;
	v_WorldPosition = worldpos.xyz;
	vec4 normals = vec4(a_Normal, 1);
	v_Surfacenormal = (u_NormalMatrix * normals).xyz;

	vec3 pos = v_WorldPosition;
	pos.y += 15;
	v_LightPos = pos;
}
