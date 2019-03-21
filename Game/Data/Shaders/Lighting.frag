#version 120

uniform sampler2D u_Texture;

varying vec2 v_UV;
varying vec3 v_WorldPosition;

varying vec3 v_LightPos;
varying vec3 v_Surfacenormal;

uniform vec4 u_MaterialColor;
uniform vec4 u_LightColor;

void main()
{
	vec4 matcolor = texture2D( u_Texture, v_UV );

	vec4 lightcolor = u_LightColor;
	lightcolor = vec4(1,1,1,1);

	vec3 lightpos = vec3(0,2,0);

	vec3 dirtolight = normalize(lightpos - v_WorldPosition);

	float diffuseperc = dot(dirtolight, v_Surfacenormal);

	if(diffuseperc < 0)
		diffuseperc = 0;

	lightcolor *= diffuseperc;
	matcolor *= diffuseperc;

	matcolor *= lightcolor;
	gl_FragColor = vec4(matcolor.xyz, texture2D( u_Texture, v_UV ).a);
}
