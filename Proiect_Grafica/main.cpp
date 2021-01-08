#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "glm/glm.hpp" 
#include "glm/gtc/matrix_transform.hpp" 
#include "glm/gtc/matrix_inverse.hpp" 
#include "glm/gtc/type_ptr.hpp" 

gps::Window myWindow;
gps::Model3D island;
gps::Model3D shark1, shark2, shark3, shark4;
gps::Model3D gull1, gull2, gull3, gull4, gull5, gull6, gull7, gull8;
gps::Model3D garfield;
gps::Model3D cloud;
gps::Model3D rain1, rain2;
gps::Model3D fire;
gps::Model3D moon;
gps::Model3D sky;
gps::Model3D chest_up;
gps::Camera myCamera(
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
gps::Shader myCustomShader, myCustomShader2;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader fogShader;

glm::mat4 modelShark1, modelShark2, modelShark3, modelShark4;
glm::mat4 modelGull1, modelGull2, modelGull3, modelGull4, modelGull5, modelGull6, modelGull7, modelGull8;
glm::mat4 modelGarf1;
glm::mat4 modelGarf2;
glm::mat4 modelGarf3;
glm::mat4 modelRain1, modelRain2, modelRain3;
glm::mat4 modelRain4, modelRain5, modelRain6;
glm::mat4 modelCloud;
glm::mat4 modelFire;
glm::mat4 modelMoon;
glm::mat4 modelSky;
glm::mat4 modelChestUp, modelChestUp2, modelChestUp3, modelChestUp4;
glm::mat4 modelChestUp5;
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat4 lightRotation;
glm::mat4 lightSpaceTrMatrix;

glm::mat3 normalMatrix;
glm::vec3 lightDir;
glm::vec3 lightColor;

GLfloat angle;
GLuint modelLoc, modelLoc2;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint lightDirLoc;
GLuint lightColorLoc;
GLuint lightSpaceTrMatrixLoc;
GLfloat cameraSpeed = 0.05f;
GLfloat lightAngle;
GLuint shadowMapFBO;
GLuint depthMapTexture;
GLfloat fog_var;
GLfloat light_var = 11.0f;

int rain_counter = 0;
bool rain_check = false;
bool jump_bool = true;
float jump_var = 0;
float rain_var = 0;
float rain_var2 = 0;
bool rain_bool = false;
bool rain_bool2 = false;
bool shader_swap = true;
const unsigned int SHADOW_WIDTH = 8192;
const unsigned int SHADOW_HEIGHT = 8192;
float pitchkey, yawkey;
bool pressedKeys[1024];
float yaw = 0.0f;
float pitch = 0.0f;
bool showDepthMap;
bool begin = true;
float x_aux, y_aux, dx, dy, limit_y = 0;
float globalObjRotationAngle = 0.0f;
float chestRotation = 0.0f;
int chest_var = false;
bool first = true;

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			error = "STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			error = "STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
	glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		if (light_var < 0) 
			light_var = 10.0f;
		else 
			light_var = -10.0f;
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		if (fog_var < 0) 
			fog_var = 10.0f;
		else 
			fog_var = -10.0f;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		if (rain_check == true) 
			rain_check = false;
		else rain_check = true;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	if (begin) {
		x_aux = xpos;
		y_aux = ypos;
		begin = false;
	}
	else {
		dx = xpos - x_aux;
		dy = ypos - y_aux;
		if (limit_y + dy < 1000.0f && limit_y + dy >-1000) 
		{
			myCamera.rotate(-0.01f * dy, 0.01f * dx);
			limit_y += dy;
		}
		x_aux = xpos;
		y_aux = ypos;
	}
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		angle -= 1.0f;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
		first = false;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle += 1.0f;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
		first = false;
	}
	if (pressedKeys[GLFW_KEY_W]) 
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);

	if (pressedKeys[GLFW_KEY_S]) 
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);

	if (pressedKeys[GLFW_KEY_A]) 
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);

	if (pressedKeys[GLFW_KEY_D]) 
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);

	if (pressedKeys[GLFW_KEY_1]) 
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);

	if (pressedKeys[GLFW_KEY_2]) 
		myCamera.move(gps::MOVE_UP, cameraSpeed);

	if (pressedKeys[GLFW_KEY_Z]) 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (pressedKeys[GLFW_KEY_X]) 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (pressedKeys[GLFW_KEY_C]) 
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

//	if (pressedKeys[GLFW_KEY_V])
	//{
	//	glEnable(GL_BLEND);
	//	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	//	glEnable(GL_POLYGON_SMOOTH);
	//	//glEnable(GL_LINE_SMOOTH);
	//	glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
	//}

}

void initOpenGLWindow()
{
	myWindow.Create(1800, 1000, "OpenGL Project Core");
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
	glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
	glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState()
{
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	//glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() 
{
	island.LoadModel("models/island1/map1.obj");
	shark1.LoadModel("models/shark1/shark1.obj");
	shark2.LoadModel("models/shark1/shark2.obj");
	shark3.LoadModel("models/shark1/shark3.obj");
	shark4.LoadModel("models/shark1/shark4.obj");
	gull1.LoadModel("models/gull/gull1.obj");
	gull2.LoadModel("models/gull/gull2.obj");
	gull3.LoadModel("models/gull/gull3.obj");
	gull4.LoadModel("models/gull/gull4.obj");
	gull5.LoadModel("models/gull/gull5.obj");
	gull6.LoadModel("models/gull/gull6.obj");
	gull7.LoadModel("models/gull/gull7.obj");
	garfield.LoadModel("models/garfield/garfield.obj");
	cloud.LoadModel("models/cloud/cloud.obj");
	rain1.LoadModel("models/rain/rain.obj");
	rain2.LoadModel("models/rain/rain2.obj");
	fire.LoadModel("models/fire/foc.obj");
	moon.LoadModel("models/moon/moon.obj");
	sky.LoadModel("models/sky/sky.obj");
	chest_up.LoadModel("models/chest_up/chest_up.obj");
}

void initShaders() 
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
}

glm::mat4 computeLightSpaceTrMatrix() 
{

	glm::mat4 lightView;

	if (light_var > 0)
		lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	else
		lightView = glm::lookAt(glm::vec3(0.0f, 1.3533f, -0.002f), glm::vec3(-0.15f, 0.40f, -0.002f), glm::vec3(1.0f, 1.0f, 1.0f));

	const GLfloat near_plane = 0.1f, far_plane = 5.0f;
	glm::mat4 lightProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void initUniforms() 
{

	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	if (light_var > 0)
		view = myCamera.getViewMatrix();
	else
		view = myCamera.getSmallLightPointMatrix();

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "smallLightView"), 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f),(float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,0.1f, 20.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightDir = glm::vec3(1.0f, 1.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");

	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

}

void initFBO() 
{
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO

	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
		0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void drawObjects(gps::Shader shader, bool depthPass) 
{

	shader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	if (!depthPass) 
	{
		if (light_var > 0)
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

		else
			normalMatrix = glm::mat3(glm::inverseTranspose(myCamera.getSmallLightPointMatrix() * model));

		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	island.Draw(shader);
	fire.Draw(shader);



	/*if (pressedKeys[GLFW_KEY_Q]) {
		modelSky = lightRotation;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSky));
		sky.Draw(shader);
	}
	else
		sky.Draw(shader);

	if (pressedKeys[GLFW_KEY_E]) {
		modelSky = lightRotation;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSky));
		sky.Draw(shader);
	}
	else*/
	sky.Draw(shader);




	modelGarf1 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	modelGarf2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, jump_var, 0.0f));
	modelGarf3 = modelGarf1 * modelGarf2;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGarf3));
	
	if (jump_bool) 
	{
		jump_var -= 0.003f;
	}
	else 
	{
		jump_var += 0.003f;
	}

	if (jump_var > 0.05)
		jump_bool = true;

	if (jump_var < 0.005)
		jump_bool = false;


	garfield.Draw(shader);


	modelChestUp2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.477693f, -0.141047f, +0.124566f)); 
	//modelChestUp2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,0.0f)); 
	modelChestUp3 = glm::rotate(glm::mat4(1.0f), glm::radians(chestRotation), glm::vec3(0.0f, 0.0f, 1.0f));
	
	if (chest_var)
	{
		chestRotation -= 0.55f;
	}
	else
	{
		chestRotation += 0.55f;
	}

	if (chestRotation > 1.0f)
		chest_var = true;

	if (chestRotation < -100.0f)
		chest_var = false;

	

	modelChestUp4 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.477693f, 0.141047f, -0.124566f));
	//modelChestUp4 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, -0.0f, -0.0f));
	//modelChestUp4 = modelChestUp5 * modelChestUp2 * modelChestUp * modelChestUp3;
	modelChestUp = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	modelChestUp5 = modelChestUp * modelChestUp4 * modelChestUp3 * modelChestUp2;

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelChestUp5));
	chest_up.Draw(shader);
	
	

	if (light_var < 0)
	{
		modelMoon = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMoon));
		moon.Draw(shader);
	}


	
	

	

	if (rain_check == true) 
	{

		modelCloud = glm::rotate(glm::mat4(1.0f), glm::radians(-globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCloud));
		cloud.Draw(shader);


		modelRain4 = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelRain4));

		modelRain5 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, rain_var, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelRain5));


		modelRain2 = glm::rotate(glm::mat4(1.0f), glm::radians(-globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelRain2));

		modelRain1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, rain_var, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelRain1));

		modelRain3 = modelRain1 * modelRain2;
		modelRain6 = modelRain4 * modelRain5;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelRain3));

		if (rain_bool) 
		{
			rain_var -= 0.2;
		}
		else 
		{
			rain_var += 1;
		}
		if (rain_bool2) 
		{
			rain_var2 -= 0.17;
		}
		else 
		{
			rain_var2 += 1;
		}

		if (rain_var > 0.5)
			rain_bool = true;

		if (rain_var < -1)
			rain_bool = false;

		if (rain_var2 > 0.5)
			rain_bool2 = true;

		if (rain_var2 < -1)
			rain_bool2 = false;

		rain1.Draw(shader);
		rain2.Draw(shader);
	}

	globalObjRotationAngle++;

	modelShark1 = glm::rotate(glm::mat4(1.0f), glm::radians(-globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelShark1));
	shark1.Draw(shader);

	modelShark2 = glm::rotate(glm::mat4(1.0f), glm::radians(-globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelShark2));
	shark2.Draw(shader);

	modelShark3 = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelShark3));
	shark3.Draw(shader);

	modelShark4 = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelShark4));
	shark4.Draw(shader);

	modelGull1 = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGull1));
	gull1.Draw(shader);

	modelGull2 = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGull2));
	gull2.Draw(shader);

	modelGull3 = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGull3));
	gull3.Draw(shader);

	modelGull4 = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGull4));
	gull4.Draw(shader);

	modelGull5 = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGull5));
	gull5.Draw(shader);

	modelGull6 = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGull6));
	gull6.Draw(shader);

	modelGull7 = glm::rotate(glm::mat4(1.0f), glm::radians(globalObjRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGull7));
	gull7.Draw(shader);

}

void renderScene() 
{

	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "fog_var"), fog_var);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "light_var"), light_var);

	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),1,GL_FALSE,glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(depthMapShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
	// final scene rendering pass (with shadows)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myCustomShader.useShaderProgram();

	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//bind the shadow map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);

	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),1,GL_FALSE,glm::value_ptr(computeLightSpaceTrMatrix()));

	drawObjects(myCustomShader, false);

}

void cleanup() 
{
	myWindow.Delete();
}

int main(int argc, const char* argv[]) 
{

	try
	{
		initOpenGLWindow();
	}
	catch (const std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
	initFBO();
	glCheckError();
	glEnable(GL_BLEND);

	while (!glfwWindowShouldClose(myWindow.getWindow())) 
	{
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

	return 0;
}
