#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <algorithm>

// sphere object
struct Sphere {
	char sName[20];
	// position x,y,z
	float x;
	float y;
	float z;
	// scale x,y,z
	float sx;
	float sy;
	float sz;
	// colour r,g,b
	float r;
	float g;
	float b;
	// local illumination
	float ka;
	float kd;
	float ks;
	float kr;
	// specular exponent
	int n;
	glm::mat4 transformMat;
};

// viewing plane object
struct ViewingPlane {
	float near;
	float left;
	float right;
	float top;
	float bottom;
};

// screen resoultion object
struct ScreenResolution {
	unsigned int x;
	unsigned int y;
};

// light source object
struct LightSource {
	// light source name
	char lName[20];
	// position x,y,z
	float x;
	float y;
	float z;
	// intensity of light source
	float ir;
	float ig;
	float ib;
};

// back colour object
struct Background {
	float r;
	float g;
	float b;
};

// scene's ambient intensity object
struct Ambient {
	float ir;
	float ig;
	float ib;
};

// output file object
struct OutputFile {
	char fName[20];
};

// object to store variables related to intersection of ray with sphere
struct Intersection {
	float a;
	float b;
	float c;
	float x1;
	float x2;
	Sphere sphere;
	bool intialized;
	bool colideInSphere;
};

// object to store variables related to rays cast
struct Ray {
	glm::vec4 origin;
	glm::vec4 direction;
	bool colided;
	int numReflections;
};


int setUp(char filename[]);
glm::vec3 applyLighting(Ray ray);
Ray calculateRayCollides(Ray ray);
void preCalculateSphereTransformMats();

void invert_matrix(double A[4][4], double Ainv[4][4]);

void save_imageP6(int Width, int Height, char* fname, unsigned char* pixels);
void save_imageP3(int Width, int Height, char* fname, unsigned char* pixels);