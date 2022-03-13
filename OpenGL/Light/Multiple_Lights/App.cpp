#include "Core.h"

#include "Shader/Shader.h"
#include "Engine/Components/CameraComponents.h"
#include "Texture/Texture.h"
#include "Engine/Components/LightComponent.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(Vector3(0.0f, 0.0f, 3.0f));
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

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn OpenGL", NULL, NULL);
	if (window == NULL)
	{
		Logger::getLogger()->PrintError("Failed to create GLFW window");
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
		Logger::getLogger()->PrintError("Failed to initialize GLAD");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader CubeShader("Shader/CubeVertexShader.vert", "Shader/CubeFragmentShader.frag");
	Shader LightShader("Shader/LightVertexShader.vert", "Shader/LightFragmentShader.frag");

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	Vector3 cubePositions[] = {
		Vector3(0.0f,  0.0f,  0.0f),
		Vector3(2.0f,  5.0f, -15.0f),
		Vector3(-1.5f, -2.2f, -2.5f),
		Vector3(-3.8f, -2.0f, -12.3f),
		Vector3(2.4f, -0.4f, -3.5f),
		Vector3(-1.7f,  3.0f, -7.5f),
		Vector3(1.3f, -2.0f, -2.5f),
		Vector3(1.5f,  2.0f, -2.5f),
		Vector3(1.5f,  0.2f, -1.5f),
		Vector3(-1.3f,  1.0f, -1.5f)
	};

	Vector3 pointLightsPosition[] = {
		Vector3(0.7f,  0.2f,  2.0f),
		Vector3(2.3f, -3.3f, -4.0f),
		Vector3(-4.0f,  2.0f, -12.0f),
		Vector3(0.0f,  0.0f, -3.0f)
	};

	unsigned int CubeVAO, VBO;
	glGenVertexArrays(1, &CubeVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(CubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int LightVAO;
	glGenVertexArrays(1, &LightVAO);
	glBindVertexArray(LightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Cube Duffuse Map
	unsigned int CubeDiffuseMap = Texture::loadTexture("Texture/pics/containerDiffuseMap.png");
	unsigned int CubeSpecularMap = Texture::loadTexture("Texture/pics/containerSpecularMap.png");

	CubeShader.useShaderProgram();

	CubeShader.useUniformInt("material.Diffuse", 0);
	CubeShader.useUniformInt("material.Specular", 1);
	CubeShader.useUniformFloat("material.Shininess", 64.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CubeDiffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, CubeSpecularMap);

	Vector3 LightColor(0.0f, 1.0f, 0.0f);

	DirectLight directLight(Vector3(-0.2f, -1.0f, -0.3f), LightColor * 0.0f, LightColor * 0.0f, LightColor * 0.0f);
	directLight.SetShaderData(CubeShader, "directLight");
	
	for (int i = 0; i < 4; ++i) {
		PointLight pointLight(pointLightsPosition[i], LightColor * 0.1f, LightColor * 0.1f, LightColor,
			1.0f, 0.09f, 0.032f);
		std::string suffix = "";
		suffix += '[';
		suffix += (char)(i + '0');
		suffix += ']';
		std::string prefix = "pointLight";
		pointLight.SetShaderData(CubeShader, prefix + suffix);
	}

	SpotLight spotLight(Vector3(0.0f), Vector3(0.0f), LightColor * 0.2f, LightColor * 0.8f, LightColor,
		1.0f, 0.09f, 0.032f, glm::cos(glm::radians(10.f)), glm::cos(glm::radians(15.f)));
	spotLight.SetShaderData(CubeShader, "spotLight");

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 ProjectionMat = glm::perspective(glm::radians(camera.getFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);

		LightShader.useShaderProgram();
		LightShader.useUniformMat4("MatProjection", ProjectionMat);
		LightShader.useUniformMat4("MatView", camera.getViewMat());
		glBindVertexArray(LightVAO);
		for (int i = 0; i < 4; ++i) {
			glm::mat4 LightModelMat = glm::mat4(1.0f);
			LightModelMat = glm::translate(LightModelMat, pointLightsPosition[i]);
			LightModelMat = glm::scale(LightModelMat, Vector3(0.2f));
			LightShader.useUniformMat4("MatModel", LightModelMat);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		CubeShader.useShaderProgram();
		CubeShader.useUniformMat4("MatView", camera.getViewMat());
		CubeShader.useUniformMat4("MatProjection", ProjectionMat);
		CubeShader.useUniformVec3("viewPos", camera.getCameraPos());

		CubeShader.useUniformVec3("spotLight.Position", camera.getCameraPos());
		CubeShader.useUniformVec3("spotLight.Direction", camera.getFrontVec3());

		glBindVertexArray(CubeVAO);

		for (int i = 0; i < 10; ++i) {
			glm::mat4 ModelMat = glm::mat4(1.0f);
			ModelMat = glm::translate(ModelMat, cubePositions[i]);
			float angle = 20.0f * i;
			ModelMat = glm::rotate(ModelMat, glm::radians(angle), Vector3(1.0f, 0.3f, 0.5f));
			CubeShader.useUniformMat4("MatModel", ModelMat);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &CubeVAO);
	glDeleteBuffers(1, &VBO);

	CubeShader.deleteShaderProgram();
	LightShader.deleteShaderProgram();

	glDeleteTextures(1, &CubeDiffuseMap);
	glDeleteTextures(1, &CubeSpecularMap);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

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