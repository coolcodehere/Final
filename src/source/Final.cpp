#include "loadShaders.h"
#include "camera.h"
#include <stb_image.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void generateTexture2D(unsigned char *data, int width, int height, int nrChannels);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 1000;

unsigned int VBO;
unsigned int cubeVAO;
unsigned int lightVAO;

unsigned int cubeShader;
unsigned int lampShader;

Camera camera(glm::vec3(0.0f, 0.0f, 3.5f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lightX, lightY = 1.0f, lightZ;
glm::vec3 lightPos;

bool rotFlg1;
bool rotFlg2;
float angle1;
float angle2;

unsigned int cubeTexture;

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RED;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void init(void)
{
	glEnable(GL_DEPTH_TEST);

	cubeShader = loadShader("../../src/shader/cube.vert", "../../src/shader/cube.frag");
	lampShader = loadShader("../../src/shader/lamp.vert", "../../src/shader/lamp.frag");

	float vertices[] = {
		// Vertex position    // Normals           //Texture
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  -1.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  -1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  -1.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -1.0f, -1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  -1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -1.0f, -1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  -1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  -1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  -1.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  -1.0f, -1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  -1.0f, -1.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  -1.0f, 1.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, -1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  -1.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, -1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, -1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, -1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  -1.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  -1.0f, -1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  -1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  -1.0f, 1.0f
	};

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	rotFlg1 = false;
	rotFlg2 = false;
	angle1 = 0.0f;
	angle2 = 0.0f;

	lightX = 1.0f;
	lightY = 0.1f;
	lightZ = 2.0f;

	cubeTexture = loadTexture("../../src/resources/textures/awesomeface.png");
	glUseProgram(cubeShader);
	glUniform1i(glGetUniformLocation(cubeShader, "cubeTexture"), 0);
}

void renderLamp(glm::mat4 projection, glm::mat4 view) {
	glUseProgram(lampShader);
	unsigned int projectionLoc = glGetUniformLocation(lampShader, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	unsigned int viewLoc = glGetUniformLocation(lampShader, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model = glm::mat4(1.0f);

	
	if (rotFlg2)
	{
		lightX = 1.0f * sin(glfwGetTime());
		lightY = 2.0f * (sin(glfwGetTime()) * 0.5) * cos(glfwGetTime());
		lightZ = -2.0f * cos(glfwGetTime());

		lightPos = glm::vec3(lightX, lightY, lightZ);
	}
	else {
		lightPos = glm::vec3(lightX, lightY, lightZ);
	}

	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
	unsigned int modelLoc = glGetUniformLocation(lampShader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void renderMainCube(glm::mat4 projection, glm::mat4 view) {
	glUseProgram(cubeShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);

	unsigned int objectColorLoc = glGetUniformLocation(cubeShader, "objectColor");
	glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);

	unsigned int lightColorLoc = glGetUniformLocation(cubeShader, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

	unsigned int lightPosLoc = glGetUniformLocation(cubeShader, "lightPos");
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

	unsigned int cameraPosLoc = glGetUniformLocation(cubeShader, "viewPos");
	glUniform3f(cameraPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

	unsigned int projectionLoc = glGetUniformLocation(cubeShader, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	unsigned int viewLoc = glGetUniformLocation(cubeShader, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model = glm::mat4(1.0f);

	if (rotFlg1)
	{
		angle1 = (float)glfwGetTime() / 2.0;
		model = glm::rotate(model, angle1, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else {
		model = glm::rotate(model, angle1, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	model = glm::scale(model, glm::vec3(1.0f));
	unsigned int modelLoc = glGetUniformLocation(cubeShader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


void renderSecondCube(glm::mat4 projection, glm::mat4 view) {
	glm::vec3 cubePos = glm::vec3(0, -0.5, 0);

	glUseProgram(cubeShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);

	unsigned int objectColorLoc = glGetUniformLocation(cubeShader, "objectColor");
	glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);

	unsigned int lightColorLoc = glGetUniformLocation(cubeShader, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

	unsigned int lightPosLoc = glGetUniformLocation(cubeShader, "lightPos");
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

	unsigned int cameraPosLoc = glGetUniformLocation(cubeShader, "viewPos");
	glUniform3f(cameraPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

	unsigned int projectionLoc = glGetUniformLocation(cubeShader, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	unsigned int viewLoc = glGetUniformLocation(cubeShader, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, cubePos);
	model = glm::scale(model, glm::vec3(10.0f, 0.1f, 10.0f));

	unsigned int modelLoc = glGetUniformLocation(cubeShader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


void display()
{	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();

	renderMainCube(projection, view);
	renderLamp(projection, view);
	renderSecondCube(projection, view);
}

void generateTexture2D(unsigned char *data, int width, int height, int nrChannels)
{
	if (data)
	{
		if (nrChannels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 1) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 2) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 3: Q1 William Walter", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glfwSetCursorPosCallback(window, mouse_callback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	init();

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		display();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		rotFlg1 = true;
	}

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		rotFlg2 = true;
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		rotFlg1 = false;
		rotFlg2 = false;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}