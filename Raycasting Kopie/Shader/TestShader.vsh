#version 110
//standard radius of globe
uniform float globeRadius;
//factor of exageration
uniform float heightOffset;

uniform float planeSize;
//interpolation value, determines whether the earth or the map is displayed (or something in between)
uniform float interpolation;
//texture where we get the height
uniform sampler2D heightTexture;

float pi = 3.14159265;
float twoPi = 2.0 * pi;
float halfPi = 0.5 * pi;

//radius change: elevation of the globe radius
vec3 transformLatLong(float lat, float lon, float radiusChange)
{
	lat = lat * pi / planeSize;
	lon = lon * twoPi / planeSize;
	
	float cosLat = cos(lat);
	float sinLat = sin(lat);
	float cosLon = cos(lon);
	float sinLon = sin(lon);
    float x = ((radiusChange*heightOffset)+globeRadius) * cosLat * cosLon;
	float y = ((radiusChange*heightOffset)+globeRadius) * cosLat * sinLon;
	float z = ((radiusChange*heightOffset)+globeRadius) * sinLat;
	
	return vec3(x, y, z);
}

//main function in OpenGL
void propFuncVS()
{
    //?
    gl_TexCoord[0].xy = vec2(gl_Vertex.y / planeSize + 0.5, gl_Vertex.x / planeSize + 0.5);
    //get height value in the alpha channel of the texture
	float heightSample = texture2D(heightTexture, gl_TexCoord[0].xy).a;
	//get globe position
    vec3 globePosition = transformLatLong(gl_Vertex.x, gl_Vertex.y, heightSample);
    //position on the flat map
    vec3 mapPostition = vec3(gl_Vertex.xy, -heightSample * heightOffset);
    //linear interpolation between new position  (pos) and old position (vec)
	vec3 pos = mix(globePosition, mapPostition , interpolation);
    //old OpenGl set glPosition (position of the vertex), mult for transformation of camera
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(pos.xyz, 1);
	float shading = 1.0;
	gl_FrontColor = vec4(shading, shading, shading, 1.0);
}

