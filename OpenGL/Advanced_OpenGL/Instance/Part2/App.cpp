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
Camera camera(Vector3(0.0f, 0.0f, 60.0f));
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

	// Model
	Model Rock("D:\\VS Projects\\ModelSources\\Rock\\rock.obj");
	Model Planet("D:\\VS Projects\\ModelSources\\Planet\\planet.obj");

	// Shader
	Shader RockShader("ShaderCodes/Instances/Rock.vert", "ShaderCodes/Instances/Rock.frag");
	Shader PlanetShader("ShaderCodes/Instances/Planet.vert", "ShaderCodes/Instances/Planet.frag");
	
	// Rock
	const unsigned int RockNum = 100000;
	Mat4* modelMatrices;
	modelMatrices = new Mat4[RockNum];
	Math::InitRandomSeed(static_cast<unsigned int>(glfwGetTime()));
	const float radius = 150.0f;
	const float offset = 25.0f;
	for (unsigned int i = 0; i < RockNum; i++)
	{
		Mat4 model = Mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)RockNum * 360.0f;
		float displacement = (Math::RandInt() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = Math::Sin(angle) * radius + displacement;
		displacement = (Math::RandInt() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (Math::RandInt() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = Math::Cos(angle) * radius + displacement;
		model = Math::Translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = static_cast<float>((Math::RandInt() % 20) / 100.0 + 0.05);
		model = Math::Scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = static_cast<float>((Math::RandInt() % 360));
		model = Math::Rotate(model, rotAngle, Vector3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	unsigned int Buffer;
	glGenBuffers(1, &Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Buffer);
	glBufferData(GL_ARRAY_BUFFER, RockNum * sizeof(Mat4) , modelMatrices, GL_STATIC_DRAW);

	std::vector<Mesh>& RockMeshes = Rock.GetModelMeshes();
	for (unsigned int i = 0; i < RockMeshes.size(); ++i) {
		unsigned int VAO = RockMeshes[i].GetMeshVAO();
		glBindVertexArray(VAO);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (void*)0);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (void*)(sizeof(Vector4)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (void*)(2 * sizeof(Vector4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (void*)(3 * sizeof(Vector4)));
		glEnableVertexAttribArray(6);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glBindVertexArray(0);
	}

	// render loop
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render
		Mat4 ViewMat = camera.getViewMat();
		Mat4 ProjectionMat = Math::Perspective(Math::Radians(camera.getFOV()), (float)(SCR_WIDTH) / (float)(SCR_HEIGHT), 0.1f, 1000.0f);
		Mat4 ModelMat = Mat4(1.0f);

		// Plante
		ModelMat = Math::Translate(ModelMat, Vector3(0.0f, -3.0f, 0.0f));
		ModelMat = Math::Scale(ModelMat, Vector3(4.0f, 4.0f, 4.0f));
		PlanetShader.useShaderProgram();
		PlanetShader.useUniformMat4("ModelMat", ModelMat);
		PlanetShader.useUniformMat4("ViewMat", ViewMat);
		PlanetShader.useUniformMat4("ProjectionMat", ProjectionMat);
		Planet.Draw(PlanetShader);

		// Rock
		RockShader.useShaderProgram();
		RockShader.useUniformMat4("ViewMat", ViewMat);
		RockShader.useUniformMat4("ProjectionMat", ProjectionMat);
		
		RockShader.useUniformUInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RockMeshes[0].GetMeshTextures()[0].TextureID);
		for (unsigned int i = 0; i < RockMeshes.size(); ++i) {
			glBindVertexArray(RockMeshes[i].GetMeshVAO());
			glDrawElementsInstanced(GL_TRIANGLES, RockMeshes[i].GetMeshIndices().size(),
				GL_UNSIGNED_INT, 0, RockNum);
			glBindVertexArray(0);
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