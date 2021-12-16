#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <cstdint>
#include <memory>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <glm/gtx/string_cast.hpp>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "OBJ_Loader.h"
#include "glider.h"
#include "terrain.h"
#include "B_Ter.h"
#include "Camera.h"
#include "Flying_Object.h"
#//include "ter.h"
//#include <assimp/include/Importer.hpp>
//#include <assimp/include/scene.h>
//#include <assimp/include/postprocess.h>
//#include "mesh.h"


using std::string;
using std::ifstream;
using std::vector;
using std::istringstream;
using std::ios;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;


float tilt_x = 0;
float tilt_y = 0;

float angle_x = -1.6;
float angle_y = 0;

GLuint tex, tex2, tex3;
Camera camera(glm::vec3(0.0f, 150.0f, 3.0f)); // camera at global scope

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Glider glider;
Terrain terrain(400,2);
FO bat("bat.obj");
FO bird("bird.obj");
FO bird2("bird2.obj");

bool crashed = false;

int rnd(int min = 0, int max = 360)
{
	return min + (rand() % static_cast<int>(max - min + 1));
}
float FOaccumulator;
int birdDirections[65];
int birdHeight[65];
int birdStart[130];
int batDirections[35];
int batHeight[35];
int batStart[70];
int gliderDirections[3];
int gliderHeight[3];
int gliderStart[6];

void init_FOs() {
	for (int i = 0; i < 65; i++) {
		birdDirections[i] = rnd();
		birdHeight[i] = rnd(0, 25);
		birdStart[2 * i] = rnd(-500, 500);
		birdStart[(2 * i) + 1] = rnd(-500, 500);
	}

	for (int i = 0; i < 35; i++) {
		batDirections[i] = rnd();
		batHeight[i] = rnd(0, 25);
		batStart[2 * i] = rnd(-500, 500);
		batStart[(2 * i) + 1] = rnd(-500, 500);
	}

	for (int i = 0; i < 3; i++) {
		gliderDirections[i] = rnd();
		gliderHeight[i] = rnd(0, 25);
		gliderStart[2 * i] = rnd(-200, 200);
		gliderStart[(2 * i) + 1] = rnd(-200, 200);
	}
}

//BTer bter;
//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.mouseCallback(window, xpos, ypos);
}
bool wyr1, wyr2 ,wyr3, wyr4 = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod) 
{
	
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		
		if (key == GLFW_KEY_W) { tilt_x = -PI; wyr1 = false; }
		if (key == GLFW_KEY_S) { tilt_x = PI; wyr2 = false; }
		if (key == GLFW_KEY_A) { tilt_y = PI; wyr3 = false; }
		if (key == GLFW_KEY_D) { tilt_y = -PI; wyr4 = false; }
	}
	
	else if (action == GLFW_RELEASE) 
	{	
		
		if (key == GLFW_KEY_W) { tilt_x = PI; wyr1 = true;}
		if (key == GLFW_KEY_S) { tilt_x = -PI; wyr2 = true;}
		if (key == GLFW_KEY_A) { tilt_y = -PI; wyr3 = true;}
		if (key == GLFW_KEY_D) { tilt_y = PI; wyr4 = true;}

	}
	
	
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data()); //zawsze 0 (poziom mipmapy) i 4 (kolory) i 0
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	return tex;
} //powyzsze do kopiowania


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();

	glClearColor(0, 0.5, 1, 1); //Ustaw kolor czyszczenia bufora kolorów
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	
	glfwSetKeyCallback(window, key_callback);
	
	tex = readTexture("glider.png");
	tex2 = readTexture("Textures/rock2.png");
	tex3 = readTexture("Textures/Bird");

}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	glDeleteTextures(1, &tex);
}

void LoadObjModel(string filename)
{

	objl::Loader Loader;
	bool loadout = Loader.LoadFile(filename);

	if (loadout)
	{
		// Create/Open e1Out.txt
		std::ofstream file5(filename +"_Vertices.txt");
		std::ofstream file1(filename +"_Normals.txt");
		std::ofstream file2(filename +"_TexCoords.txt");
		std::ofstream file3(filename +"_Indices.txt");
		std::ofstream file(filename + "_Materials.txt");
		// Go through each loaded mesh and out its contents
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{

			objl::Mesh curMesh = Loader.LoadedMeshes[i];

			for (int j = 0; j < curMesh.Indices.size(); j += 1)
			{
				file5 << curMesh.Vertices[curMesh.Indices[j]].Position.X << ", " << curMesh.Vertices[curMesh.Indices[j]].Position.Y << ", " << curMesh.Vertices[curMesh.Indices[j]].Position.Z << ",\n";
			
				file1 << curMesh.Vertices[curMesh.Indices[j]].Normal.X << ", " << curMesh.Vertices[curMesh.Indices[j]].Normal.Y << ", " << curMesh.Vertices[curMesh.Indices[j]].Normal.Z << ",\n ";

				file2 << curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.X << ", " << 1 - curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.Y << ",\n";

			}

		}
		
		file.close();
		file1.close();
		file2.close();
		file3.close();
		file5.close();
	}
	else
	{
		cout << "Failed to Load OBJ. File.\n";
	}
}

void drawglider(glm::mat4 P, glm::mat4 V, glm::mat4 M, ShaderProgram* sp)
{	
	sp->use(); //Aktywuj program cieniujący
	glUniform1i(sp->u("tex"), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M)); //Załaduj do programu cieniującego macierz modelu

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 3, GL_FLOAT, false, 0, static_cast<void*>(glider.vertices.data()));

	glEnableVertexAttribArray(sp->a("texCoord"));
	glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, static_cast<void*>(glider.texCoords.data()));
	
	glm::vec4 cp = glm::vec4(camera.getPos(), 1.0);
	glUniform3fv(sp->u("CamPos"), 1, glm::value_ptr(cp));

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 3, GL_FLOAT, false, 0, static_cast<void*>(glider.normals.data()));

	glDrawArrays(GL_TRIANGLES, 0, glider.vertexCount);
	
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color"));
	glDisableVertexAttribArray(sp->a("normal"));


}
void drawTerrain(glm::mat4 P, glm::mat4 V, glm::mat4 M, ShaderProgram *sp)
{
	sp->use(); //Aktywuj program cieniujący
	glUniform1i(sp->u("tex"), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M)); //Załaduj do programu cieniującego macierz modelu

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 3, GL_FLOAT, false, 0, static_cast<void*>(terrain.Vertices.data()));

	glEnableVertexAttribArray(sp->a("texCoord"));
	glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, static_cast<void*>(terrain.TexCoords.data()));

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 3, GL_FLOAT, false, 0, static_cast<void*>(terrain.Normals.data()));

	glm::vec4 cp = glm::vec4(camera.getPos(), 1.0);
	glUniform3fv(sp->u("CamPos"), 1, glm::value_ptr(cp));

	glDrawArrays(GL_TRIANGLES, 0, terrain.vertexCount);

	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color"));
	glDisableVertexAttribArray(sp->a("normal"));
	
}
/*void drawBter(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
	spGlider->use(); //Aktywuj program cieniujący
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUniformMatrix4fv(spGlider->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(spGlider->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
	glUniformMatrix4fv(spGlider->u("M"), 1, false, glm::value_ptr(M)); //Załaduj do programu cieniującego macierz modelu

	glEnableVertexAttribArray(spGlider->a("vertex"));
	glVertexAttribPointer(spGlider->a("vertex"), 3, GL_FLOAT, false, 0, static_cast<void*>(bter.vertices.data()));

	glEnableVertexAttribArray(spGlider->a("texCoord"));
	glVertexAttribPointer(spGlider->a("texCoord"), 2, GL_FLOAT, false, 0, static_cast<void*>(bter.texCoords.data()));

	glEnableVertexAttribArray(spGlider->a("normal"));
	glVertexAttribPointer(spGlider->a("normal"), 3, GL_FLOAT, false, 0, static_cast<void*>(bter.normals.data()));

	glDrawArrays(GL_TRIANGLES, 0, bter.vertexCount);

	glDisableVertexAttribArray(spGlider->a("vertex"));
	glDisableVertexAttribArray(spGlider->a("color"));
	glDisableVertexAttribArray(spGlider->a("normal"));

}*/
void drawFOs(glm::mat4 P, glm::mat4 V, glm::mat4 M, ShaderProgram* sp)
{
	for (int i = 0; i < 65; i++) {
		M = glm::mat4(1.0f);
		M = glm::translate(M, glm::vec3((float)(birdStart[2 * i]), 200.f + birdHeight[i], (float)(birdStart[(2 * i) + 1])));
		M = glm::rotate(M, (float)(birdDirections[i]), glm::vec3(0.f, 1.f, 0.f));
		M = glm::scale(M, glm::vec3(0.01f, 0.01f, 0.01f));
		//M = glm::rotate(M, FOaccumulator * 0.1f, glm::vec3(0.f, 1.f, 0.f));
		M = glm::translate(M, glm::vec3(0.f, 0.f, FOaccumulator * 250.f));
		
		glBindTexture(GL_TEXTURE_2D, tex3);
		sp->use(); 
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
		glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V)); 
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M)); 

		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 3, GL_FLOAT, false, 0, static_cast<void*>(bird.vertices.data()));

		glEnableVertexAttribArray(sp->a("texCoord"));
		glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, static_cast<void*>(bird.texCoords.data()));

		glEnableVertexAttribArray(sp->a("normal"));
		glVertexAttribPointer(sp->a("normal"), 3, GL_FLOAT, false, 0, static_cast<void*>(bird.normals.data()));

		glDrawArrays(GL_TRIANGLES, 0, bird.vertexCount);

		glDisableVertexAttribArray(sp->a("vertex"));
		glDisableVertexAttribArray(sp->a("color"));
		glDisableVertexAttribArray(sp->a("normal"));
	}
	for (int i = 0; i < 35; i++) {
		M = glm::mat4(1.0f);
		M = glm::translate(M, glm::vec3((float)(batStart[2 * i]), 150.f + batHeight[i], (float)(batStart[(2 * i) + 1])));
		M = glm::rotate(M, (float)(batDirections[i]), glm::vec3(0.f, 1.f, 0.f));
		M = glm::scale(M, glm::vec3(0.01f, 0.02f, 0.01f));
		//M = glm::rotate(M, FOaccumulator * 0.1f, glm::vec3(0.f, 1.f, 0.f));
		M = glm::translate(M, glm::vec3(0.f, 0.f, FOaccumulator * 300.f));
		glBindTexture(GL_TEXTURE_2D, tex3);
		glUniform1i(sp->u("tex"), 0);
		spGlider->use(); 
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P)); 
		glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V)); 
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M)); 

		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 3, GL_FLOAT, false, 0, static_cast<void*>(bat.vertices.data()));

		glEnableVertexAttribArray(sp->a("texCoord"));
		glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, static_cast<void*>(bat.texCoords.data()));

		glEnableVertexAttribArray(sp->a("normal"));
		glVertexAttribPointer(sp->a("normal"), 3, GL_FLOAT, false, 0, static_cast<void*>(bat.normals.data()));

		glDrawArrays(GL_TRIANGLES, 0, bat.vertexCount);

		glDisableVertexAttribArray(sp->a("vertex"));
		glDisableVertexAttribArray(sp->a("color"));
		glDisableVertexAttribArray(sp->a("normal"));
	}
	glBindTexture(GL_TEXTURE_2D, tex);
	for (int i = 0; i < 3; i++) {
		M = glm::mat4(1.0f);
		M = glm::translate(M, glm::vec3((float)(gliderStart[2 * i]), 500.f + gliderHeight[i], (float)(gliderStart[(2 * i) + 1])));
		M = glm::rotate(M, -90.f, glm::vec3(1.f, 0.f, 0.f));
		M = glm::rotate(M, (float)(gliderDirections[i]), glm::vec3(0.f, 0.f, 1.f));
		M = glm::translate(M, glm::vec3(0.f, FOaccumulator * -4.f, 0.f));

		drawglider(P, V, M, sp);
	}
}

void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	
	glm::mat4 M, V, P;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	V = camera.getView();   
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex2);

	M = glm::mat4(1.0f);

	P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 1500.0f); 

	drawTerrain(P, V, M, spTerrain);
	//drawBter(P, V, M);
	
	glBindTexture(GL_TEXTURE_2D, tex);

	glm::mat4 GM = glm::mat4(1.0f);
	//GM = glm::translate(GM, glm::vec3(camera.getPos().x, camera.getPos().y, camera.getPos().z));
	GM = glm::rotate(GM, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)); // obrot do poziomej pozycji
	//GM = glm::rotate(GM, glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));
	GM = glm::scale(GM, glm::vec3(0.3));
	//GM = glm::translate(GM, glm::vec3(camera.getPos().x, camera.getPos().y+, camera.getPos().z));
	//GM = glm::rotate(GM, glm::radians(0.f), camera.getUp());
	//GM = glm::translate(GM, glm::vec3(0.0f, 0.0f, 20.0f));

	/*GM = camera.getView();
	GM = glm::translate(GM, glm::vec3(0.f, 0.f, 10.f));*/
	if (!crashed)
	{
		GM = glm::rotate(GM, -angle_y, glm::vec3(0.0f, 0.0f, 1.0f)); //przechylanie
		GM = glm::rotate(GM, angle_x, glm::vec3(1.0f, 0.0f, 0.0f)); //przechylanie
	}
	drawglider(P, glm::translate(glm::mat4(1.f), glm::vec3(0.f,-1.f, -5.f)), GM, spTerrain);
	
	
	drawFOs(P, V, GM, spGlider);

		
	glfwSwapBuffers(window); 
}


int main(void)
{

	//LoadObjModel("FFGLOBJ.obj");
	//ogLoad("terrain_small.obj");
	terrain.LoadHeightmap("iransmall.raw", 8, 873, 873);

	init_FOs();

	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno
	
	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}
	window = glfwCreateWindow(1600, 900, "OpenGL", NULL, NULL);  

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); 
	glfwSwapInterval(1); 

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); 

	glfwSetTime(0); //Wyzeruj licznik czasu
	while (!glfwWindowShouldClose(window)) 
	{

		camera.processInput(window, glfwGetTime(), crashed);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		angle_x += 0.25 * tilt_x * glfwGetTime(); 
		if (angle_x > -1.3) { angle_x = -1.3; }
		if (angle_x < -1.9) { angle_x = -1.9; }

		angle_y += 0.25 * tilt_y * glfwGetTime();
		if (angle_y > 0.5) { angle_y = 0.5; }
		if (angle_y < -0.5) { angle_y = -0.5; }


		if (wyr1 == true && tilt_x == PI && angle_x >= -1.6) { tilt_x = 0; angle_x = -1.6; wyr1 = false; }
		if (wyr2 == true && tilt_x == -PI && angle_x <= -1.6) { tilt_x = 0; angle_x = -1.6; wyr2 = false; }
		if (wyr3 == true && tilt_y == -PI && angle_y <= 0) { tilt_y = 0; angle_y = 0.0; wyr3 = false; }
		if (wyr4 == true && tilt_y == PI && angle_y >= 0) { tilt_y = 0; angle_y = 0.0; wyr4 = false; }

		FOaccumulator += glfwGetTime();
		glfwSetTime(0); 
		if (terrain.GetHeightAt(camera.getTarget()) > (camera.getTarget().y)) {
			crashed = true;
		}
		drawScene(window, angle_x, angle_y); 
		
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}
	
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}