#version 150 

// per-fragment interpolated values from the vertex shader
in  vec3 normal;
in  vec3 LightDir;
in  vec3 CameraDir;
in  vec2 fTexCoord;

out vec4 fColor;

vec2 center = vec2(0.5, 0.5);
float radius = 0.25;
float inner = 0.1;
float outer = 0.15;

uniform vec4 LightAmbient, LightDiffuse, LightSpecular;
uniform vec4 MaterialAmbient, MaterialDiffuse, MaterialSpecular;
uniform float Shininess;
uniform sampler2D texture;
uniform vec2 offset;
uniform vec2 tiling;

uniform int mode;

vec4 checkeredFlag(vec2 tCoord)
{
	if(tCoord.x > 0.5 && tCoord.y > 0.5 || tCoord.x < 0.5 && tCoord.y < 0.5)
	{
		return vec4(0, 0, 0, 1);
	}
	else
	{
		return vec4(1, 1, 1, 1);
	}
}

vec4
filledCircle(vec2 texCoord)
{
	float measure = distance(center, texCoord);
	if(measure < radius)
	{
		return vec4(0.1, 0.1, 0.1, 1.0);
	}
	else
	{
		return vec4(1.0, 1.0, 1.0, 1.0);
	}
}

vec4
Ring(vec2 texCoord)
{
	float measure = distance(center, texCoord);
	if(measure < outer && measure > inner)
	{
		return vec4(0.1, 0.1, 0.1, 1.0);
	}
	else
	{
		return vec4(1.0, 1.0, 1.0, 1.0);
	}
}

void main() 
{
	vec4 specular = vec4(0.0);
	float nxDir = max(0.0, dot(normalize(normal), normalize(LightDir)));
	vec4 diffuse = LightDiffuse * texture2D(texture,  (fTexCoord + offset) * tiling) * nxDir;
	if(mode == 2)
	{ 
	  diffuse = LightDiffuse * MaterialDiffuse * nxDir;

	}
	if(nxDir > 0.0)
	{
		vec3 halfVector = normalize(LightDir + CameraDir);
		float nxHalf = max(0.0, dot(normal, halfVector));
		float specularPower = pow(nxHalf, Shininess);
		specular = LightSpecular * specularPower;
	}
	vec4 ambient = LightAmbient * texture2D(texture,  (fTexCoord + offset) * tiling);
	if(mode == 2)
	{
	 ambient = LightAmbient * MaterialAmbient;
	 
	}


	
	fColor =  ambient + diffuse + specular;
} 

