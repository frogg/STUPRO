#version 110

// Standard radius of globe
uniform float globeRadius;

// Factor of heightmap exaggeration
uniform float heightFactor;

// Size of the input plane in X/Y directions
uniform float planeSize;

// Starting/ending latitude/longitude values (lat=-90~+90; long=-180~+180)
uniform float latStart;
uniform float latEnd;
uniform float longStart;
uniform float longEnd;
/*
float latStart = -90.0;
float latEnd = 90.0;
float longStart = -180.0;
float longEnd = 180.0;
*/

// Interpolation value, determines whether globe or map is displayed (or something in between)
uniform float displayMode;

// Texture to sample the height from (alpha channel).
uniform sampler2D heightTexture;

// Various mathematical constants.
const float pi = 3.14159265;
const float twoPi = 2.0 * pi;
const float halfPi = 0.5 * pi;

// Converts a lat/long flat position into a x/y/z globe position.
vec3 transformLatLong(float lon, float lat, float height)
{
	lat = lat * halfPi / planeSize;
	lon = lon * twoPi / planeSize;
	
	float rad = (height * heightFactor + globeRadius);
	float cosLat = cos(lat);
	float sinLat = sin(lat);
	float cosLon = cos(lon);
	float sinLon = sin(lon);
	float x = -rad * cosLat * cosLon;
	float y =  rad * sinLat;
	float z =  rad * cosLat * sinLon;
	
	return vec3(x, y, z);
}

// Main function
void propFuncVS()
{
	// Normalize texture coordinates.
	gl_TexCoord[0].xy = vec2(gl_Vertex.x / planeSize + 0.5, gl_Vertex.y / planeSize + 0.5);
	
	// Get height value from the alpha channel of the texture.
	float heightSample = texture2D(heightTexture, gl_TexCoord[0].xy).a;
	
	// Calculate lat-long subrectangle transformation.
	vec2 inPos = gl_Vertex.xy / planeSize + vec2(0.5, 0.5);
	inPos.x *= (longEnd - longStart) / 360.0;
	inPos.x += longStart / 360.0;
	//inPos.x = 1.0 - inPos.x;
	inPos.y *= (latEnd - latStart) / 180.0;
	inPos.y += latStart / 180.0;
	//inPos.y = 1.0 - inPos.y;
	
	// Calculate position for single globe/map vertex.
	vec3 transformedPos = transformLatLong(inPos.x + 0.25, inPos.y * 2.0, heightSample);
	vec3 flatPos = vec3((inPos) * vec2(4.0 * globeRadius, 2.0 * globeRadius), heightSample * heightFactor);
	//vec3 flatPos = vec3(inPos, 0.0);
	
	// Linear interpolation between map and globe positions.
	vec3 pos = mix(transformedPos, flatPos, displayMode);
	//vec3 pos = flatPos;
	
	// Transform vertex position by camera matrix.
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(pos.xyz, 1);
	
	// Use constant shading for now.
	//float shading = gl_Vertex.y + 0.5;
	//gl_FrontColor = vec4(shading, shading, shading, 1.0);
	gl_FrontColor = vec4(gl_Vertex.x + 0.5, gl_Vertex.y + 0.5, 0.0, 1.0);
	//gl_FrontColor = vec4(1.0, 1.0, 1.0, 1.0);
}

