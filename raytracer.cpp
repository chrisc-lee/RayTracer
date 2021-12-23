#include <raytracer.h>
// list of spheres created from text file
std::vector <Sphere> spheres;

// list of light sources created from text file
std::vector <LightSource> lightSources;

// global instatiation of objects
ViewingPlane viewingPlane;
ScreenResolution screenResolution;
Background background;
Ambient ambient;
OutputFile outputFile;
Intersection quadraticEqn1;

int orginalSphereCount = 0;
int prevSphereCount = 0;

int main(int argc, char* argv[])
{
	// must be exactly two arguments
	if (argc != 2) {
		std::cout << "\nInvalid number of arguments. Arguments should be of the following form: raytracer.exe <filename>.txt\n";
		return -1;
	}

	// read information from file and store information in structs
	if (setUp(argv[1]) < 0) {
		return -1;
	}

	std::cout << "Rendering Scene using parameters from: " << argv[1] << "\n";
	
	// set up an array to keep track of rgb values of each pixel on screen
	unsigned char* pixels;
	pixels = new unsigned char[3 * screenResolution.x * screenResolution.y];

	// ratio of width to height of screen
	float width = ((float(screenResolution.x) / screenResolution.y) + 1.0f) / 2.0f;
	float height = ((float(screenResolution.y) / screenResolution.x) + 1.0f) / 2.0f;

	// precalculate sphere transformation matrices
	preCalculateSphereTransformMats();


	// counter for rgb triple
	int k = 0;
	glm::vec3 pixelColour = glm::vec3(0.0f, 0.0f, 0.0f);
	for (int i = screenResolution.y - 1; i >= 0; i--) {
		for (int j = 0; j < screenResolution.x; j++) {
			// create ray 
			// ray(r,c,t) = eye + t(-Nn + W(2c/nCols - 1)u + H(2r/nRows - 1)v)
			// eye here is fixed at (0,0,0)
			Ray ray = { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4((width * (2.0f * j / screenResolution.x) - 1.0f), (height * ((2.0f * i / screenResolution.y) - 1.0f)), (-1.0f * viewingPlane.near), (0.0f)), false, 0};
			quadraticEqn1.intialized = false;
			ray = calculateRayCollides(ray);
			// if no collision draw the background colour
			if (ray.colided == false) {
				pixelColour = glm::vec3(background.r * 255.0f, background.g * 255.0f, background.b * 255.0f);
			}
			// if there is a collision we need to calculate the colour of the object based on the lightsources and ambient
			else {
				// apply lighting model
				pixelColour = applyLighting(ray);
			}
			
			pixels[k] = std::min(255.0f, pixelColour.x); // r
			pixels[k + 1] = std::min(255.0f, pixelColour.y); // g
			pixels[k + 2] = std::min(255.0f, pixelColour.z); // b
			
			k = k + 3; // go to next triplet
		}
	}


	//save_imageP3(screenResolution.x, screenResolution.y, outputFile.fName, pixels);
	save_imageP6(screenResolution.x, screenResolution.y, outputFile.fName, pixels);
	return 0;
}

// read input from file and instantiate necessary objects based on file input
int setUp(char filename[]) {
	FILE* file;
	char line[1024];
	
	file = fopen(filename, "r");
	if (!file) {
		printf("Unable to open file '%s'\n", filename);
		return -1;
	}
	// get parameters from file
	while (fgets(line, 1024, file)) {
		char* split;
		split = strtok(line, " ");
		int numSpheresCreated = 0;
		int numLightSourcesCreated = 0;
		while (split != NULL)
		{
			if (strcmp(split, "NEAR") == 0) {
				split = strtok(NULL, " ");
				viewingPlane.near = atof(split);
			}
			if (strcmp(split, "LEFT") == 0) {
				split = strtok(NULL, " ");
				viewingPlane.left = atof(split);
			}
			if (strcmp(split, "RIGHT") == 0) {
				split = strtok(NULL, " ");
				viewingPlane.right = atof(split);
			}
			if (strcmp(split, "BOTTOM") == 0) {
				split = strtok(NULL, " ");
				viewingPlane.bottom = atof(split);
			}
			if (strcmp(split, "TOP") == 0) {
				split = strtok(NULL, " ");
				viewingPlane.top = atof(split);
			}
			if (strcmp(split, "RES") == 0) {
				split = strtok(NULL, " ");
				screenResolution.x = atoi(split);
				split = strtok(NULL, " ");
				screenResolution.y = atoi(split);
			}
			if (strcmp(split, "SPHERE") == 0) {
				split = strtok(NULL, " ");
				// create up to 15 spheres
				for (int i = 0; i < 15; i++) {
					if (numSpheresCreated == i) {
						Sphere sphere1;
						strcpy(sphere1.sName, split);
						split = strtok(NULL, " ");
						sphere1.x = atof(split);
						split = strtok(NULL, " ");
						sphere1.y = atof(split);
						split = strtok(NULL, " ");
						sphere1.z = atof(split);

						split = strtok(NULL, " ");
						sphere1.sx = atof(split);
						split = strtok(NULL, " ");
						sphere1.sy = atof(split);
						split = strtok(NULL, " ");
						sphere1.sz = atof(split);

						split = strtok(NULL, " ");
						sphere1.r = atof(split);
						split = strtok(NULL, " ");
						sphere1.g = atof(split);
						split = strtok(NULL, " ");
						sphere1.b = atof(split);

						split = strtok(NULL, " ");
						sphere1.ka = atof(split);
						split = strtok(NULL, " ");
						sphere1.kd = atof(split);
						split = strtok(NULL, " ");
						sphere1.ks = atof(split);
						split = strtok(NULL, " ");
						sphere1.kr = atof(split);

						split = strtok(NULL, " ");
						sphere1.n = atoi(split);

						spheres.push_back(sphere1);
					}
				}
			}
			if (strcmp(split, "LIGHT") == 0) {
				split = strtok(NULL, " ");
				// create up to 10 light sources
				for (int i = 0; i < 10; i++) {
					if (numLightSourcesCreated == i) {
						LightSource lightsource1;
						strcpy(lightsource1.lName, split);
						split = strtok(NULL, " ");
						lightsource1.x = atof(split);
						split = strtok(NULL, " ");
						lightsource1.y = atof(split);
						split = strtok(NULL, " ");
						lightsource1.z = atof(split);

						split = strtok(NULL, " ");
						lightsource1.ir = atof(split);
						split = strtok(NULL, " ");
						lightsource1.ig = atof(split);
						split = strtok(NULL, " ");
						lightsource1.ib = atof(split);

						lightSources.push_back(lightsource1);
					}
				}
			}
			if (strcmp(split, "BACK") == 0) {
				split = strtok(NULL, " ");
				background.r = atof(split);
				split = strtok(NULL, " ");
				background.g = atof(split);
				split = strtok(NULL, " ");
				background.b = atof(split);
			}
			if (strcmp(split, "AMBIENT") == 0) {
				split = strtok(NULL, " ");
				ambient.ir = atof(split);
				split = strtok(NULL, " ");
				ambient.ig = atof(split);
				split = strtok(NULL, " ");
				ambient.ib = atof(split);
			}
			if (strcmp(split, "OUTPUT") == 0) {
				split = strtok(NULL, " ");
				strcpy(outputFile.fName, split);
			}
			split = strtok(NULL, " ");
		}
	}
	return 0;
}

// pre-calculate transfomation matrices for spheres
void preCalculateSphereTransformMats() {
	glm::mat4 inverseTransformMat;
	glm::mat4 transformMat;
	glm::mat4 identityMat = glm::mat4(1.0f);
	// no need to calculate if no scaling transformation
	for (int m = 0; m < spheres.size(); m++) {
		if (spheres[m].sx == 1 && spheres[m].sy == 1 && spheres[m].sz == 1) {
			inverseTransformMat = identityMat;
		}
		// transform mmatrix = translation * scaling
		else {
			transformMat = translate(identityMat, glm::vec3(spheres[m].x, spheres[m].y, spheres[m].z)) * scale(identityMat, glm::vec3(spheres[m].sx, spheres[m].sy, spheres[m].sz));
			inverseTransformMat = inverse(transformMat);
		}
		spheres[m].transformMat = inverseTransformMat;
	}
}

// calculate the objects our ray collides with
Ray calculateRayCollides(Ray ray) {
	
	for (int l = 0; l < spheres.size(); l++) {
		glm::vec4 inverseOrigin = { ray.origin.x - spheres[l].x, ray.origin.y - spheres[l].y, ray.origin.z - spheres[l].z , 0.0f };
		glm::vec4 inverseDirection = ray.direction;
		inverseDirection.w = 0.0f;

		if (spheres[l].sx != 1.0f || spheres[l].sy != 1.0f || spheres[l].sz != 1.0f) {

			inverseOrigin = spheres[l].transformMat * inverseOrigin;
			inverseDirection = spheres[l].transformMat * inverseDirection;
		}

		// a^2x + 2bx + c quadratic equation is - (b/a) +- (sqrt(b^2 -ac) / a) 
		// S is origin of ray and c is the direction of the ray vector
		// a = |c|^2
		// b = (S * C)
		// c = |S|^2 - 1
		// d = b^2 - a * c
		float a = dot(inverseDirection, inverseDirection);
		float b = dot(inverseDirection, inverseOrigin);
		float c = dot(inverseOrigin, inverseOrigin) - 1.0f;
		float d = powf(b, 2.0f) - a * c;

		// if d >= 0 we have a collision with an object d = 0 means one solution, d > 0 means two solutions
		// two solutions
		if (d > 0) {
			float x1 = (-b + sqrtf(d)) / a;
			float x2 = (-b - sqrtf(d)) / a;
			ray.colided = true;
			// if colision is behind the ray
			if (x1 < 0.0f && x2 < 0.0f) {
				ray.colided = false;
			}
			else if (quadraticEqn1.intialized == false) {
				quadraticEqn1 = { a, b, c, x1, x2, spheres[l], true };
			}
			// if there is already a collision we check if the collision is closer or not
			else {
				float curXTemp = std::min(x1, x2);
				float prevXTemp = std::min(quadraticEqn1.x1, quadraticEqn1.x2);
				if (curXTemp < prevXTemp) {
					quadraticEqn1 = { a, b, c, x1, x2, spheres[l], true };
				}
			}
		}
		// one solution
		else if (d == 0) {
			float x1 = (-b + sqrtf(d)) / a;
			ray.colided = true;
			// if colision is behind the ray
			if (x1 < 0.0f) {
				ray.colided = false;
			}
			else if (quadraticEqn1.intialized == false) {
				quadraticEqn1 = { a, b, c, x1, x1, spheres[l], true };
			}
			// if there is already a collision we check if the collision is closer or not
			else {
				if (x1 < quadraticEqn1.x1 || x1 < quadraticEqn1.x2) {
					quadraticEqn1 = { a, b, c, x1, x1, spheres[l], true };
				}
			}
		}
	}
	return ray;
}

// apply lighting for pixels out ray collides with
glm::vec3 applyLighting(Ray ray) {
	if (ray.numReflections > 3) {
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	glm::vec4 normalRay;
	glm::vec4 intersection;
	intersection = ray.direction * std::min(quadraticEqn1.x1, quadraticEqn1.x2);
	intersection.w = 1.0f;
	normalRay = intersection - glm::vec4(quadraticEqn1.sphere.x, quadraticEqn1.sphere.y, quadraticEqn1.sphere.z, 1.0f);
	normalRay = normalRay / glm::vec4(quadraticEqn1.sphere.sx * quadraticEqn1.sphere.sx, quadraticEqn1.sphere.sy * quadraticEqn1.sphere.sy, quadraticEqn1.sphere.sz * quadraticEqn1.sphere.sz, 1.0f);
	normalRay.w = 0.0f;
	normalRay = normalize(normalRay);
	Intersection originalSphereIntersection = quadraticEqn1;
	glm::vec3 lightColour = glm::vec3(0.0f, 0.0f, 0.0f);
	
	for (int m = 0; m < lightSources.size(); m++) {
		// create ray from sphere to light source and check if it intersects
		glm::vec4 intersectionLightRay = glm::vec4(lightSources[m].x, lightSources[m].y, lightSources[m].z, 0.0f) - intersection;
		intersectionLightRay.w = 0.0f;
		Ray lightRay = { glm::vec4(intersection + intersectionLightRay * 0.0001f), intersectionLightRay, false };
		lightRay.origin.w = 1.0f;
		lightRay = calculateRayCollides(lightRay);
		// apply local illumination
		if (!lightRay.colided) {
			intersectionLightRay = normalize(intersectionLightRay);
			glm::vec4 reflectionRay = normalize(2.0f * dot(intersectionLightRay, normalRay) * normalRay - intersectionLightRay);
			float dt = dot(intersection, intersection) - 1.0f;
			glm::vec4 cameraVector = (-1.0f * intersection) / sqrt(dt);

			// for diffuse reflection: resulting intensity = (n * l)
			// for specular reflection: resulting intensity = (n * l) + (r * v)^n
			float diffuse = dot(normalRay, intersectionLightRay);
			float specular = dot(reflectionRay, cameraVector);

			// apply diffuse reflection
			if (diffuse > 0.0f) {
				lightColour.x += originalSphereIntersection.sphere.kd * lightSources[m].ir * diffuse * originalSphereIntersection.sphere.r;
				lightColour.y += originalSphereIntersection.sphere.kd * lightSources[m].ig * diffuse * originalSphereIntersection.sphere.g;
				lightColour.z += originalSphereIntersection.sphere.kd * lightSources[m].ib * diffuse * originalSphereIntersection.sphere.b;
			}

			// apply specular reflection
			if (specular > 0.0f) {
				lightColour.x += originalSphereIntersection.sphere.ks * lightSources[m].ir * pow(specular, originalSphereIntersection.sphere.n);
				lightColour.y += originalSphereIntersection.sphere.ks * lightSources[m].ig * pow(specular, originalSphereIntersection.sphere.n);
				lightColour.z += originalSphereIntersection.sphere.ks * lightSources[m].ib * pow(specular, originalSphereIntersection.sphere.n);
			}
		}

	}
	if (quadraticEqn1.sphere.kr != 0.0f) {
		glm::vec4 reflectedRayDirection = -2.0f * dot(normalRay, ray.direction) * normalRay + ray.direction;
		Ray reflectedRay = { intersection, reflectedRayDirection, false, ray.numReflections + 1 };
		lightColour += applyLighting(reflectedRay) * originalSphereIntersection.sphere.kr;
		
	}
	// colour = colour_local + kre * colour_reflect * kre + ka*colour_refract)
	glm::vec3 pixelColour = glm::vec3((originalSphereIntersection.sphere.r * originalSphereIntersection.sphere.ka * ambient.ir + lightColour.x) * 255.0f, (originalSphereIntersection.sphere.g * originalSphereIntersection.sphere.ka * ambient.ig + lightColour.y) * 255.0f, (originalSphereIntersection.sphere.b * originalSphereIntersection.sphere.ka * ambient.ib + lightColour.z) * 255.0f);
	return pixelColour;
}
