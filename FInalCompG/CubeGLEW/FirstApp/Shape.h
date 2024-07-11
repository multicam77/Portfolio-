#pragma once
#include "Angel.h"
#include "SOIL.h"

class Camera;

typedef vec4 color4;
typedef vec4 point4;

typedef struct
{
	color4 diffuse;
	color4 specular;
	color4 ambient;
	float shininess;
} Material;

class Viewport
{
public:
	GLint xPos;
	GLint yPos;
	GLint width;
	GLint height;
	Camera* camera;

	Viewport(GLint newXPos, GLint newYPos, GLint newWidth, GLint newHeight, Camera* newCamera);
	GLfloat aspectRatio();
	void useViewport(GLuint shaderID);

};

class Shape
{
public:
	vec2 offset;
	vec2 tiling;
	Material material;
	GLuint globalTextureID0;
	vec3 position;
	vec3 rotation;
	vec3 rotationSpeed;
	vec3 scale;
	GLuint shaderID;
	vec4 color;
	mat4 *projection;
	mat4 *camera;
	void (*shapeF)();
	GLuint textureID0;
	GLuint textureID1; 

	Shape();
	void draw();
	void update();
};

Shape::Shape()
{
    textureID0;
	
	position = vec3(0.0f);
	rotation = vec3(0.0f);
	scale = vec3(1.0f);
	color = vec4(1, 0, 0, 1);
	rotationSpeed = vec3(0, 0, 0);
	vec2 offset = (.10, .10);
	vec2 tiling = (1, 1);
}

void Shape::draw()
{
	mat4 pTransform = Translate(position);
	mat4 rzTransform = RotateZ(rotation.z);
	mat4 ryTransform = RotateY(rotation.y);
	mat4 rxTransform = RotateX(rotation.x);
	mat4 sTransform = Scale(scale);
	glUseProgram(shaderID);
	GLint uniformLocation = glGetUniformLocation(shaderID, "uColor");
	GLint modelLocation = glGetUniformLocation(shaderID, "Model");
	glUniform4fv(uniformLocation, 1, color);
	glUniformMatrix4fv(modelLocation, 1, true, pTransform * rzTransform * ryTransform * rxTransform * sTransform);

	// Send the material to the shader
	glUniform4fv(glGetUniformLocation(shaderID, "MaterialAmbient"), 1, &material.ambient.x);// ambient.x);
	glUniform4fv(glGetUniformLocation(shaderID, "MaterialDiffuse"), 1, &material.diffuse.x);
	glUniform4fv(glGetUniformLocation(shaderID, "MaterialSpecular"), 1, &material.specular.x);
	glUniform1f(glGetUniformLocation(shaderID, "Shininess"), material.shininess); 

	glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(shaderID, "texture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	shapeF();

	glDisable(GL_TEXTURE_2D); 

	

	
}

void Shape::update()
{
	rotation += rotationSpeed;
}

class Camera
{
public:
	GLfloat lensAngle;
	GLfloat zNear;
	GLfloat zFar;
	vec4 eye;
	vec4 at;
	vec4 up;

	Camera();
	void useCamera(Viewport *viewport, GLuint shaderID);
};

Camera::Camera()
{
	lensAngle = 45.0f;
	zNear = 0.3;
	zFar = 20.0;
	eye = vec4(0, 0, 10, 1);
	at = vec4(0, 0, 0, 1);
	up = vec4(0, 1, 0, 0);
}

void Camera::useCamera(Viewport* viewport, GLuint shaderID)
{
	//viewport->useViewport();
	mat4 projection = Perspective(lensAngle, viewport->aspectRatio(), zNear, zFar);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "Projection"), 1, GL_TRUE, projection);
	mat4 view = LookAt(eye, at, up);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "View"), 1, GL_TRUE, view);
	glUniform4fv(glGetUniformLocation(shaderID, "CameraPosition"), 1, eye);

}


Viewport::Viewport(GLint newXPos, GLint newYPos, GLint newWidth, GLint newHeight, Camera* newCamera)
{
	xPos = newXPos;
	yPos = newYPos;
	width = newWidth;
	height = newHeight;
	camera = newCamera;
}

GLfloat Viewport::aspectRatio()
{
	return (GLfloat)width / (GLfloat)height;
}

void Viewport::useViewport(GLuint shaderID)
{
	glViewport(xPos, yPos, width, height);
	camera->useCamera(this, shaderID);
}

