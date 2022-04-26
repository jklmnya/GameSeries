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
	Shader GBufferShader("ShaderCodes/Advanced_Lighting/GBuffer/GeometryPassShader.vert", 
		"ShaderCodes/Advanced_Lighting/GBuffer/GeometryPassShader.frag");
	Shader ScreenShader("ShaderCodes/Advanced_Lighting/GBuffer/LightingPassShader.vert",
		"ShaderCodes/Advanced_Lighting/GBuffer/LightingPassShader.frag");
	Shader LightCubeShader("ShaderCodes/Advanced_Lighting/GBuffer/LightCubeShader.vert",
		"ShaderCodes/Advanced_Lighting/GBuffer/LightCubeShader.frag");

	// Lights
	const unsigned int NR_LIGHTS = 32;
	std::vector<Vector3> lightPositions;
	std::vector<Vector3> lightColors;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// Calculate slightly random offsets
		float xPos = static_cast<float>((rand() % 100) / 100.0) * 6.0 - 3.0;
		float yPos = static_cast<float>((rand() % 100) / 100.0) * 6.0 - 4.0;
		float zPos = static_cast<float>((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(std::move(Vector3(xPos, yPos, zPos)));
		// Also calculate random color
		float rColor = static_cast<float>((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		float gColor = static_cast<float>((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		float bColor = static_cast<float>((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		lightColors.push_back(std::move(Vector3(rColor, gColor, bColor)));
	}

	ScreenShader.useShaderProgram();
	std::string s1 = "Lights[";
	std::string s2 = "].LightPos";
	std::string s3 = "].LightColor";
	for (size_t i = 0; i < NR_LIGHTS;++i) {
		ScreenShader.useUniformVec3((s1 + std::to_string(i) + s2).c_str(), lightPositions[i]);
		ScreenShader.useUniformVec3((s1 + std::to_string(i) + s3).c_str(), lightColors[i]);
	}

	// 3D Model
	Model Nano("D:/VS Projects/ModelSources/Nanosuit/nanosuit.obj");
	std::vector<Vector3> ObjectPosition = {
		{-3.0, -3.0, -3.0},
		{0.0, -3.0, -3.0},
		{3.0, -3.0, -3.0},
		{-3.0, -3.0, 0.0},
		{0.0, -3.0, 0.0},
		{3.0, -3.0, 0.0},
		{-3.0, -3.0, 3.0},
		{0.0, -3.0, 3.0},
		{3.0, -3.0, 3.0}
	};
	Quad quad;
	Cube cube;

	// GBuffer ,Blur Buffer
	NativeOpenGL::GBuffer gBuffer = NativeOpenGL::GenGBuffer(SCR_WIDTH, SCR_HEIGHT);

	// render loop
	while (!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// Render
		// Bind G Buffer
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FrameBuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mat4 ModelMat = Mat4(1.0f);
		Mat4 ViewMat = camera.getViewMat();
		Mat4 ProjectionMat = Math::Perspective(Math::Radians(camera.getFOV()),
			(float)(SCR_WIDTH) / (float)(SCR_HEIGHT), 0.1f, 100.f);

		GBufferShader.useShaderProgram();
		GBufferShader.useUniformMat4("ViewMat", ViewMat);
		GBufferShader.useUniformMat4("ProjectionMat", ProjectionMat);

		for (auto& Pos : ObjectPosition) {
			ModelMat = Mat4(1.0f);
			ModelMat = Math::Translate(ModelMat, Pos);
			ModelMat = Math::Scale(ModelMat, Vector3(0.25f));
			GBufferShader.useUniformMat4("ModelMat", ModelMat);
			Nano.Draw(GBufferShader);
		}
		
		// Bind Default Buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ScreenShader.useShaderProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBuffer.PosBuffer);
		ScreenShader.useUniformInt("FragPosMap", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gBuffer.NormalBuffer);
		ScreenShader.useUniformInt("NormalMap", 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gBuffer.AlbedoSpecBuffer);
		ScreenShader.useUniformInt("AlbedoSpecMap", 2);
		ScreenShader.useUniformVec3("ViewPos", camera.getCameraPos());
		quad.Draw(ScreenShader);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.FrameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// LightCube
		LightCubeShader.useShaderProgram();
		for (size_t i = 0; i < NR_LIGHTS; ++i) {
			ModelMat = Mat4(1.0f);
			ModelMat = Math::Translate(ModelMat, lightPositions[i]);
			ModelMat = Math::Scale(ModelMat, Vector3(0.1f));
			LightCubeShader.useMVP(ModelMat, ViewMat, ProjectionMat);
			LightCubeShader.useUniformVec3("LightColor", lightColors[i]);
			cube.Draw(LightCubeShader);
		}

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