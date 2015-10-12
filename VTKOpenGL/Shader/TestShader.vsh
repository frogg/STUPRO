#version 110

uniform float globeRadius;
uniform float planeSize;

float pi = 3.14159265;
float twoPi = 2.0 * pi;
float halfPi = 0.5 * pi;

vec3 transformLatLong(float lat, float lon)
{
	lat = lat * twoPi / planeSize;
	lon = lon * twoPi / planeSize;
	
	float cosLat = cos(lat);
	float sinLat = sin(lat);
	float cosLon = cos(lon);
	float sinLon = sin(lon);
	float x = globeRadius * cosLat * cosLon;
	float y = globeRadius * cosLat * sinLon;
	float z = globeRadius * sinLat;
	
	return vec3(x, y, z);
}

void propFuncVS()
{
	vec3 pos = transformLatLong(gl_Vertex.x, gl_Vertex.y);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(pos, 1);
	//float shading = pos.x / globeRadius / 2.0 + 0.5;
	float shading = 1.0;
	gl_FrontColor = vec4(shading, shading, shading, 1.0);
	gl_TexCoord[0].xy = vec2(gl_Vertex.x / planeSize + 0.5, gl_Vertex.y / planeSize + 0.5);
}

