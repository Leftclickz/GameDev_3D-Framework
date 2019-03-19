#version 120

uniform sampler2D u_Texture;

varying vec2 v_UV;
varying vec4 v_Color;

void main()
{
	//vec4 color = texture2D( u_Texture, v_UV );

	//color.a = 1.0f;
	//color += vec4(0.7, 0.4, 0, 0);
	gl_FragColor = v_Color;
}
