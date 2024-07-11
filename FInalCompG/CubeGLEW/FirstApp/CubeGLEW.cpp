/* sierpinski gasket using vertex buffer objects */

#include "Angel.h"
#include "Shape.h"
#include "SOIL.h"

const int NumPoints = 10000;
int Index = 0;

GLuint currentShader;
int count = 0;
GLfloat aspectRatio = 1;
GLuint textureID0;
GLuint textureID1; 
GLuint globalTextureID0;
GLuint globalTextureID1;
int mode; 
vec2 offset;
vec2 tiling; 

//GLfloat rotation;
typedef struct Sphere
{
	int firstPoint;
	int numPoints;
	int pointsPerStrip;
	int numStrips;
	int mode;
};

int width, height;
GLfloat xw, yw;
Shape *current;
Shape *first, *second, *third;
vec4 yellow = vec4(1, 1, 0, 1);
vec4 magenta = vec4(1, 0, 1, 1);
//vec4 cyan = vec4(0, 1, 1, 1);
vec4 eye = { 0,5,5,1 };
vec4 at = { 0,0,0,1 };
vec4 eye2 = { 5, 2, 0, 1 };
GLfloat lensAngle = 30.0f;
//vec4 up = { 0.707f,0.707f,0,0 };
vec4 up = { 0,1,0,0 };
mat4 projection = mat4(1);
mat4 camera = mat4(1);
bool animate = true;

Viewport *viewport0, * viewport1, * viewport2;
Camera *camera0, * camera1;

GLuint startCube, startTetrahedron;




Sphere sphere;


point4 light_position(10.0, 0.0, 0.0, 1.0);
// Initialize shader lighting parameters
color4 light_ambient(0.1, 0.1, 0.1, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

color4 material_ambient(0.0215, 0.1745, 0.0215, 1.0);
color4 material_diffuse(0.07568, 0.61424, 0.07568, 1.0);
color4 material_specular(0.633, 0.727811, 0.633, 1.0);
float  material_shininess = 0.6;

color4 created_ambient(0.2125,	0.1275,	0.054, 1.0);
color4 created_diffuse(0.714,	0.4284,	0.18144, 1.0);
color4 created_specular(0.393548,	0.271906,	0.166721, 1.0);
float  created_shininess = 76.8;



color4 emerald_ambient(0.0215, 0.1745, 0.0215, 1.0);
color4 emerald_diffuse(0.07568, 0.61424, 0.07568, 1.0);
color4 emerald_specular(0.633, 0.727811, 0.633, 1.0);
float  emerald_shininess = 76.8;

color4 ruby_ambient(0.1745, 0.01175, 0.01175, 1.0);
color4 ruby_diffuse(0.61424, 0.04136, 0.04136, 1.0);
color4 ruby_specular(0.727811, 0.626959, 0.626959, 1.0);
float  ruby_shininess = 76.8;

color4 silver_ambient(0.19225,	0.19225,	0.19225, 1.0);
color4 silver_diffuse(0.19225,	0.19225,	0.19225, 1.0);
color4 silver_specular(0.508273,	0.508273,	0.508273, 1.0);
float  silver_shininess = 10.8;


color4 cyan_rubber_ambient(0.0, 0.05, 0.05, 1.0);
color4 cyan_rubber_diffuse(0.4, 0.5, 0.5, 1.0);
color4 cyan_rubber_specular(0.04, 0.7, 0.7, 1.0);
float  cyan_rubber_shininess = .078125 * 128;

color4 gold_ambient(0.24725, 0.1995, 0.0745, 1.0);
color4 gold_diffuse(0.75164, 0.60648, 0.22648, 1.0);
color4 gold_specular(0.628281, 0.555802, 0.366065, 1.0);
float  gold_shininess = .4 * 128;

typedef struct
{
	point4 position;
	vec3 rotation;
	vec3 scale;
} Transform;


/*
typedef struct
{
	point4 position;
	color4 diffuse;
	color4 specular;
	color4 ambient;
} LightStruct;

typedef struct
{
	point4 eye;
	point4 at;
	vec4 up;
} CameraStruct;
*/
Material emerald = { emerald_diffuse, emerald_specular, emerald_ambient, emerald_shininess };
Material ruby = { ruby_diffuse, ruby_specular, ruby_ambient, ruby_shininess };
Material cyan = { cyan_rubber_diffuse, cyan_rubber_specular, cyan_rubber_ambient, cyan_rubber_shininess };
Material gold = { gold_diffuse, gold_specular, gold_ambient, gold_shininess };
Material silver = { silver_diffuse, silver_specular, silver_ambient, silver_shininess };
Material created = { created_diffuse, created_specular, created_ambient, created_shininess }; 
Material *cMaterial;

void
drawTriangle()
{
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void
drawDiamond()
{
	glDrawArrays(GL_TRIANGLES, 3, 6);
}

void
drawCube()
{
	glDrawArrays(GL_TRIANGLES, startCube, 36);
}

void
drawTetrahedron()
{
	glDrawArrays(GL_TRIANGLES, startTetrahedron, 12);
}

point4 cubeVertices[8] = {
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0)
};

// RGBA colors
color4 cubeColors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

point4 tetrahedronVertices[4] = {
	point4(2.0, -3.0, 2.0, 1.0),
	point4(1.0, -1.0, -1.0, 1.0),
	point4(-1.0, 1.0, -1.0, 1.0),
	point4(-1.0, -1.0, 1.0, 1.0)
};

color4 tetrahedronColors[4] = {
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0)  // blue
};

point4 points[NumPoints];
color4 colors[NumPoints];
vec3 normals[NumPoints];
vec3 tangents[NumPoints];
vec2 texCoords[NumPoints];


// quad generates two triangles for each face and assigns colors
//    to the vertices
void quad(int a, int b, int c, int d)
{
	vec3 normal = -cross(cubeVertices[c] - cubeVertices[a], cubeVertices[b] - cubeVertices[a]);
	vec3 tangent = vec3(cubeVertices[c].x - cubeVertices[a].x, cubeVertices[c].y - cubeVertices[a].y, cubeVertices[c].z - cubeVertices[a].z);
	printf("%f %f %f %f\n", cubeVertices[a].x, cubeVertices[a].y, cubeVertices[a].z, cubeVertices[a].w);
	printf("%f %f %f %f\n", cubeVertices[b].x, cubeVertices[b].y, cubeVertices[b].z, cubeVertices[b].w);
	printf("%f %f %f %f\n", cubeVertices[c].x, cubeVertices[c].y, cubeVertices[c].z, cubeVertices[c].w);
	printf("%f %f %f %f\n", cubeVertices[d].x, cubeVertices[d].y, cubeVertices[d].z, cubeVertices[d].w);
	printf("\nnormal: x = %f, y = %f, z = %f.\n\n", normal.x, normal.y, normal.z);
	printf("\ntangent: x = %f, y = %f, z = %f.\n\n", tangent.x, tangent.y, tangent.z);
	colors[Index] = cubeColors[a]; points[Index] = cubeVertices[a]; normals[Index] = normal; tangents[Index] = tangent; Index++;
	colors[Index] = cubeColors[b]; points[Index] = cubeVertices[b]; normals[Index] = normal; tangents[Index] = tangent; Index++;
	colors[Index] = cubeColors[c]; points[Index] = cubeVertices[c]; normals[Index] = normal; tangents[Index] = tangent; Index++;
	colors[Index] = cubeColors[a]; points[Index] = cubeVertices[a]; normals[Index] = normal; tangents[Index] = tangent; Index++;
	colors[Index] = cubeColors[c]; points[Index] = cubeVertices[c]; normals[Index] = normal; tangents[Index] = tangent; Index++;
	colors[Index] = cubeColors[d]; points[Index] = cubeVertices[d]; normals[Index] = normal; tangents[Index] = tangent; Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

void
triangle(int a, int b, int c)
{
	vec3 normal = -cross(tetrahedronVertices[c] - tetrahedronVertices[a], tetrahedronVertices[b] - tetrahedronVertices[a]);
	vec3 tangent = vec3(tetrahedronVertices[c].x - tetrahedronVertices[a].x, tetrahedronVertices[c].y - tetrahedronVertices[a].y, tetrahedronVertices[c].z - tetrahedronVertices[a].z);
	colors[Index] = tetrahedronColors[a]; points[Index] = tetrahedronVertices[a]; normals[Index] = normal; tangents[Index] = tangent; Index++;
	colors[Index] = tetrahedronColors[b]; points[Index] = tetrahedronVertices[b]; normals[Index] = normal; tangents[Index] = tangent; Index++;
	colors[Index] = tetrahedronColors[c]; points[Index] = tetrahedronVertices[c]; normals[Index] = normal; tangents[Index] = tangent; Index++;
}


void
tetrahedron()
{
	triangle(0, 3, 1);
	triangle(0, 1, 2);
	triangle(0, 2, 3);
	triangle(1, 3, 2);
}

/*
Create a sphere centered at the origin, with radius r, and precision n
Draw a point for zero radius spheres
Use CCW facet ordering
"method" is 0 for quads, 1 for triangles
(quads look nicer in wireframe mode)
Partial spheres can be created using theta1->theta2, phi1->phi2
in radians 0 < theta < 2pi, -pi/2 < phi < pi/2
*/
Sphere CreateSphere(double r, int n, int method, double theta1, double theta2, double phi1, double phi2)
{
	Sphere sphere;
	int i, j;
	vec3 e, e2;
	vec4 p, p2;
	vec4 tangent, tangent2;
	double jdivn, j1divn, idivn, dosdivn, unodivn = 1 / (double)n, ndiv2 = (double)n / 2, t1, t2, t3, cost1, cost2, cte1, cte3;
	cte3 = (theta2 - theta1) / n;
	cte1 = (phi2 - phi1) / ndiv2;
	dosdivn = 2 * unodivn;
	/* Handle special cases */
	if (r < 0)
		r = -r;
	if (n < 0) {
		n = -n;
		ndiv2 = -ndiv2;
	}
	if (n < 4 || r <= 0) {
		points[Index] = vec4(0.0, 0.0, 0.0, 1.0);
		sphere.firstPoint = Index++;
		sphere.mode = GL_POINTS;
		sphere.numPoints = 1;
		sphere.numStrips = 1;
		sphere.pointsPerStrip = 1;
		return sphere;
	}

	//printf("<<<< New >>>>\n");
	t2 = phi1;
	cost2 = cos(phi1);
	j1divn = 0;
	sphere.firstPoint = Index;
	sphere.mode = method;
	sphere.numStrips = ndiv2;
	//printf("*** Start a planet. ***\n");
	for (j = 0; j < ndiv2; j++) {
		t1 = t2;//t1 = phi1 + j * cte1;
		t2 += cte1;//t2 = phi1 + (j + 1) * cte1;
		t3 = theta1 - cte3;
		cost1 = cost2;//cost1=cos(t1);
		cost2 = cos(t2);
		e.y = sin(t1);
		e2.y = sin(t2);
		p.y = r * e.y;
		p2.y = r * e2.y;
		tangent.y = -cost1;
		tangent2.y = -cost2;
		//vec4 lastPoint = p;

		//if (method == 0)
		//   glBegin(GL_QUAD_STRIP);
		//else
		//   glBegin(GL_TRIANGLE_STRIP);

		idivn = 0;
		jdivn = j1divn;
		j1divn += dosdivn;//=2*(j+1)/(double)n;
		for (i = 0; i <= n; i++) {
			//t3 = theta1 + i * (theta2 - theta1) / n;
			t3 += cte3;
			e.x = cost1 * cos(t3);
			//e.y = sin(t1);
			e.z = cost1 * sin(t3);
			p.x = r * e.x;
			//p.y = c.y + r * e.y;
			p.z = r * e.z;
			normals[Index] = e;
			p.w = 1.0;
			//tangent = p - lastPoint;
			//tangent = lastPoint - p;
			tangent.x = sin(t1) * cos(t3);
			tangent.z = sin(t1) * sin(t3);
			//lastPoint = p;
			tangents[Index] = vec3(tangent.x, tangent.y, tangent.z);//vec3(tempTangent.x, tempTangent.y, tempTangent.z);
			points[Index] = p;
			texCoords[Index].x = 1.0 - idivn;
			texCoords[Index].y = jdivn;
			Index++;
			//glNormal3f(e.x,e.y,e.z);
			//glTexCoord2f(idivn,jdivn);
			//glVertex3f(p.x,p.y,p.z);

			e2.x = cost2 * cos(t3);
			//e.y = sin(t2);
			e2.z = cost2 * sin(t3);
			p2.x = r * e2.x;
			//p.y = c.y + r * e.y;
			p2.z = r * e2.z;
			normals[Index] = e2;
			p2.w = 1.0;
			//tangent = p2 - lastPoint;
			//tangent = lastPoint - p2;
			tangent2.x = sin(t2) * cos(t3);
			tangent2.z = sin(t2) * sin(t3);
			//lastPoint = p2;
			tangents[Index] = vec3(tangent2.x, tangent2.y, tangent2.z);//vec3(tempTangent.x, tempTangent.y, tempTangent.z);
			points[Index] = p2;
			texCoords[Index].x = 1.0 - idivn;
			texCoords[Index].y = j1divn;
			Index++;
			//glNormal3f(e2.x,e2.y,e2.z);
			//glTexCoord2f(idivn,j1divn);
			//glVertex3f(p2.x,p2.y,p2.z);
			idivn += unodivn;
		}
		//glEnd();
	}
	sphere.numPoints = Index - sphere.firstPoint;
	sphere.pointsPerStrip = 2 * n + 2;
	return sphere;
}

void
drawSphere()
{
	int currentVertex = sphere.firstPoint;
	for (int i = 0; i < sphere.numStrips; i++)
	{
		glDrawArrays(sphere.mode, currentVertex, sphere.pointsPerStrip);
		currentVertex += sphere.pointsPerStrip;

	}
	
	/*glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(currentShader, "texture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glFlush();
	
	*/
	
}

//----------------------------------------------------------------------------
/* This function initializes an array of 3d vectors 
   and sends it to the graphics card along with shaders
   properly connected to them.
*/

void
init( void )
{
    // Specifiy the vertices for a triangle
	

	startCube = Index;
	colorcube();
	startTetrahedron = Index;
	tetrahedron();
	double size = 1.0;
	sphere = CreateSphere(size, 72, GL_TRIANGLE_STRIP, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2);

	globalTextureID0 = SOIL_load_OGL_texture
	(
		"texture.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO
		| SOIL_FLAG_MIPMAPS
		| SOIL_FLAG_INVERT_Y
		| SOIL_FLAG_MULTIPLY_ALPHA 
		| SOIL_FLAG_DDS_LOAD_DIRECT
	);

	globalTextureID1 = SOIL_load_OGL_texture
	(
		"img_test.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO
		| SOIL_FLAG_MIPMAPS
		| SOIL_FLAG_INVERT_Y
		| SOIL_FLAG_MULTIPLY_ALPHA
		| SOIL_FLAG_DDS_LOAD_DIRECT
	);

	// Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    //glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals) + sizeof(tangents) + sizeof(texCoords), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), sizeof(tangents), tangents);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals) + sizeof(tangents), sizeof(texCoords), texCoords);

    // Load shaders and use the resulting shader program
    //currentShader = InitShader( "simpleShader.vert", "simpleShader.frag" );
	//currentShader = InitShader("vertexShading.vert", "vertexShading.frag");
	//currentShader = InitShader("fragmentShading.vert", "fragmentShading.frag");
	currentShader = InitShader("shading.vert", "shading.frag");
	// make thIS shader the current shader
    glUseProgram(currentShader);

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPositionLocation = glGetAttribLocation(currentShader, "vPosition" );
    glEnableVertexAttribArray(vPositionLocation);
    glVertexAttribPointer(vPositionLocation, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	// Initialize the vertex color attribute from the vertex shader
	GLuint vColorPosition = glGetAttribLocation(currentShader, "vColor");
	glEnableVertexAttribArray(vColorPosition);
	glVertexAttribPointer(vColorPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	// Initialize the vertex normal attribute from the vertex shader
	GLuint vNormalPosition = glGetAttribLocation(currentShader, "vNormal");
	glEnableVertexAttribArray(vNormalPosition);
	glVertexAttribPointer(vNormalPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	// Initialize the vertex normal attribute from the vertex shader
	GLuint vTangentPosition = glGetAttribLocation(currentShader, "vTangent");
	glEnableVertexAttribArray(vTangentPosition);
	glVertexAttribPointer(vTangentPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors) + sizeof(normals)));

	GLuint vTexCoord = glGetAttribLocation(currentShader, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points) + sizeof(colors) + sizeof(normals) + sizeof(tangents)));
	// This enables hidden surface removal
	glEnable(GL_DEPTH_TEST);

	projection = Perspective(30, aspectRatio, 0.3, 20.0);
	camera = LookAt(eye, at, up);

	glClearColor( 0.5, 0.5, 0.5, 1.0 ); // gray background


	width = 512;
	height = 512;

	first = new Shape();
	first->shaderID = currentShader;
	first->color = yellow;
   //first->shapeF = drawCube;
   first->shapeF = drawSphere;
	first->projection = &projection;
	first->camera = &camera;
	//first->rotationSpeed = vec3(1.0f, 0.01f, 0.05f);
	//first->rotationSpeed = vec3(0);
	first->rotationSpeed = vec3(0.5, 0.5, 0.3);
	first->position = vec3(-2, 0, 0);
	first->textureID0 = globalTextureID0;
	first->material = emerald;
	first->scale = vec3(1.5, 1.5,1.5);
	

	//cMaterial = &emerald;
	//cMaterial = &gold;

	current = first;

	second = new Shape();
	second->shaderID = currentShader;
	second->color = yellow;
	//second->shapeF = drawTetrahedron;
	second->shapeF = drawSphere;
	second->projection = &projection;
	second->camera = &camera;
	second->rotationSpeed = vec3(0.2, 0.4, 0.1);
	second->position = vec3(2, 0, 0);
	second->textureID0 = globalTextureID1; 
	second->material = ruby;
	second->scale = vec3(0.5, 0.5, 0.5);

	camera0 = new Camera();
	camera0->eye = { 0, 5, 5, 1 };
	camera1 = new Camera();
	camera1->eye = { 5, 2, 0, 1 };

	



	
}

//----------------------------------------------------------------------------
/* This function handles the display and it is automatically called by GLUT
   once it is declared as the display function. The application should not
   call it directly.
*/

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clear the window


	// Send the material to the shader
/*	glUniform4fv(glGetUniformLocation(first->shaderID, "MaterialAmbient"), 1, &cMaterial->ambient.x);// ambient.x);
	glUniform4fv(glGetUniformLocation(first->shaderID, "MaterialDiffuse"), 1, &cMaterial->diffuse.x);
	glUniform4fv(glGetUniformLocation(first->shaderID, "MaterialSpecular"), 1, &cMaterial->specular.x);
	glUniform1f(glGetUniformLocation(first->shaderID, "Shininess"), cMaterial->shininess);
	*/
	 offset = (.1, .1);
	 tiling = (1, 1);
	glUniform1i(glGetUniformLocation(first->shaderID, "mode"), mode);
	glUniform2fv(glGetUniformLocation(first->shaderID, "offset"),1, &offset.x);
	glUniform2fv(glGetUniformLocation(first->shaderID, "tiling"), 1, &tiling.x); 
	
	// send the light color parameters to the shader
	glUniform4fv(glGetUniformLocation(first->shaderID, "LightAmbient"), 1, light_ambient);
	glUniform4fv(glGetUniformLocation(first->shaderID, "LightDiffuse"), 1, light_diffuse);
	glUniform4fv(glGetUniformLocation(first->shaderID, "LightSpecular"), 1, light_specular);
	glUniform4fv(glGetUniformLocation(first->shaderID, "LightPosition"), 1, light_position);

	//camera0->useCamera(viewport0, currentShader);
	viewport0->useViewport(currentShader);

	first->draw();
	second->draw();

	//camera1->useCamera(viewport1, currentShader);
	viewport1->useViewport(currentShader);
	first->draw();
	second->draw();

	viewport2->useViewport(currentShader);
	first->draw();
	second->draw();

	
	// flush the buffer
}

//----------------------------------------------------------------------------
/* This function handles the keyboard and it is called by GLUT once it is 
   declared as the keyboard function. The application should not call it
   directly.
*/

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:					// escape key
		glutPostRedisplay();
        exit( EXIT_SUCCESS );	// terminates the program
        break;
	case '1':
		current = first;
		
		//translation = current->position;
		//rotation = current->rotation;
		//scale = current->scale;
		break;
	case '2':
		current = second;
		//translation = current->position;
		//rotation = current->rotation;
		//scale = current->scale;
		break;
	case '3':
		current = third;
		//translation = current->position;
		//rotation = current->rotation;
		//scale = current->scale;
		break;
	case 'n':
		current = new Shape();
		current->shaderID = currentShader;
		current->color = yellow;
		current->shapeF = drawDiamond;
		break;
	case 'a':
		animate = !animate;
		break;
	case 'o': 
		
		mode = 1;
		break;
	case 'p':
		mode = 2;
		
		printf(" OOOOOOOOOOOOOOOOOOOOOOOOO\n");
		;
		break;
	default:
		//printf("Mouse Position: %i, %i\n", x, y);
		break;
    }
}

void
specialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		//printf("F1 - Mouse Position: %i, %i\n", x, y);
		camera1->lensAngle -= 0.5f;
		break;
	case GLUT_KEY_F2:
		camera1->lensAngle += 0.5f;
		break;
	case GLUT_KEY_UP:
		//printf("UP - Mouse Position: %i, %i\n", x, y);
		//eye.y *= 1.1f;
		break;
	case GLUT_KEY_DOWN:
		//eye.y *= 0.9f;
		break;
	case GLUT_KEY_RIGHT:
		//current->rotation.z -= 45;
		//rotation = RotateZ(angle);
		//eye.x *= 1.1f;
		printf("RIGHT - Mouse Position: %i, %i\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		eye.x *= 0.9f;
		break;
	case GLUT_KEY_HOME:
		printf("HOME - Mouse Position: %i, %i\n", x, y);
		break;
	default:
		break;
	}
}

void
mouse(int button, int state, int x, int y)
{
	xw = 2 * ((GLfloat)x / width) - 1;
	yw = 2 * (height - (GLfloat)y) / height - 1;
	printf("RMB - World Position: %f, %f\n", xw, yw);
	current->position.x = xw;
	current->position.y = yw;
	//translation = Translate(xw, yw, 0.0);
	switch (button)
	{
	case GLUT_RIGHT_BUTTON:
		glutPostRedisplay();
		printf("RMB - Mouse Position: %i, %i\n", x, y);
		printf("RMB - World Position: %f, %f\n", xw, yw);
		break;
	case GLUT_LEFT_BUTTON:
		//printf("LMB - Mouse Position: %i, %i\n", x, y);
		break;
	case GLUT_MIDDLE_BUTTON:
		//printf("MMB - Mouse Position: %i, %i\n", x, y);
		break;
	}
	switch (state)
	{
	case GLUT_UP:
		//printf("Button Up - Mouse Position: %i, %i\n", x, y);
		break;
	case GLUT_DOWN:
		//printf("Button Down - Mouse Position: %i, %i\n", x, y);
		break;
	}
}

void
mouseMove(int x, int y)
{
//	printf("MOVING - Mouse Position: %i, %i\n", x, y);
	xw = 2 * ((GLfloat)x / width) - 1;
	yw = 2 * (height - (GLfloat)y) / height - 1;
//	printf("RMB - World Position: %f, %f\n", xw, yw);
	current->position.x = xw;
	current->position.y = yw;
	//translation = Translate(xw, yw, 0.0);
}

//----------------------------------------------------------------------------
/* 
	This is the reshape callback
*/
void
reshapeFunction(int newWidth, int newHeight)
{
	width = newWidth;
	height = newHeight;
	viewport0 = new Viewport(0, 0, width / 2, height / 2, camera0);
	viewport1 = new Viewport(width / 2, height / 2, width / 2, height / 2, camera1);
	viewport2 = new Viewport(0, height / 2, width / 2, height / 2, camera1);
	aspectRatio = (GLfloat)width / (GLfloat)height;
	glViewport(0, 0, width, height);
}



void
mainMenu(int menuOption)
{
	
	switch (menuOption)
	{
	case 1:
		if (current != NULL)
		{
			current->material = gold;
			
		}
		break;
	case 2:
		if (current != NULL)
		{
			current->material = emerald;

		}
		break;
	case 3:
		if (current != NULL)
		{
			current->material = ruby;

		}
		break;
	case 4:
		if (current != NULL)
		{
			current->material = cyan;

		}
		break;
	case 5:
		if (current != NULL)
		{
			current->material = created;

		}
		break;
	case 6:
		if (current != NULL)
		{
			current->material = silver;

		}
		break;
	case 7:
		if (current != NULL)
		{ 
			if (mode == 1)
			{

				mode = 2;
			}
			else
			{
				mode = 1;
			}


	
			

		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}


void
createMenus()
{
	GLint mMenu = glutCreateMenu(mainMenu);
	glutAddMenuEntry("Gold", 1);
	glutAddMenuEntry("Emerald", 2);
	glutAddMenuEntry("Ruby", 3);
	glutAddMenuEntry("Cyan", 4);
	glutAddMenuEntry("Bronze",5);
	glutAddMenuEntry("Silver", 6);
	glutAddMenuEntry("Mode", 7);

	
	



	glutAttachMenu(GLUT_RIGHT_BUTTON);



}

//----------------------------------------------------------------------------
/*
	This will change variables for animation
*/
void
idleFunction()
{
	first->update();
	second->update();

	glutPostRedisplay();
}

void
frameFunction(int value)
{
	if (animate)
	{
		first->update();
		second->update();
	}

	glutPostRedisplay();
	glutTimerFunc(value, frameFunction, value);
}

//----------------------------------------------------------------------------
/* This is the main function that calls all the functions to initialize
   and setup the OpenGL environment through GLUT and GLEW.
*/

int
main( int argc, char **argv )
{
	// Initialize GLUT
    glutInit( &argc, argv );
	// Initialize the display mode to a buffer with Red, Green, Blue and Alpha channels
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH);
	// Set the window size
    glutInitWindowSize( 512, 512 );
	// Here you set the OpenGL version
    glutInitContextVersion( 3, 2 );
	//Use only one of the next two lines
    //glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitContextProfile( GLUT_COMPATIBILITY_PROFILE );
    glutCreateWindow( "Simple GLSL example" );

    // Uncomment if you are using GLEW
	glewInit(); 

    // initialize the array and send it to the graphics card
	init();
	createMenus();

    // provide the function that handles the display
	glutDisplayFunc( display );
	// provide the functions that handles the keyboard
    glutKeyboardFunc( keyboard );
	// provide the function for special keys handling
	glutSpecialFunc( specialKeys );
	// provide the function for idle handling
	//glutIdleFunc(idleFunction);
	// provide the function for frame timing
	glutTimerFunc(33, frameFunction, 33);
	// provide the function for the mouse handling
	glutMouseFunc(mouse);
	// provide the function for the mouse moving
	glutMotionFunc(mouseMove);
	// provide the callback function for window resizing
	glutReshapeFunc(reshapeFunction);

	// Calls create menu function
	

    // Wait for input from the user (the only meaningful input is the key escape)  Main loop should be last 
	glutMainLoop();


    return 0;
}