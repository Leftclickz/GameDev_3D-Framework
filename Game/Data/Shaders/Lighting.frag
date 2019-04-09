#version 120

uniform sampler2D u_Texture;
varying vec2 v_UV;

varying vec3 v_WorldPosition;
varying vec3 v_Surfacenormal;
uniform vec3 u_Campos;


void main()
{
	vec4 matcolor = texture2D( u_Texture, v_UV );

	gl_FragColor = ApplyLights(v_WorldPosition, v_Surfacenormal, u_Campos, matcolor, u_Lights, 0.7);
}
