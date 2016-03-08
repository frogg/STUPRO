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

// Upper and lower height limits (in meters)
uniform float minHeight;
uniform float maxHeight;

// Interpolation value, determines whether globe or map is displayed (or something in between)
uniform float displayMode;

// Texture to sample the height from (alpha channel).
uniform sampler2D heightTexture;

// Various mathematical constants.
const float pi = 3.14159265;
const float twoPi = 2.0 * pi;
const float halfPi = 0.5 * pi;

// Converts a lat/long flat position into a x/y/z globe position.
vec3 transformLatLong(float lon, float lat, float radius)
{
	lat = lat * halfPi;
	lon = lon * twoPi;
	
	float cosLat = cos(lat);
	float sinLat = sin(lat);
	float cosLon = cos(lon);
	float sinLon = sin(lon);
	float x = -radius * cosLat * cosLon;
	float y =  radius * sinLat;
	float z =  radius * cosLat * sinLon;
	
	return vec3(x, y, z);
}

// Main function
void propFuncVS()
{
	// Normalize texture coordinates.
	gl_TexCoord[0].xy = vec2(gl_Vertex.x / planeSize + 0.5, gl_Vertex.y / planeSize + 0.5);
	
	// Apply a tiny(!!!) bit of downscaling to fix tile boundaries.
	gl_TexCoord[0].xy /= 1.0001;
	
	// Get height value from the alpha channel of the texture.
	float heightSample = texture2D(heightTexture, gl_TexCoord[0].xy).a;
	float radius = max((heightSample * (maxHeight - minHeight) + minHeight) * heightFactor * globeRadius, 0.0);
	//float heightSample = 0.0;
	
	// Initialize input position
	vec2 inPos = gl_Vertex.xy / planeSize + vec2(0.5, 0.5);
	
	// Apply a tiny(!!!) bit of upscaling to fix tile boundaries.
	inPos *= 1.0001;
	
	// Calculate lat-long subrectangle transformation.
	inPos.x *= (longEnd - longStart) / 360.0;
	inPos.x += longStart / 360.0;
	//inPos.x = 1.0 - inPos.x;
	inPos.y *= (latEnd - latStart) / 180.0;
	inPos.y += latStart / 180.0;
	//inPos.y = 1.0 - inPos.y;
	
	// Calculate position for single globe/map vertex.
	vec3 transformedPos = transformLatLong(inPos.x + 0.25, inPos.y * 2.0, radius + globeRadius);
	vec3 flatPos = vec3((inPos) * vec2(4.0 * globeRadius, 2.0 * globeRadius), radius);
	//vec3 flatPos = vec3(inPos, 0.0);
	
	// Linear interpolation between map and globe positions.
	vec3 pos = mix(transformedPos, flatPos, displayMode);
	//vec3 pos = flatPos;
	
	// Transform vertex position by camera matrix.
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(pos.xyz, 1);
	
	// Use constant shading for now.
	//float shading = gl_Vertex.y + 0.5;
	//gl_FrontColor = vec4(shading, shading, shading, 1.0);
	//gl_FrontColor = vec4(gl_Vertex.x + 0.5, gl_Vertex.y + 0.5, 0.0, 1.0);
	gl_FrontColor = vec4(1.0, 1.0, 1.0, 1.0);
}

