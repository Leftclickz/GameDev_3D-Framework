#version 120

attribute vec3 a_Position;
attribute vec3 a_Normal;
attribute vec2 a_UV;
attribute vec4 a_Color;

uniform mat4 u_WorldMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

varying vec2 v_UV;
varying vec4 v_Color;

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

	vec4 normal_color = vec4(0.5f + a_Normal.x * 0.5,0.5f + a_Normal.y * 0.5, 0.5f + a_Normal.z * 0.5 , 1.0f);

	v_Color = normal_color;
}
