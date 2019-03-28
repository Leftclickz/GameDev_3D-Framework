#version 120

struct PointLight
{
	vec3 pos;
	vec4 color;
	float attenuationFactor;
	float ambientCoefficient;
};

uniform PointLight u_Lights[5];

vec3 GLImplementation(vec3 objectpos, vec3 objectnormal, vec3 campos, vec4 matcolor, PointLight light, float shininess)
{

	//our distance and direction to our light
	float dist = distance(light.pos, objectpos);
	vec3 dirtolight = normalize(light.pos - objectpos);

	//diffuse
	float diffuseperc = max(0.0,dot(dirtolight, normalize(objectnormal)));
	vec3 diffuseLight = diffuseperc * matcolor.rgb * light.color.rgb;

	//specular
	vec3 surfaceToCamera = normalize(campos - objectpos);
	float specularperc = 0.0f;
	if (diffuseperc > 0.0f)
			specularperc = pow(max(0.0, dot(surfaceToCamera, reflect(-dirtolight, objectnormal))), shininess);
	vec3 specularLight = specularperc * matcolor.rgb * light.color.rgb;

	//ambient
	vec3 ambientLight = light.ambientCoefficient * matcolor.rgb * light.color.rgb;

	//attenuation
	float attenuation = 1.0 / (1.0 + (light.attenuationFactor) * pow(dist, 2));
	if (attenuation < 0.01)
		attenuation = 0.0;

	//linear color before gamma
	vec3 linearcolor = ambientLight + attenuation * (diffuseLight + specularLight);

	//gamma correction
    	vec3 gamma = vec3(1.0/1.0);
    	vec3 finalColor = vec3(pow(linearcolor, gamma));

	return finalColor;
}

vec4 ApplyLights(vec3 objectpos, vec3 objectnormal, vec3 campos, vec4 matcolor, PointLight light[5], float shininess)
{
	vec4 color = vec4(0,0,0,matcolor.a);
	for (int i = 0; i < 5; i++)
	{
		if (light[i].color.a > 0)
			color.rgb += GLImplementation(objectpos, objectnormal, campos, matcolor, light[i], shininess);
	}

	return color;
}
