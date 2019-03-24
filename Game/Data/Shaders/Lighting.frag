#version 120

uniform sampler2D u_Texture;

varying vec2 v_UV;
varying vec3 v_WorldPosition;

varying vec3 v_Surfacenormal;

varying vec3 v_LightPos;
uniform vec4 u_MaterialColor;
uniform vec4 u_LightColor;
uniform float u_LightRange;

uniform vec3 u_Campos;

void main()
{
	vec4 matcolor = texture2D( u_Texture, v_UV );

	vec4 lightcolor = u_LightColor;
	lightcolor = vec4(1,1,0,1);

	vec3 lightpos = vec3(0,8,0);

	float dist = distance(lightpos, v_WorldPosition);

	vec3 dirtolight = normalize(lightpos - v_WorldPosition);

	float diffuseperc = dot(dirtolight, normalize(v_Surfacenormal));


	vec3 simulatedlightpos = normalize(normalize(lightpos) + normalize(u_Campos));
	float specular = dot(normalize(v_Surfacenormal), simulatedlightpos);

	diffuseperc += specular;
	diffuseperc = max(0, diffuseperc);

	lightcolor *= diffuseperc;
	//matcolor *= diffuseperc;

	matcolor *= lightcolor;

	float range = 2;

	//something is wrong with this line
	float attenuation = max(0.5, pow(1 - (range / dist), 0.25));
	//float attenuation = max(0.5, pow(1 - (dist / range), 0.25));

	matcolor /= attenuation;
	//matcolor = normalize(matcolor);
	gl_FragColor = vec4(matcolor.xyz, texture2D( u_Texture, v_UV ).a);
}
