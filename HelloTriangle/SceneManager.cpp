#include "SceneManager.h"
#include <iostream>
#include <fstream>

//static controllers for mouse and keyboard
static bool keys[1024];
static bool resized;
static GLuint width, height;
// load image, create texture and generate mipmaps
float x = 400.0f;
float y = 132.0f;
float TamanhoMapaX = 9 , TamanhoMapaY = 9;
float uTextureTile = 1.0 / 8.0, vTextureTile = 1.0 / 2.0;
float uTexturePlayer = 1.0 / 8.0, vTexturePlayer = 1.0 / 2.0;
float wtTile = 64.0f, htTile = 32.0f;
float wtPlayer = 64.0f, htPlayer = 32.0f;
int mapa[10][10];
float mapX = wtTile * TamanhoMapaX;
float mapY = htTile * TamanhoMapaX;
float xo = 400.0f;
float yo = 100.0f;


SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::initialize(GLuint w, GLuint h)
{
	width = w;
	height = h;

	ifstream file("../SetupTiles.txt");
	if (!file) {
		cout << "-";
	}
	else {

		for (int i = 0; i < TamanhoMapaX; i++)
		{
			for (int j = 0; j < TamanhoMapaY; j++)
			{
				file >> mapa[i][j];
				cout << mapa[i][j];
			}
			cout << endl;
		}
	}

	// GLFW - GLEW - OPENGL general setup -- TODO: config file
	initializeGraphics();
}

void SceneManager::initializeGraphics()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(width, height, "Hello Transform", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	//Setando a callback de redimensionamento da janela
	glfwSetWindowSizeCallback(window, resize);
	
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Build and compile our shader program
	addShader("../shaders/transformations.vs", "../shaders/transformations.frag");

	//setup the scene -- LEMBRANDO QUE A DESCRIÇÃO DE UMA CENA PODE VIR DE ARQUIVO(S) DE 
	// CONFIGURAÇÃO
	setupScene();

	resized = true; //para entrar no setup da câmera na 1a vez

}

void SceneManager::addShader(string vFilename, string fFilename)
{
	shader = new Shader (vFilename.c_str(), fFilename.c_str());
}


void SceneManager::key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void SceneManager::resize(GLFWwindow * window, int w, int h)
{
	width = w;
	height = h;
	resized = true;

	// Define the viewport dimensions
	glViewport(0, 0, width, height);
}


void SceneManager::do_movement()
{
	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		y -= 10.00f;
	}else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		y += 10.00f;
	}else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		x -= 10.00f;
	}else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		x += 10.00f;
	}
}

void SceneManager::renderBackGround()
{

	setupTexture(1);

	// Clear the colorbuffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render scene
	shader->Use();

	float xi = 0.0, yi = 0.0;
	for (int i = 0; i < TamanhoMapaX; i++)
	{	
		for (int j = 0; j < TamanhoMapaY; j++)
		{
			// Create transformations 
			model = glm::mat4();

			xi = ((i - j) * wtTile / 2.0f) + xo;
			yi = ((i + j) * htTile / 2.0f) + yo;

			model = glm::translate(model, glm::vec3(xi, yi, 0.0));

			// Get their uniform location
			GLint modelLoc = glGetUniformLocation(shader->Program, "model");

			// Pass them to the shaders
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));	

			GLint offsetLoc = glGetUniformLocation(shader->Program, "offsetUV");
			GLfloat TexX = 0.0f;
			GLfloat TexY = 0.0f;

			if (mapa[i][j] > 7) {
				TexY = 0.5f;
				TexX = (mapa[i][j] / 8.0f) - 1;
			}
			else {
				TexY = 0.0f;
				TexX = mapa[i][j] * (1.0f / 8.0f);
			}

			glm::vec2 offset(TexX, TexY);
			glUniform2f(offsetLoc, offset.x, offset.y);

			if (resized) //se houve redimensionamento na janela, redefine a projection matrix
			{
				setupCamera2D();
				resized = false;
			}

			// bind Texture
			// Bind Textures using texture units

			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(glGetUniformLocation(shader->Program, "ourTexture1"), 0);

			// render container
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
}

void SceneManager::renderPlayer()
{
	setupTexture(0);
	// Create transformations 
	model = glm::mat4();

	model = glm::translate(model, glm::vec3(x, y, 0.0));

	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(shader->Program, "model");

	// Pass them to the shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	GLint offsetLoc = glGetUniformLocation(shader->Program, "offsetUV");

	glm::vec4 vec(1.0f, 1.0f, 0.0f, 1.0f);
	glm::vec2 offset(0, 0);
	glUniform2f(offsetLoc, offset.x, offset.y);

	if (resized) //se houve redimensionamento na janela, redefine a projection matrix
	{
		setupCamera2D();
		resized = false;
	}

	// bind Texture
	// Bind Textures using texture units

	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader->Program, "ourTexture1"), 0);

	// render container
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void SceneManager::run()
{
	//GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
		
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		//Update method(s)
		do_movement();

		//Render scene
		renderBackGround();

		renderPlayer();
		
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
}

void SceneManager::finish()
{
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}


void SceneManager::setupScene()
{
	float vertices[] = {
		//positions          // colors           // texture coords
		wtTile/2, htTile,   0.0f,   1.0f, 0.0f, 0.0f,   0.0f, vTextureTile, // top 
		wtTile,   htTile/2, 0.0f,   0.0f, 1.0f, 0.0f,   uTextureTile, vTextureTile, // right
		0.0f, htTile/2, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // left
		wtTile/2, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   uTextureTile, 0.0f  // down
	};

	unsigned int indices[] = {
		0, 1, 2, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void SceneManager::setupCamera2D()
{
	//corrigindo o aspecto
	float ratio;
	float xMin = 0.0, xMax = 800, yMin = 600, yMax = 0.0, zNear = -1.0, zFar = 1.0;
	if (width >= height)
	{
		ratio = width / (float)height;
		projection = glm::ortho(xMin*ratio, xMax*ratio, yMin, yMax, zNear, zFar);
	}
	else
	{
		ratio = height / (float)width;
		projection = glm::ortho(xMin, xMax, yMin*ratio, yMax*ratio, zNear, zFar);
	}

	// Get their uniform location
	GLint projLoc = glGetUniformLocation(shader->Program, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void SceneManager::setupTexture(int textura)
{

	// load and create a texture 
	// -------------------------
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	int width, height, nrChannels;

	unsigned char *data;

	if(textura == 1)
		data = stbi_load("../textures/tileset.png", &width, &height, &nrChannels, 0);
	else
		data = stbi_load("../textures/sprite.png", &width, &height, &nrChannels, 0);

	if (data)
	{
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
