#version 110
//standard radius of globe
uniform float globeRadius;
//factor of exageration
uniform float heightOffset;

uniform float planeSize;
uniform float interpolation;

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
    gl_TexCoord[0].xy = vec2(gl_Vertex.y / planeSize + 0.5, gl_Vertex.x / planeSize + 0.5);
	float heightSample = texture2D(heightTexture, gl_TexCoord[0].xy).a;
	vec3 pos = transformLatLong(gl_Vertex.x, gl_Vertex.y, heightSample);
	pos = mix(pos, vec3(gl_Vertex.xy, -heightSample * heightOffset), interpolation);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(pos.xyz, 1);
	float shading = 1.0;
	gl_FrontColor = vec4(shading, shading, shading, 1.0);
}

