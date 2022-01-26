#include <CanvasTriangle.h>
#include <CanvasPoint.h>
#include <Colour.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include <RayTriangleIntersection.h>
#include <unordered_map>

using std::istringstream;
using std::string;
using std::ifstream;
using std::vector;
using std::array;
using std::cout;

#define WIDTH 640
#define HEIGHT 640
#define pi 3.14159265359
#define WIREFRAME 1
#define RASTERISE 2
#define RAYTRACE 3


float  depthMatrix[HEIGHT][WIDTH];

bool orb = false;
int rotation = 0;
int drawing = 1;
float focalLength;
bool basic = true;
bool gouraudDraw = false;
bool phongDraw = false;
glm::vec3 camPos(0.0, 0.0, 4.0);
glm::mat3 orientation = {
						  glm::vec3(1,0,0),
						  glm::vec3(0,1,0),
						  glm::vec3(0,0,1)
};

vector<glm::vec3> lights{
	glm::vec3(0.0, 0.45, 0.0),
	//glm::vec3(0.0, 0.45, 0.1),
	//glm::vec3(0.0, 0.45, -0.1),
	//glm::vec3(0.0,0.45,0.1),
	//glm::vec3(0.0,0.45,-0.1),
	//glm::vec3(0.1,0.45,0.0), // uncomment for soft shadows(extremely long time to compile on windows with cmake)
	//glm::vec3(-0.1,0.45,0.1),
	//glm::vec3(0.1,0.45,-0.1),
	//glm::vec3(0.1,0.45,0.1),
	//glm::vec3(-0.1,0.45,0.0),
	//glm::vec3(-0.1,0.45,-0.1)
};


void vertexNormals(std::vector<ModelTriangle>& triangle) {
	for (int i = 0; i < triangle.size(); i++) {
		ModelTriangle ti = triangle[i];
		std::vector<glm::vec3> normals;
		for (int v = 0; v < ti.vertices.size(); v++) {
			glm::vec3 vertex = ti.normal;
			int k = 1;
			for (int j = 0; j < triangle.size(); j++) {
				ModelTriangle tj = triangle[j];
				for (int u = 0; u < tj.vertices.size(); u++) {
					if (i != j && ti.vertices[v].x == tj.vertices[u].x && ti.vertices[v].y == tj.vertices[u].y && ti.vertices[v].z == tj.vertices[u].z) {
						if (acos(dot(ti.normal, tj.normal) / (length(ti.normal) * length(tj.normal))) < pi / 4) {
							vertex = vertex + tj.normal;
							k += 1;
						}
					}
				}
			}
			vertex = vertex / float(k);
			triangle[i].normals[v] = normalize(vertex);
		}
	}
}


vector<ModelTriangle> loadOBJ(string file_name, float scaleFactor, std::unordered_map<string, Colour> col) {
	ModelTriangle triangle;
	vector<ModelTriangle> triangles;
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	string colour;
	string line;
	ifstream inFile(file_name);

	while (getline(inFile, line)) {
		if (line == "") continue;
		vector<string> tokens = split(line, ' ');

		if (tokens[0] == "vn") {
			glm::vec3 normal(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
			normals.push_back(normal);
		}

		else if (tokens[0] == "v") {
			glm::vec3 vertex(stof(tokens[1]) * scaleFactor, stof(tokens[2]) * scaleFactor, stof(tokens[3]) * scaleFactor);
			vertices.push_back(vertex);
		}

		else if (tokens[0] == "f") {
			vector<string> l1 = split(tokens[1], '/');
			vector<string> l2 = split(tokens[2], '/');
			vector<string> l3 = split(tokens[3], '/');

			triangle.vertices[0] = vertices[stoi(l1[0]) - 1];
			triangle.vertices[1] = vertices[stoi(l2[0]) - 1],
			triangle.vertices[2] = vertices[stoi(l3[0]) - 1],
			triangle.colour = col[colour];

			if (!normals.empty()) {
				triangle.normals[0] = normals[stoi(l1[2]) - 1];
				triangle.normals[1] = normals[stoi(l2[2]) - 1];
				triangle.normals[2] = normals[stoi(l3[2]) - 1];
			}
			triangle.normal = glm::cross(glm::vec3(triangle.vertices[1] - triangle.vertices[0]), glm::vec3(triangle.vertices[2] - triangle.vertices[0]));
			triangles.push_back(triangle);
		}

		else if (tokens[0] == "usemtl") {
			colour = tokens[1];
		}
	}

	if (normals.empty()) {
		vertexNormals(triangles);
	}
	inFile.close();

	return triangles;
}


std::unordered_map<string, Colour> loadColours(string file_name) {
	std::unordered_map<string, Colour> col;
	string colour_name;
	string line;
	ifstream inFile(file_name);

	while (getline(inFile, line)) {
		if (line == "") continue;
		vector<string> tokens = split(line, ' ');

		if (tokens[0] == "newmtl") {
			colour_name = tokens[1];
		}

		else if (tokens[0] == "Kd") {
			Colour colour(int(stof(tokens[1]) * 255), int(stof(tokens[2]) * 255), int(stof(tokens[3]) * 255));
			col.insert({ colour_name, colour });
		}

		/*else if (tokens[0] == "map_Kd") {
			Colour colour = col[colour_name];
			colour.name = tokens[1];
			textures.insert({ tokens[1], TextureMap(tokens[1]) });
			colours[colour_name] = colour;
		}*/
	}
	inFile.close();

	return col;
}


std::unordered_map<string, Colour> colour = loadColours("D:/University/3rd Year/CG/RenderingEngine/colours.mtl");
vector<ModelTriangle> triangle = loadOBJ("D:/University/3rd Year/CG/RenderingEngine/cornell-box3.obj", 0.17, colour);


void vertices_sort(CanvasPoint& A, CanvasPoint& B, CanvasPoint& C) {
	if (A.y > B.y) {
		std::swap(A, B);
	}
	if (A.y > C.y) {
		std::swap(A, C);
	}
	if (B.y > C.y) {
		std::swap(B, C);
	}
}


vector<CanvasPoint> interpolatePoints(CanvasPoint from, CanvasPoint to) {
	vector<CanvasPoint> res;

	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	float zDiff = to.depth - from.depth;

	float numberOfSteps = std::max(std::max(fabs(xDiff), fabs(yDiff)), fabs(zDiff));

	float xStepSize = xDiff / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;
	float zStepSize = zDiff / numberOfSteps;

	for (float i = 0.0; i < numberOfSteps; i++) {
		float x = from.x + (xStepSize * i);
		float y = from.y + (yStepSize * i);
		float z = from.depth + (zStepSize * i);

		res.push_back(CanvasPoint(round(x), round(y), z));
	}

	return res;
}


void drawLine(DrawingWindow& window, CanvasPoint to, CanvasPoint from, Colour col) {
	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	float zDiff = to.depth - from.depth;

	float numberOfSteps = std::max(std::max(fabs(xDiff), fabs(yDiff)), fabs(zDiff));

	float xStepSize = xDiff / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;
	float zStepSize = zDiff / numberOfSteps;

	uint32_t colour = (255 << 24) + (col.red << 16) + (col.green << 8) + col.blue;

	for (float i = 0.0; i < numberOfSteps; i++) {
		int x = round(from.x + (xStepSize * i));
		int y = round(from.y + (yStepSize * i));
		float z = from.depth + (zStepSize * i);

		if (depthMatrix[x][y] <= 1.0 / z) {
			depthMatrix[x][y] = 1.0 / z;
			window.setPixelColour(x, y, colour);
		}
	}
}


void drawTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour col) {
	CanvasPoint a, b, c;
	a = triangle[0];
	b = triangle[1];
	c = triangle[2];

	drawLine(window, b, a, col);
	drawLine(window, c, b, col);
	drawLine(window, a, c, col);
}


void fill_triangle(DrawingWindow& window, vector<CanvasPoint> line1, vector<CanvasPoint> line2, Colour col) {
	int i = 0, j = 0;

	while (i < line1.size() && j < line2.size()) {
		if (line1[i].y == line2[j].y) {
			drawLine(window, line1[i], line2[j], col);
		}

		if (line1[i].y < line2[j].y)
			i++;
		else if (line1[i].y > line2[j].y)
			j++;
		else {
			i++;
			j++;
		}
	}
}


void drawRasterisedTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour col) {
	vector<CanvasPoint> line1;
	vector<CanvasPoint> line2;

	CanvasPoint p1 = triangle.v0();
	CanvasPoint p2 = triangle.v1();
	CanvasPoint p3 = triangle.v2();

	vertices_sort(p1, p2, p3);

	CanvasPoint mid = CanvasPoint(fabs(p1.x + (((p3.x - p1.x) * (p2.y - p1.y))) / (p3.y - p1.y)), p2.y, p1.depth + (((p3.depth - p1.depth) * (p2.y - p1.y))) / (p3.y - p1.y));
	line1 = interpolatePoints(p1, p2);
	line2 = interpolatePoints(p1, mid);
	fill_triangle(window, line1, line2, col);

	line1 = interpolatePoints(p2, p3);
	line2 = interpolatePoints(mid, p3);
	fill_triangle(window, line1, line2, col);

	drawLine(window, p1, p2, col);
	drawLine(window, p2, p3, col);
	drawLine(window, p3, p1, col);
}


CanvasPoint getCanvasIntersectionPoint(glm::vec3 cameraPosition, glm::vec3 vertexPosition, float focalLength) {
	CanvasPoint screenPos;
	float x, y, z, u, v;
	x = vertexPosition.x - cameraPosition.x;
	y = vertexPosition.y - cameraPosition.y;
	z = vertexPosition.z - cameraPosition.z;

	u = focalLength * round(-x * 650 / z) + (WIDTH / 2);
	v = focalLength * round(y * 650 / z) + (HEIGHT / 2);

	screenPos.x = u;
	screenPos.y = v;
	screenPos.depth = -z;

	return screenPos;
}


void lookAt() {
	glm::vec3 newForward = glm::normalize(camPos - glm::vec3(0, 0, 0));
	glm::vec3 newRight = glm::cross(glm::normalize(glm::vec3(0.0, 1.0, 0.0)), newForward);
	glm::vec3 newUp = glm::cross(newForward, newRight);

	orientation[0] = newRight;
	orientation[1] = newUp;
	orientation[2] = newForward;
}


void resetCamera() {
	glm::vec3 camPos(0.0, 0.0, 4.0);
	glm::mat3 orientation = {
						  glm::vec3(1,0,0),
						  glm::vec3(0,1,0),
						  glm::vec3(0,0,1)
	};
}


void orbit(bool orbit) {
	if (orbit) {
		float angle = -pi / 180;
		glm::mat3 mat = glm::mat3(
			cos(angle), 0.0, sin(angle),
			0.0, 1.0, 0.0,
			-sin(angle), 0.0, cos(angle)
		);
		camPos = camPos * mat;
		lookAt();
	}
}


//1
void RenderWireframe(DrawingWindow& window) {
	focalLength = 2.0;

	for (ModelTriangle i : triangle) {
		CanvasTriangle triangles;
		for (int j = 0; j < i.vertices.size(); j++) {
			glm::vec3 cameraToVertex = i.vertices[j] - camPos;
			glm::vec3 adjustedVector = cameraToVertex * orientation;
			i.vertices[j] = adjustedVector + camPos;

			float u = focalLength * round(-adjustedVector.x * 650 / adjustedVector.z) + (WIDTH / 2);
			float v = focalLength * round(adjustedVector.y * 650 / adjustedVector.z) + (HEIGHT / 2);

			triangles.vertices[j] = CanvasPoint(u, v);
		}

		drawTriangle(window, triangles, Colour(255, 255, 255));
	}
}


//2
void RenderResterised(DrawingWindow& window, glm::vec3& camPos) {
	for (int p = 0; p < WIDTH; p++) {
		for (int q = 0; q < HEIGHT; q++) {
			depthMatrix[p][q] = 0;
		}
	}

	focalLength = 2.0;

	for (ModelTriangle i : triangle) {
		for (int j = 0; j < i.vertices.size(); j++) {
			glm::vec3 cameraToVertex = i.vertices[j] - camPos;
			glm::vec3 adjustedVector = cameraToVertex * orientation;
			i.vertices[j] = adjustedVector + camPos;
		}

		CanvasPoint a, b, c;
		CanvasTriangle v;
		a = getCanvasIntersectionPoint(camPos, i.vertices[0], focalLength);
		b = getCanvasIntersectionPoint(camPos, i.vertices[1], focalLength);
		c = getCanvasIntersectionPoint(camPos, i.vertices[2], focalLength);

		v[0].x = a.x;
		v[0].y = a.y;
		v[1].x = b.x;
		v[1].y = b.y;
		v[2].x = c.x;
		v[2].y = c.y;

		v[0].depth = a.depth;
		v[1].depth = b.depth;
		v[2].depth = c.depth;

		drawRasterisedTriangle(window, v, i.colour);
	}
	cout << "finished";
}


RayTriangleIntersection getClosestIntersection(glm::vec3 camPos, glm::vec3 rayDirection, int old_index) {
	ModelTriangle i;
	RayTriangleIntersection point;
	glm::vec3 solution = glm::vec3(100, 100, 100);
	int index = -1;

	for (int t = 0; t < triangle.size(); t++) {
		i = triangle[t];
		glm::vec3 e0 = i.vertices[1] - i.vertices[0];
		glm::vec3 e1 = i.vertices[2] - i.vertices[0];

		glm::vec3 SPVector = camPos - i.vertices[0];
		glm::mat3 DEMatrix(-rayDirection, e0, e1);

		glm::vec3 possibleS = glm::inverse(DEMatrix) * SPVector;

		if ((possibleS.y >= 0) && (possibleS.y <= 1) && (possibleS.z >= 0) && (possibleS.z <= 1) &&
			((possibleS.y + possibleS.z) <= 1)) {
			if (possibleS.x >= 0) {
				if (possibleS.x < solution.x && t != old_index) {
					solution = possibleS;
					index = t;
				}
			}
		}
	}

	if (index > -1) {
		point.intersectionPoint = triangle[index].vertices[0] +
			solution.y * (triangle[index].vertices[1] - triangle[index].vertices[0]) +
			solution.z * (triangle[index].vertices[2] - triangle[index].vertices[0]);
		point.distanceFromCamera = solution.x;
		point.u = solution.y;
		point.v = solution.z;
		point.triangleIndex = index;
		point.intersectedTriangle = triangle[index];
	}
	else {
		point.intersectionPoint = glm::vec3(0, 0, 0);
		point.distanceFromCamera = -1;
		point.triangleIndex = 0;
		point.intersectedTriangle = triangle[0];
	}

	return point;
}


float gouraud(RayTriangleIntersection intersection, glm::vec3 light) {
	ModelTriangle triangle = intersection.intersectedTriangle;
	glm::vec3 light_ray = light - intersection.intersectionPoint;
	glm::vec3 camera_ray = (camPos * orientation) - intersection.intersectionPoint;
	float length = glm::length(light_ray);
	std::vector<float> brightnesses;

	for (int i = 0; i < triangle.normals.size(); i++) {
		float grade = glm::dot(triangle.normals[i], glm::normalize(light_ray));
		float brightness = 50 / (4 * pi * length * length);

		if (grade > 0) {
			brightness *= grade;
		}
		else {
			brightness *= 0;
		}

		glm::vec3 reflection_ray = glm::normalize(light_ray) - ((2.0f * triangle.normals[i]) * glm::dot(glm::normalize(light_ray), triangle.normals[i]));
		float specular = std::pow(glm::dot(glm::normalize(reflection_ray), glm::normalize(camera_ray)), 128);

		if (specular >= 0) {
			brightness += specular * 0.2;
		}

		brightnesses.push_back(brightness);
	}

	float finalBrightness = (1 - intersection.u - intersection.v) * brightnesses[0] + intersection.u * brightnesses[1] + intersection.v * brightnesses[2];

	if (finalBrightness > 1) {
		finalBrightness = 1;
	}
	if (finalBrightness < 0.2) {
		finalBrightness = 0.2;
	}

	return finalBrightness;
}

float phong(RayTriangleIntersection intersection, glm::vec3 light) {
	ModelTriangle triangle = intersection.intersectedTriangle;
	glm::vec3 light_ray = light - intersection.intersectionPoint;
	glm::vec3 camera_ray = (camPos * orientation) - intersection.intersectionPoint;
	glm::vec3 specLightRay = intersection.intersectionPoint - light;

	float length = glm::length(light_ray);

	glm::vec3 normal = (1 - intersection.u - intersection.v) * triangle.normals[0] + intersection.u * triangle.normals[1] + intersection.v * triangle.normals[2];

	float incidence_angle = glm::dot(glm::normalize(light_ray), glm::normalize(normal));
	float brightness = 50 / (4 * pi * length * length);

	glm::vec3 reflection_angle = glm::normalize(specLightRay) - (2.0f * glm::normalize(normal) * glm::dot(glm::normalize(specLightRay), glm::normalize(normal)));

	float specular = std::pow(glm::dot(glm::normalize(reflection_angle), glm::normalize(camera_ray)), 128);

	if (incidence_angle > 0) {
		brightness *= incidence_angle;
	}
	else {
		brightness *= 0;
	}

	if (specular >= 0) {
		brightness += specular * 0.2;
	}

	if (brightness > 1) {
		brightness = 1;
	}

	if (brightness < 0.2) {
		brightness = 0.2;
	}

	return brightness;
}


float get_brightness(glm::vec3 intersectionPoint, glm::vec3 normal, glm::vec3 light) {
	glm::vec3 light_ray = light - intersectionPoint;
	glm::vec3 camera_ray = (camPos * orientation) - intersectionPoint;
	float length = glm::length(light_ray);

	float incidence_angle = glm::dot(glm::normalize(light_ray), normal);
	float brightness = 50 / (4 * pi * length * length);

	glm::vec3 reflection_angle = glm::normalize(light_ray) - ((2.0f * normal) * glm::dot(glm::normalize(light_ray), normal));

	float specular = std::pow(glm::dot(glm::normalize(reflection_angle), glm::normalize(camera_ray)), 128);

	if (incidence_angle > 0) {
		brightness *= incidence_angle;
	}

	if (specular) {
		brightness += specular;
	}

	if (brightness > 1) {
		brightness = 1;
	}
	if (brightness < 0.2) {
		brightness = 0.2;
	}

	return brightness;
}

//3
void RenderRTX(DrawingWindow& window, glm::vec3 camPos, float focalLength) {

	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			double u = 2 * (x + 0.5) / WIDTH - 1;
			double v = 1 - 2 * (y + 0.5) / HEIGHT;

			glm::vec3 rayDirection = glm::vec3(u, v, focalLength) - camPos;
			rayDirection = glm::normalize(orientation * rayDirection);

			RayTriangleIntersection p = getClosestIntersection(camPos, rayDirection, -1);
			if (!isinf(p.distanceFromCamera)) {
				float bright = 0.0;
				float bright_mode;

				for (int i = 0; i < lights.size(); i++) {

					glm::vec3 rayDirection_light = lights[i] - p.intersectionPoint;
					rayDirection_light = glm::normalize(rayDirection_light);

					RayTriangleIntersection q = getClosestIntersection(p.intersectionPoint, rayDirection_light, p.triangleIndex);

					float light_dist = glm::dot(glm::length(p.intersectionPoint - lights[i]), glm::length(p.intersectionPoint - lights[i]));
					float triangle_dist = q.distanceFromCamera * q.distanceFromCamera;
					glm::vec3 normal = triangle.at(p.triangleIndex).normal;

					//check if it is in shadow
					if ((triangle_dist > light_dist) || (q.distanceFromCamera == -1)) {
						if (phongDraw)
							bright_mode = phong(p, lights[i]);
						else if (basic)
							bright_mode = get_brightness(p.intersectionPoint, normal, lights[i]);
						else
							bright_mode = gouraud(p, lights[i]);
					}
					else {
						bright_mode = 0.5;
					}
					bright += bright_mode;
				}
				bright = bright / lights.size();

				uint32_t colour = (255 << 24) + (int(bright * p.intersectedTriangle.colour.red) << 16) +
					(int(bright * p.intersectedTriangle.colour.green) << 8) +
					int(bright * p.intersectedTriangle.colour.blue);
				window.setPixelColour(x, y, colour);
			}
		}
	}
	cout << "finished";
}


void moveLights(int move, float amount) {
	for (int i = 0; i < lights.size(); i++) {
		if (move == 0) {
			lights[i].x += amount;
		}
		else if (move == 1) {
			lights[i].y += amount;
		}
		else {
			lights[i].z += amount;
		}

	}
}


void handleEvent(SDL_Event event, DrawingWindow& window) {
	if (event.type == SDL_KEYDOWN) {

		if (event.key.keysym.sym == SDLK_UP) camPos.y -= 0.1;
		else if (event.key.keysym.sym == SDLK_DOWN) camPos.y += 0.1;
		else if (event.key.keysym.sym == SDLK_LEFT) camPos.x += 0.1;
		else if (event.key.keysym.sym == SDLK_RIGHT) camPos.x -= 0.1;
		else if (event.key.keysym.sym == SDLK_s) camPos.z += 0.1;
		else if (event.key.keysym.sym == SDLK_w) camPos.z -= 0.1;
		else if (event.key.keysym.sym == SDLK_b) moveLights(1, -0.1); // test
		else if (event.key.keysym.sym == SDLK_n) moveLights(1, 0.1); // test
		else if (event.key.keysym.sym == SDLK_z) moveLights(0, -0.1); // test
		else if (event.key.keysym.sym == SDLK_x) moveLights(0, 0.1); // test
		else if (event.key.keysym.sym == SDLK_c) moveLights(2, -0.1); // test
		else if (event.key.keysym.sym == SDLK_v) moveLights(2, 0.1); // test
		// ROTATION
		else if (event.key.keysym.sym == SDLK_r) {
			float angle = pi / 180;
			glm::mat3 mat = glm::mat3(
				1.0, 0.0, 0.0,
				0.0, cos(angle), -sin(angle),
				0.0, sin(angle), cos(angle)
			);
			camPos = camPos * mat;
			lookAt();
		}
		else if (event.key.keysym.sym == SDLK_e) {
			float angle = -pi / 180;
			glm::mat3 mat = glm::mat3(
				1.0, 0.0, 0.0,
				0.0, cos(angle), -sin(angle),
				0.0, sin(angle), cos(angle)
			);
			camPos = camPos * mat;
			lookAt();
		}
		else if (event.key.keysym.sym == SDLK_f) {
			float angle = pi / 180;
			glm::mat3 mat = glm::mat3(
				cos(angle), 0.0, sin(angle),
				0.0, 1.0, 0.0,
				-sin(angle), 0.0, cos(angle)
			);
			camPos = camPos * mat;
			lookAt();
		}
		else if (event.key.keysym.sym == SDLK_g) {
			float angle = -pi / 180;
			glm::mat3 mat = glm::mat3(
				cos(angle), 0.0, sin(angle),
				0.0, 1.0, 0.0,
				-sin(angle), 0.0, cos(angle)
			);
			camPos = camPos * mat;
			lookAt();
		}
		// ORIENTATION ROTATION
		else if (event.key.keysym.sym == SDLK_i) {
			float angle = pi / 180;
			glm::mat3 mat = glm::mat3(
				glm::vec3(1.0, 0.0, 0.0),
				glm::vec3(0.0, cos(angle), -sin(angle)),
				glm::vec3(0.0, sin(angle), cos(angle))
			);
			orientation = orientation * mat;
		}
		else if (event.key.keysym.sym == SDLK_k) {
			float angle = -pi / 180;
			glm::mat3 mat = glm::mat3(
				glm::vec3(1.0, 0.0, 0.0),
				glm::vec3(0.0, cos(angle), -sin(angle)),
				glm::vec3(0.0, sin(angle), cos(angle))
			);
			orientation = orientation * mat;
		}
		else if (event.key.keysym.sym == SDLK_l) {
			float angle = -pi / 180;
			glm::mat3 mat = glm::mat3(
				glm::vec3(cos(angle), 0.0, sin(angle)),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(-sin(angle), 0.0, cos(angle))
			);
			orientation = orientation * mat;
		}
		else if (event.key.keysym.sym == SDLK_j) {
			float angle = pi / 180;
			glm::mat3 mat = glm::mat3(
				glm::vec3(cos(angle), 0, sin(angle)),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(-sin(angle), 0.0, cos(angle))
			);
			orientation = orientation * mat;
		}
		else if (event.key.keysym.sym == SDLK_o) {
			if (orb) {
				orb = false;
			}
			else orb = true;
		}
		else if (event.key.keysym.sym == SDLK_q) {
			lookAt();
		}
		else if (event.key.keysym.sym == SDLK_0) {
			resetCamera();
		}
		else if (event.key.keysym.sym == SDLK_1) {
			drawing = WIREFRAME;
		}
		else if (event.key.keysym.sym == SDLK_2) {
			drawing = RASTERISE;
		}
		else if (event.key.keysym.sym == SDLK_3) {
			drawing = RAYTRACE;
		}
		else if (event.key.keysym.sym == SDLK_4) {
			basic = true;
			gouraudDraw = false;
			phongDraw = false;
			std::cout << "basic" << std::endl;

			window.clearPixels();
			RenderRTX(window, camPos, -2);
		}

		else if (event.key.keysym.sym == SDLK_5) {
			basic = false;
			gouraudDraw = true;
			phongDraw = false;
			std::cout << "gouraud" << std::endl;

			window.clearPixels();
			RenderRTX(window, camPos, -2);
		}

		else if (event.key.keysym.sym == SDLK_6) {
			basic = false;
			gouraudDraw = false;
			phongDraw = true;
			std::cout << "phong" << std::endl;

			window.clearPixels();
			RenderRTX(window, camPos, -2);
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN) { window.savePPM("output.ppm"); }
}


int main(int argc, char* argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	vertexNormals(triangle);

	while (true) {
		if (window.pollForInputEvents(event)) handleEvent(event, window);

		//orbit(orb);

		window.clearPixels();

		if (drawing == WIREFRAME) {
			RenderWireframe(window);
		}
		else if (drawing == RASTERISE) {
			RenderResterised(window, camPos);
		}
		else if (drawing == RAYTRACE) {
			RenderRTX(window, camPos, -1.0);\
		}

		window.renderFrame();
	}
}