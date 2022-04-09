#include "Engine/Basic/Core.h"

#include "Engine/Components/CameraComponents.h"
#include "Engine/Components/LightComponent.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const unsigned int SHODOW_MAPPING_WIDTH = 1024;

// Camera
Camera camera(Vector3(0.0f, 10.0f, 30.0f));
double lastX = SCR_WIDTH / 2.f;
double lastY = SCR_HEIGHT / 2.f;
bool firstMouse = true;

float lastFrame = 0.0f;
float deltaTime = 0.0f;

Vector3 Bias(0.0f);

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
	if (window == NULL)
	{
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
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Logger::GetLogger()->PrintError("Failed to initialize GLAD");
		return -1;
	}

	// global OpenGL State
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE);


	// Models
	Model Floor("D:\\VS Projects\\ModelSources\\Floor\\floor.obj");
	Model Marry("D:\\VS Projects\\ModelSources\\Mary\\marry.obj");
	Cube LightCube;

	// Shaders
	Shader FloorShader("ShaderCodes\\RTR\\PCSS\\FloorShader.vert", "ShaderCodes\\RTR\\PCSS\\FloorShader.frag");
	Shader LightCubeShader("ShaderCodes\\RTR\\PCSS\\LightCubeShader.vert", "ShaderCodes\\RTR\\PCSS\\LightCubeShader.frag");
	Shader MarryShader("ShaderCodes\\RTR\\PCSS\\MarryShader.vert", "ShaderCodes\\RTR\\PCSS\\MarryShader.frag");
	Shader ShodowMappingShader("ShaderCodes\\RTR\\PCSS\\ShadowMappingShader.vert", "ShaderCodes\\RTR\\PCSS\\ShadowMappingShader.frag");

	// Light
	Vector3 DirectLight1Pos(0.0f, 10.f, 20.f);
	Vector3 DirectLightColor(1.0f);
	DirectLight directLight1(-DirectLight1Pos, 0.1f * DirectLightColor, 0.2f * DirectLightColor, DirectLightColor);
	
	const float f = 10.f;
	const float zNear = 0.1f;
	const float zFar = 100.f;
	Mat4 DirectLightVPMat = Math::Ortho(-f, f, -f, f, zNear, zFar) * Math::LookAt(DirectLight1Pos,
		Vector3(0.0f), Vector3(0.0f, 1.0f, 0.0f));

	// Depth Buffer
	auto [FrameBuffer, DepthBuffer] = NativeOpenGL::GenFrameBuffer();

	// render loop
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// Shodow Mapping
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHODOW_MAPPING_WIDTH, SHODOW_MAPPING_WIDTH);

		ShodowMappingShader.useShaderProgram();
		ShodowMappingShader.useUniformMat4("LightVP", DirectLightVPMat);

		Mat4 ModelMat = Mat4(1.0f);
		ModelMat = Math::Translate(ModelMat, Bias);
		ShodowMappingShader.useUniformMat4("ModelMat", ModelMat);
		Marry.Draw(ShodowMappingShader);

		ModelMat = Math::Scale(ModelMat, Vector3(0.4f));
		ShodowMappingShader.useUniformMat4("ModelMat", ModelMat);
		Floor.Draw(ShodowMappingShader);
		
		// Render to Screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		ModelMat = Mat4(1.0f);
		Mat4 ViewMat = camera.getViewMat();
		Mat4 ProjectionMat = Math::Perspective(Math::Radians(camera.getFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);

		// LightCube
		ModelMat = Mat4(1.0f);
		ModelMat = Math::Translate(ModelMat, DirectLight1Pos);
		ModelMat = Math::Scale(ModelMat, Vector3(0.2f));
		LightCubeShader.useShaderProgram();
		LightCubeShader.useMVP(ModelMat, ViewMat, ProjectionMat);
		LightCube.Draw(LightCubeShader);

		// Floor
		ModelMat = Mat4(1.0f);
		FloorShader.useShaderProgram();
		ModelMat = Math::Scale(ModelMat, Vector3(0.4f));
		FloorShader.useMVP(ModelMat, ViewMat, ProjectionMat);
		FloorShader.useUniformMat4("LightVP", DirectLightVPMat);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, DepthBuffer);
		FloorShader.useUniformUInt("ShadowMap", 0);
		Floor.Draw(FloorShader);

		// Mary
		ModelMat = Mat4(1.0f);
		ModelMat = Math::Translate(ModelMat, Bias);
		MarryShader.useShaderProgram();
		MarryShader.useMVP(ModelMat, ViewMat, ProjectionMat);
		MarryShader.useUniformMat4("LightVP", DirectLightVPMat);
		directLight1.SetShaderData(MarryShader, "uDirectLight");
		MarryShader.useUniformVec3("ViewPos", camera.getCameraPos());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, DepthBuffer);
		MarryShader.useUniformUInt("ShadowMap", 0);
		Marry.Draw(MarryShader);

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

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		Bias.x += 0.01f;
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