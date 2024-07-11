#version 150

in vec4 vPosition;
in vec4 vColor;

uniform mat4 Model;
uniform mat4 Projection;
uniform mat4 View;

out vec4 pColor;


void
main()
{
    gl_Position = Projection * View * Model * vPosition;
	pColor = vColor;
}
