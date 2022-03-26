#include "Engine/Basic/Core.h"

#include "Engine/Components/CameraComponents.h"
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	std::vector<Vector3> grassPosition = {
		{ -1.5f,  0.0f, -0.48f },
		{ 1.5f,  0.0f,  0.51f },
		{ 0.0f,  0.0f,  0.7f },
		{ -0.3f,  0.0f, -2.3f },
		{ 0.5f,  0.0f, -0.6f }
	};

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
	// Grass VAO
	unsigned int grassVAO, grassVBO;
	glGenVertexArrays(1, &grassVAO);
	glGenBuffers(1, &grassVBO);
	glBindVertexArray(grassVAO);
	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), &transparentVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	Shader GrassShader("ShaderCodes/Blending/Grass.vert", "ShaderCodes/Blending/Grass.frag");
	Shader CubeFloorShader("ShaderCodes/StencilTest/StencilTest_Object.vert", "ShaderCodes/StencilTest/StencilTest_Object.frag");

	unsigned int floorTexture = TextureUtil::TextureFromFile("Textures/pics/metal.png");
	unsigned int cubeTexture = TextureUtil::TextureFromFile("Textures/pics/marble.jpg");
	unsigned int grassTexture = TextureUtil::TextureFromFile("Textures/pics/window.png");

	// render loop
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		//
		std::sort(grassPosition.begin(), grassPosition.end(), [](const Vector3& v1, const Vector3& v2) {
			return Math::Vec3Distance(camera.getCameraPos(), v1) > Math::Vec3Distance(camera.getCameraPos(), v2);
		});

		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Uniforms
		Mat4 ViewMat = camera.getViewMat();
		Mat4 ProjectionMat = Math::Perspective(Math::Radians(camera.getFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT,
			0.1f, 100.f);
		CubeFloorShader.useShaderProgram();
		CubeFloorShader.useUniformMat4("ViewMat", ViewMat);
		CubeFloorShader.useUniformMat4("ProjectionMat", ProjectionMat);
		GrassShader.useShaderProgram();
		GrassShader.useUniformMat4("ViewMat", ViewMat);
		GrassShader.useUniformMat4("ProjectionMat", ProjectionMat);

		// Draw Floor
		CubeFloorShader.useShaderProgram();
		glActiveTexture(GL_TEXTURE0);
		CubeFloorShader.useUniformUInt("texture1", floorTexture);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		CubeFloorShader.useUniformMat4("ModelMat", Mat4(1.0f));
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Draw Cube
		glActiveTexture(GL_TEXTURE0);
		CubeFloorShader.useUniformUInt("texture1", cubeTexture);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glBindVertexArray(cubeVAO);
		Mat4 ModelMat = Mat4(1.0f);
		ModelMat = Math::Translate(ModelMat, Vector3(-1.0f, -0.5f, -1.0f));
		CubeFloorShader.useUniformMat4("ModelMat", ModelMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		ModelMat = Mat4(1.0f);
		ModelMat = Math::Translate(ModelMat, Vector3(2.0f, 0.01f, 0.0f));
		CubeFloorShader.useUniformMat4("ModelMat", ModelMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Draw Grass
		glActiveTexture(GL_TEXTURE0);
		GrassShader.useShaderProgram();
		GrassShader.useUniformUInt("grass_texture", grassTexture);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		glBindVertexArray(grassVAO);
		for (unsigned int i = 0; i < 5; ++i) {
			ModelMat = Mat4(1.0f);
			ModelMat = Math::Translate(ModelMat, grassPosition[i]);
			GrassShader.useUniformMat4("ModelMat", ModelMat);
			glDrawArrays(GL_TRIANGLES, 0, 6);
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