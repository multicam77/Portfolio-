#version 150 

// values provided through vertex arrays
in   vec4 vPosition;
in   vec3 vNormal;
in   vec3 vTangent;
in   vec2 vTexCoord;

// output values to the fragment shader that will be interpolated per-fragment
out  vec3 normal;
out  vec3 LightDir;
out  vec3 CameraDir;
out  vec2 fTexCoord;

// constant values per primitive
uniform mat4 Model;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform vec4 CameraPosition;
uniform mat4 View;

void main()
{
	normal = normalize((Model * vec4(vNormal, 0.0)).xyz);
	vec3 tangent = normalize((Model * vec4(vTangent, 0.0)).xyz);
	vec3 position = (Model * vPosition).xyz;
	vec3 bino = cross(normal, tangent);
	mat3 itSpace = inverse(mat3(tangent, bino, normal));
	normal = itSpace * normal;
	LightDir = normalize(itSpace * (LightPosition.xyz - position.xyz));
	CameraDir = normalize(itSpace * (CameraPosition.xyz - position));

	fTexCoord = vTexCoord;

	gl_Position = Projection * View * Model * vPosition;
}
