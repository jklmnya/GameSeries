#include "Engine/Basic/Core.h"

#include "Engine/Components/CameraComponents.h"
#include "Engine/Components/LightComponent.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderQuad();
void renderCube();

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const unsigned int SHODOW_MAPPING_WIDTH = 1024;

// Camera
Camera camera(Vector3(0.0f, 0.0f, 5.0f));
double lastX = SCR_WIDTH / 2.f;
double lastY = SCR_HEIGHT / 2.f;
bool firstMouse = true;
bool hdr = true;
bool hdrKeyPressed = false;

float lastFrame = 0.0f;
float deltaTime = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn OpenGL", NULL, NULL);
	if (window == NULL) {
		Logger::GetLogger()->PrintError("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Logger::GetLogger()->PrintError("Failed to initialize GLAD");
		return -1;
	}

	// global OpenGL State
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE);

	// Shader
	Shader CubeShader("ShaderCodes/Advanced_Lighting/HDR/CubeShader.vert", 
		"ShaderCodes/Advanced_Lighting/HDR/CubeShader.frag");
	Shader ScreenHDRShader("ShaderCodes/Advanced_Lighting/HDR/ScreenHDRShader.vert",
		"ShaderCodes/Advanced_Lighting/HDR/ScreenHDRShader.frag");

	// Texture
	unsigned int DiffuseMap = TextureUtil::TextureFromFile("Textures/pics/wood.png", true);

	// FrameBuffer
	auto [HDRBuffer, HDRColor, HDRDepth] = NativeOpenGL::GenFrameBufferHDR(SCR_WIDTH, SCR_HEIGHT);

	// Light
	// positions
	std::vector<Vector3> lightPositions;
	lightPositions.push_back(Vector3(0.0f, 0.0f, 49.5f)); // back light
	lightPositions.push_back(Vector3(-1.4f, -1.9f, 9.0f));
	lightPositions.push_back(Vector3(0.0f, -1.8f, 4.0f));
	lightPositions.push_back(Vector3(0.8f, -1.7f, 6.0f));
	// colors
	std::vector<Vector3> lightColors;
	lightColors.push_back(Vector3(200.0f, 200.0f, 200.0f));
	lightColors.push_back(Vector3(0.1f, 0.0f, 0.0f));
	lightColors.push_back(Vector3(0.0f, 0.0f, 0.2f));
	lightColors.push_back(Vector3(0.0f, 0.1f, 0.0f));

	const std::string s1 = "Lights[";
	const std::string s2 = "].Pos";
	const std::string s3 = "].Color";
	CubeShader.useShaderProgram();
	for (int i = 0; i < lightPositions.size(); ++i) {

		// std::cout << s1 + std::to_string(i) + s2 << '\n';

		CubeShader.useUniformVec3((s1 + std::to_string(i) + s2).c_str(), lightPositions[i]);
		CubeShader.useUniformVec3((s1 + std::to_string(i) + s3).c_str(), lightColors[i]);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DiffuseMap);
	CubeShader.useUniformInt("DiffuseMap", 0);
	CubeShader.useUniformInt("Inverse_normal", true);

	ScreenHDRShader.useShaderProgram();
	ScreenHDRShader.useUniformFloat("Exposure", 0.1f);

	// render loop
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// Render
		glBindFramebuffer(GL_FRAMEBUFFER, HDRBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mat4 ViewMat = camera.getViewMat();
		Mat4 ProjectionMat = Math::Perspective(Math::Radians(camera.getFOV()), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
		Mat4 ModelMat = Mat4(1.0f);
		ModelMat = Math::Translate(ModelMat, Vector3(0.0f, 0.0f, 25.0f));
		ModelMat = Math::Scale(ModelMat, Vector3(2.5f, 2.5f, 27.5f));
		
		CubeShader.useShaderProgram();
		CubeShader.useMVP(ModelMat, ViewMat, ProjectionMat);
		CubeShader.useUniformVec3("ViewPos", camera.getCameraPos());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, DiffuseMap);
		renderCube();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ScreenHDRShader.useShaderProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, HDRColor);
		ScreenHDRShader.useUniformInt("HDRMap", 0);
		ScreenHDRShader.useUniformInt("bUseHDR", hdr);
		renderQuad();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboardInput(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboardInput(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboardInput(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboardInput(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.processKeyE(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.processKeyQ(deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !hdrKeyPressed) {
		hdr = !hdr;
		hdrKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		hdrKeyPressed = false;
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	camera.processMouseMovement(static_cast<float>(xpos - lastX), static_cast<float>(lastY - ypos));
	lastX = xpos;
	lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.processMouseScroll(static_cast<float>(yoffset));
}