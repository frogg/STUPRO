#version 110

// Input texture with earth's RGB data.
uniform sampler2D texture;

// Main function.
void propFuncFS()
{
	// Sample texture color, ignoring alpha channel (reserved for heightmap).
	//gl_FragColor = gl_Color * vec4(texture2D(texture, gl_TexCoord[0].xy).rgb, 1.0);
	gl_FragColor = gl_Color;
}
