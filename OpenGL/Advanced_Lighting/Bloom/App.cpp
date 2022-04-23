#include "Engine/Basic/Core.h"

#include "Engine/Components/CameraComponents.h"
#include "Engine/Components/LightComponent.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1900;
const unsigned int SCR_HEIGHT = 1060;
const unsigned int SHODOW_MAPPING_WIDTH = 1024;

// Camera
Camera camera(Vector3(0.0f, 0.5f, 5.0f));
double lastX = SCR_WIDTH / 2.f;
double lastY = SCR_HEIGHT / 2.f;
bool firstMouse = true;

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
	// bool isFullScreen = true;
	// GLFWmonitor* pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
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
	Shader LightCubeShader("ShaderCodes/Advanced_Lighting/Bloom/LightCubeShader.vert", 
		"ShaderCodes/Advanced_Lighting/Bloom/LightCubeShader.frag");
	Shader Model3DShader("ShaderCodes/Advanced_Lighting/Bloom/3DObjectShader.vert",
		"ShaderCodes/Advanced_Lighting/Bloom/3DObjectShader.frag");
	Shader HdrShader("ShaderCodes/Advanced_Lighting/Bloom/ScreenHdrShader.vert",
		"ShaderCodes/Advanced_Lighting/Bloom/ScreenHdrShader.frag");
	Shader BlurShader("ShaderCodes/Advanced_Lighting/Bloom/BlurShader.vert",
		"ShaderCodes/Advanced_Lighting/Bloom/BlurShader.frag");

	// Texture
	unsigned int PlaneTexture = TextureUtil::TextureFromFile("Textures/pics/wood.png", true);
	unsigned int CubeTexture = TextureUtil::TextureFromFile("Textures/pics/containerDiffuseMap.png", true);

	// 3D Model
	Cube cube;
	Plane plane;
	Quad quad;

	// Lights
	std::vector<Vector3> lightPositions;
	lightPositions.push_back(Vector3(0.0f, 0.5f, 1.5f));
	lightPositions.push_back(Vector3(-6.0f, 0.5f, -3.0f));
	lightPositions.push_back(Vector3(3.0f, 0.5f, 1.0f));
	lightPositions.push_back(Vector3(-.8f, 2.4f, -1.0f));
	std::vector<Vector3> lightColors;
	lightColors.push_back(Vector3(5.0f, 5.0f, 5.0f));
	lightColors.push_back(Vector3(10.0f, 0.0f, 0.0f));
	lightColors.push_back(Vector3(0.0f, 0.0f, 15.0f));
	lightColors.push_back(Vector3(0.0f, 5.0f, 0.0f));

	std::vector<Vector3> CubePosition = {
		{0.0f, 1.5f, 0.0f},
		{2.0f, 0.0f, 1.0f},
		{-3.0f, 0.0f, 0.0f},
		{-1.0f, -1.0f, 2.0f},
		{0.0f, 2.7f, 4.0f},
		{-2.0f, 1.0f, -3.0f}
	};

	std::vector<float> CubeRotation = {60.f, 23.0f, 124.f};

	std::string s1 = ".Pos";
	std::string s2 = ".Color";
	Model3DShader.useShaderProgram();
	for (size_t i = 0; i < lightPositions.size(); ++i) {
		std::string ValName = "Lights[" + std::to_string(i) + ']';
		Model3DShader.useUniformVec3((ValName + s1).c_str(), lightPositions[i]);
		Model3DShader.useUniformVec3((ValName + s2).c_str(), lightColors[i]);
	}

	// HDR ,Blur Buffer
	NativeOpenGL::HdrBuffer hdrBuffer = NativeOpenGL::GenFrameBufferHDR(SCR_WIDTH, SCR_HEIGHT, 2);
	NativeOpenGL::GaussianBlurBuffer blurBuffer[2];
	blurBuffer[0] = NativeOpenGL::GenFrameBufferBlur(SCR_WIDTH, SCR_HEIGHT);
	blurBuffer[1] = NativeOpenGL::GenFrameBufferBlur(SCR_WIDTH, SCR_HEIGHT);

	// render loop
	while (!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render
		glBindFramebuffer(GL_FRAMEBUFFER, hdrBuffer.FrameBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mat4 ModelMat = Mat4(1.0f);
		Mat4 ViewMat = camera.getViewMat();
		Mat4 ProjectionMat = Math::Perspective(Math::Radians(camera.getFOV()),
			(float)(SCR_WIDTH) / (float)(SCR_HEIGHT), 0.1f, 100.f);

		// Light Cube
		LightCubeShader.useShaderProgram();
		for (size_t i = 0; i < lightPositions.size(); ++i) {
			ModelMat = Mat4(1.0f);
			ModelMat = Math::Translate(ModelMat, lightPositions[i]);
			ModelMat = Math::Scale(ModelMat, Vector3(0.25f));
			LightCubeShader.useMVP(ModelMat, ViewMat, ProjectionMat);
			LightCubeShader.useUniformVec3("LightColor", lightColors[i]);
			cube.Draw(LightCubeShader);
		}

		// Plane
		Model3DShader.useShaderProgram();
		Model3DShader.useUniformVec3("ViewPos", camera.getCameraPos());
		ModelMat = Mat4(1.0f);
		ModelMat = Math::Translate(ModelMat, Vector3(0.0f, -1.0f, 0.0f));
		ModelMat = Math::Scale(ModelMat, Vector3(8.0f, 1.0f, 8.0f));
		Model3DShader.useMVP(ModelMat, ViewMat, ProjectionMat);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, PlaneTexture);
		Model3DShader.useUniformInt("diffuseMap", 0);
		plane.Draw(Model3DShader);

		// Cube
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CubeTexture);
		Model3DShader.useUniformInt("diffuseMap", 0);
		for (size_t i = 0; i < 3; ++i) {
			ModelMat = Mat4(1.0f);
			ModelMat = Math::Translate(ModelMat, CubePosition[i]);
			ModelMat = Math::Scale(ModelMat, Vector3(0.5f));
			Model3DShader.useMVP(ModelMat, ViewMat, ProjectionMat);
			cube.Draw(Model3DShader);
		}
		for (size_t i = 3; i < CubePosition.size(); ++i) {
			ModelMat = Mat4(1.0f);
			ModelMat = Math::Translate(ModelMat, CubePosition[i]);
			ModelMat = Math::Rotate(ModelMat, Math::Radians(CubeRotation[i - 3]), Vector3(1.0f, 0.0f, 1.0f));
			Model3DShader.useMVP(ModelMat, ViewMat, ProjectionMat);
			cube.Draw(Model3DShader);
		}

		// Blur
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		BlurShader.useShaderProgram();
		bool bHorizontal = true, first_iteration = true;
		for (unsigned int i = 0; i < 10; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer[!bHorizontal].FrameBuffer);
			BlurShader.useUniformInt("bHorizontal", bHorizontal);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? hdrBuffer.ColorBuffer[1] : blurBuffer[bHorizontal].BlurBuffer); 
			BlurShader.useUniformInt("ImageMap", 0);
			quad.Draw(BlurShader);
			bHorizontal = !bHorizontal;
			if (first_iteration)
				first_iteration = false;
		}
		
		// Switch to Default Buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		HdrShader.useShaderProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrBuffer.ColorBuffer[0]);
		HdrShader.useUniformInt("HdrMap", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, blurBuffer[bHorizontal].BlurBuffer);
		HdrShader.useUniformInt("BlurMap", 1);
		quad.Draw(HdrShader);

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