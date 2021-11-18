#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh.h"
using namespace std;

Mesh mesh;

const int WID = 800; //window width
const int HGT = 600; //window height
int indexcount;

const char* vertexShaderSource = "#version 330 core\n"
"layout(location=0) in vec3 pos;\n"
"layout(location=1) in vec3 color;\n"
"out vec3 outColor;\n"
"void main(){\n"
"	gl_Position = vec4(pos, 1.0f);\n"
"	outColor = color;\n"
"}\n\0";

const char* vertexShaderSourceAdd = "#version 330 core\n"
"layout(location=0) in vec3 pos;\n"
"layout(location=1) in vec3 color;\n"
"uniform mat4 viewpoint;\n"
"uniform mat4 rotate;\n"
"uniform mat4 projection;\n"
"out vec3 outColor;\n"
"void main(){\n"
"	gl_Position = projection * viewpoint * rotate * vec4(pos, 1.0f);\n"
"	outColor = color;\n"
"}\n\0";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 outColor;\n"
"out vec4 fragcolor;\n"
"void main(){\n"
"	fragcolor = vec4(outColor,1.0f);\n"
"}\n\0";

const vector<float> vertexPositions = {

	//vertex                      //color
	-0.5f,-.25f,0.0f,             1.0f,0.0f,0.0f,
	0.5f,-.75f,0.0f,              0.0f,1.0f,0.0f,
	0.0f,0.5f,0.0f,               0.0f,0.0f,1.0f,

	-0.5f,-0.5f,0.0f,             0.0f,1.0f,0.0f,
	0.5f,-0.5f,0.0f,              1.0f,0.0f,0.0f,
	0.0f,-1.0f,0.0f,              0.0f,0.0f,1.0f
};

inline void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

inline void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void shaderFile(const GLchar* vertexPath, const GLchar* fragmentPath, string& vertcode, string& fragcode) {
	ifstream vshaderFile;
	ifstream fshaderFile;
	vshaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fshaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		vshaderFile.open(vertexPath);
		fshaderFile.open(fragmentPath);
		stringstream vshaderStream, fshaderStream;
		//read the file into data stream
		vshaderStream << vshaderFile.rdbuf();
		fshaderStream << fshaderFile.rdbuf();
		//close file
		vshaderFile.close();
		fshaderFile.close();
		//transform the stream int string
		vertcode = vshaderStream.str();
		fragcode = fshaderStream.str();
	}
	catch (ifstream::failure e) {
		cout << "Error:: shader:: file not succesfully read" << endl;
	}
};

GLuint CreateShader(GLenum eType, const string& strShaderFile) {
	int shaderHandle = glCreateShader(eType);    //create shader
	const char* strFileData = strShaderFile.c_str();
	glShaderSource(shaderHandle, 1, &strFileData, NULL);    //bind with GLSL shader
	glCompileShader(shaderHandle);      //compile

	GLint status;          //check the error
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shaderHandle, infoLogLength, NULL, infoLog);

		const char* strShaderType = NULL;
		switch (eType) {
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}
		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, infoLog);
		delete[] infoLog;
	}
	return shaderHandle;
}

GLuint CreateProgram(const vector<GLuint>& shaderList) {
	GLuint programHandle = glCreateProgram();
	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++) {
		glAttachShader(programHandle, shaderList[iLoop]);      //attach shaders and program
	}

	glLinkProgram(programHandle);      //link program

	GLint status;            //check if there is error
	glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint InfoLogLength;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &InfoLogLength);

		GLchar* InfoLog = new GLchar[InfoLogLength + 1];
		glGetProgramInfoLog(programHandle, InfoLogLength, NULL, InfoLog);
		fprintf(stderr, "Linkder failure: %s\n", InfoLog);
		delete[] InfoLog;
	}

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++) {
		glDetachShader(programHandle, shaderList[iLoop]);
	}
	return programHandle;
}

int VertexBuffer(GLuint& VAO, GLuint& VBO, GLuint& EBO, int flag, const vector<GLfloat>& vertices, const vector<GLuint>& verindex) {
	int sz = 0;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	switch (flag)
	{
	case 1:       //simple, without file
		glBufferData(GL_ARRAY_BUFFER, vertexPositions.size() * sizeof(float), &vertexPositions[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(3 * sizeof(GLfloat)));
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);
		sz = 12;
		break;
	case 2:        //file
		sz = vertices.size();
		glBufferData(GL_ARRAY_BUFFER, sz * sizeof(float), &vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		sz = sz / (float)3;   //vertices number
		break;
	case 3:        //file with EBO
		sz = vertices.size();
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ARRAY_BUFFER, sz * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexcount * sizeof(unsigned int), &verindex[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		sz = sz / (float)3;   //vertices number
		break;
	default:
		break;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return sz;
}

int main() {
	int flag = 1;
	cout << "Want to render directly? Or read from obj file?  1--Render anyway  2--read a file  3--read a file + EBO" << endl;
	cin >> flag;
	if (flag != 1) {
		char map[] = "D:\\UF Study\\Semester 2\\Computer Graphic\\Project2\\openglwarm\\data\\dragon.obj";
		mesh.readFile(map);
	}
	const vector<GLfloat> vertices = mesh.transform();
	//const vector<GLfloat> verticesOnly = mesh.transformIndecise();
	const vector<GLuint> verindex = mesh.indextrans();

	indexcount = verindex.size();

	//Initialize()
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WID, HGT, "objects", NULL, NULL);
	if (window == nullptr) {
		cout << "failed to create GLFW window." << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to initialize glad" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	//build and compile shaders
	const GLchar vertexPath[] = "source.vs";
	const GLchar fragmentPath[] = "source.fs";
	string vertexcode, fragmentcode;
	GLuint vertexShader, fragmentShader;
	shaderFile(vertexPath, fragmentPath, vertexcode, fragmentcode);
	if (flag != 1) {
		vertexShader = CreateShader(GL_VERTEX_SHADER, vertexcode);
		fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentcode);

	}
	else {
		vertexShader = CreateShader(GL_VERTEX_SHADER, vertexShaderSource);
		fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	}

	//program
	vector<GLuint> shaderList;
	shaderList.push_back(vertexShader);
	shaderList.push_back(fragmentShader);
	GLuint programHandle = CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	//Bind vertax buffer
	GLuint VAO, VBO, EBO;
	int siz;
	if (flag == 2)
		siz = VertexBuffer(VAO, VBO, EBO, flag, vertices, verindex);
	else
		siz = VertexBuffer(VAO, VBO, EBO, flag, mesh.vVert, verindex);

	//while loop to render
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.745f, 0.745f, 0.745f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programHandle);

		if (flag != 1) {
			glm::mat4 viewport;  //the perspective camera
			viewport = glm::lookAt(glm::vec3(0.0f, 0.0f, abs(vertices[2]) * 6), glm::vec3(0.0f, 0.0f, abs(vertices[2] * 3 - 1)), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 trans = glm::mat4(1.0f);   //rotation
			trans = glm::rotate(trans, 2 * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 projection = glm::mat4(1.0f);  //projection on the screen
			projection = glm::perspective(45.0f, (float)WID / (float)HGT, 0.1f, 100.0f);

			GLuint modelLoc = glGetUniformLocation(programHandle, "rotate");
			GLuint viewpointLoc = glGetUniformLocation(programHandle, "viewpoint");
			GLuint projectionLoc = glGetUniformLocation(programHandle, "projection");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(trans));
			glUniformMatrix4fv(viewpointLoc, 1, GL_FALSE, glm::value_ptr(viewport));
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			//changable color
			srand((unsigned int)time(NULL));//float timevalue = glfwGetTime();
			float valuearea_x = rand() % 256 / (float)256;//sin(timevalue) / 2.0f + .5f; //set color value between [0,1]
			float valuearea_y = rand() % 256 / (float)256;// cos(timevalue) / 2.0f + .5f; //set color value between [0,1]
			float valuearea_z = rand() % 256 / (float)256;
			int vertexColorLocation = glGetUniformLocation(programHandle, "ourColor");
			glUniform4f(vertexColorLocation, valuearea_x, valuearea_y, valuearea_z, 1.0f);
		}

		glBindVertexArray(VAO);
		if (flag == 3)
			glDrawElements(GL_TRIANGLES, indexcount, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, siz);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glUseProgram(0);

	glfwTerminate();
	return 0;
}