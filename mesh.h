#pragma once
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <assert.h>
#include <sstream>
#include <time.h>
#include <stdlib.h>
using namespace std;

class Vertex {
public:
	GLint vertIndex;
	GLint vertNorm;
	GLint textIndex;
};

class Face {
public:
	vector<Vertex> vertex;
	Face() {}
	~Face() {}
};

class Mesh {
public:
	vector<GLfloat> vVert; //vertex
	vector<GLfloat> vText; //texture
	vector<GLfloat> vNorm; //normal
	vector<Face> vFace; //faces index

	Mesh() {};
	~Mesh() {};
	bool readFile(char* path);
	vector<GLfloat> transform();
	vector<GLfloat> transformIndecise();
	vector<GLuint> indextrans();
};

bool Mesh::readFile(char* path) {
	ifstream file(path);
	if (!file) {
		cerr << "Error:: objectloader, could not open obj file:" << path << endl;
		return false;
	}
	string line;
	srand((unsigned)time(NULL));
	while (getline(file, line)) {
		if (line.substr(0, 2) == "vt") {    //vertex texture
			istringstream s(line.substr(2));
			/*vec2 v;
			s >> v.x >> v.y;
			v.y = -v.y;
			vText.push_back(v);*/
			GLfloat u, v;
			s >> u >> v;
			v = -v;
			vText.push_back(u); vText.push_back(v);
		}
		else if (line.substr(0, 2) == "vn") { //vertex normal
			istringstream s(line.substr(2));
			/*vec3 v;
			s >> v.x >> v.y >> v.z;
			vNorm.push_back(v);*/
			GLfloat u, v, w;
			s >> u >> v >> w;
			vNorm.push_back(u); vNorm.push_back(v); vNorm.push_back(w);
		}
		else if (line.substr(0, 1) == "v") {  //vertex
			istringstream s(line.substr(1));
			GLfloat u, v, w;
			s >> u >> v >> w;
			vVert.push_back(u); vVert.push_back(v); vVert.push_back(w);
			float c1 = rand() % 256 / (float)256, c2 = rand() % 256 / (float)256, c3 = rand() % 256 / (float)256;
			vVert.push_back(c1); vVert.push_back(c2); vVert.push_back(c3); //cout << c1 << "-" << c2 << "-" << c3 << endl;
		}
		else if (line.substr(0, 1) == "f") {  //faces
			istringstream vtns(line.substr(1));
			Face face;
			string vtn;
			while (vtns >> vtn) {
				Vertex vertex;
				replace(vtn.begin(), vtn.end(), '/', ' ');
				istringstream ivtn(vtn);
				if (vtn.find("  ") != string::npos) { //no texture info
					ivtn >> vertex.vertIndex >> vertex.vertNorm;
				}
				else {
					ivtn >> vertex.vertIndex >> vertex.textIndex >> vertex.vertNorm;
					vertex.textIndex--;
				}
				vertex.vertIndex--;
				vertex.vertNorm--;

				face.vertex.push_back(vertex);
				//cout << vertex.vertIndex<<"-";
			}
			vFace.push_back(face);
		}
		else {}
	}
	return true;
};

vector<GLfloat> Mesh::transform() {
	vector<GLfloat> final;
	GLint t;
	for (size_t i = 0; i < vFace.size(); i++) {
		for (size_t j = 0; j < 3; j++) {
			t = vFace[i].vertex[j].vertIndex;
			final.push_back(vVert[t * 6]);
			final.push_back(vVert[t * 6 + 1]);
			final.push_back(vVert[t * 6 + 2]);
			final.push_back(vVert[t * 6 + 3]);
			final.push_back(vVert[t * 6 + 4]);
			final.push_back(vVert[t * 6 + 5]);
		}
		if (vFace[i].vertex.size() == 4) {
			for (size_t j = 0; j < 3; j++) {
				t = j == 0 ? vFace[i].vertex[j].vertIndex : vFace[i].vertex[j + 1].vertIndex;
				final.push_back(vVert[t * 6]);
				final.push_back(vVert[t * 6 + 1]);
				final.push_back(vVert[t * 6 + 2]);
				final.push_back(vVert[t * 6 + 3]);
				final.push_back(vVert[t * 6 + 4]);
				final.push_back(vVert[t * 6 + 5]);
			}
		}
	}
	return final;
};

vector<GLfloat> Mesh::transformIndecise() {  //vertex without color, in ordering
	vector<GLfloat> final;
	GLfloat t;
	for (size_t i = 0; i < vVert.size(); i++) {
		if (i % 6 == 0 || i % 6 == 1 || i % 6 == 2) {
			t = vVert[i];
			final.push_back(t);
		}
	}
	return final;
};

vector<GLuint> Mesh::indextrans() {
	vector<GLuint> final;
	GLuint t;
	for (size_t i = 0; i < vFace.size(); i++) {
		for (size_t j = 0; j < 3; j++) {
			t = (GLuint)vFace[i].vertex[j].vertIndex;
			final.push_back(t);// *2);
		}
		if (vFace[i].vertex.size() == 4) {
			for (size_t j = 0; j < 3; j++) {
				t = (GLuint)(j == 0 ? vFace[i].vertex[j].vertIndex : vFace[i].vertex[j + 1].vertIndex);
				final.push_back(t);// *2);
			}
		}
	}
	return final;
};

#endif // !MESH_H
