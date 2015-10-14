#version 110

uniform sampler2D texture;

void propFuncFS()
{
	//gl_FragColor = vec4(gl_TexCoord[0].xy, 0.0, 1.0);
	gl_FragColor = gl_Color * vec4(texture2D(texture, gl_TexCoord[0].xy).rgb, 1.0);
    //gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    //glFragColor = gl_Color;
}
