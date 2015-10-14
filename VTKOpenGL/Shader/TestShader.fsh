#version 110

uniform sampler2D texture;

void propFuncFS()
{
	//gl_FragColor = gl_Color * texture2D(texture, gl_TexCoord[0].xy);
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    //glFragColor = gl_Color;
}
