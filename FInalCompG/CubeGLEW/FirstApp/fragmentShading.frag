#version 150

out vec4  fColor;
in vec3 fL;
in vec3 fE;
in vec3 fN;
in vec2 fTexCoord;
uniform sampler2D texture;

uniform vec4 LightAmbient, LightDiffuse, LightSpecular;
uniform vec4 MaterialAmbient, MaterialDiffuse, MaterialSpecular;
uniform float Shininess;


void
main()
{
	vec3 L = normalize(fL);
	vec3 E = normalize(fE);
	vec3 N = normalize(fN);
	vec3 H = normalize( L + E );

	vec4 ambient = MaterialAmbient * LightAmbient;
	float Kd = max( dot(L, N), 0.0);
	vec4 diffuse = Kd * LightDiffuse * MaterialDiffuse;
	float Ks = pow( max(dot(N, H), 0.0), Shininess );
	vec4 specular = Ks * MaterialSpecular * LightSpecular;
	if( dot(L, N) < 0.0)
	{
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}
	fColor = ambient + diffuse + specular;
	fColor.a = 1.0;
}
