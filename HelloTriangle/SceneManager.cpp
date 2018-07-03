#include "SceneManager.h"
#include <iostream>
#include <fstream>

//static controllers for mouse and keyboard
static bool keys[1024];
static bool resized;
static GLuint width, height;

//geracao de mapa com as posicoes de cada tile de acordo com o txt
int mapa[10][10];
float tamanhoMapaX = 10, tamanhoMapaY = 10;

//player
float posicaoPlayerTelaX = 460.0f;
float posicaoPlayerTelaY = 50.0f; //132
int posicaoPlayerMapaX = 0;
int posicaoPlayerMapaY = 0;
int posicaoAnteriorPlayerMapaX = posicaoPlayerMapaX;
int posicaoAnteriorPlayerMapaY = posicaoPlayerMapaY;

//textura do player
float offsetMovimentacaoPlayer = 40.0f; //40
float offsetXTexturaPlayer = 0.0f;
float offsetYTexturaPlayer = 0.0f;
int posicaoAnteriorTexturaSpritesheetPlayer;
/*0 - w , 1 - w+a, 2 - w+d,  3 - s, 4 - s+a, 5 - s+d, 6 - a - 7 - d */

// load image, create texture and generate mipmaps
float uTextureTile = 1.0f / 11.0f, vTextureTile = 1.0f / 11.0f;
float uTexturePlayer = 1.0f / 14.0f, vTexturePlayer = 1.0f / 10.0f;
float wtTile = 80.0f, htTile = 40.0f;
float wtPlayer = 160.0f, htPlayer = 80.0f;

float mapX = wtTile * tamanhoMapaX;
float mapY = htTile * tamanhoMapaX;
float comecaMontagemCenarioX = 500.0f;
float comecaMontagemCenarioY = 100.0f;

//click mouse
double clickMousePosicaoX = 460, clickMousePosicaoY = 200;
int posicaoTileMudarX = 0, posicaoTileMudarY = 0;


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

		for (int i = 0; i < tamanhoMapaX; i++)
		{
			for (int j = 0; j < tamanhoMapaY; j++)
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


	glfwSetMouseButtonCallback(window, mouseButtonCallback);

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

	//setup the scene -- LEMBRANDO QUE A DESCRI��O DE UMA CENA PODE VIR DE ARQUIVO(S) DE 
	// CONFIGURA��O	
	setupScene();

	resized = true; //para entrar no setup da c�mera na 1a vez

}

void SceneManager::addShader(string vFilename, string fFilename)
{
	shader = new Shader(vFilename.c_str(), fFilename.c_str());
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

bool SceneManager::check_colision(int posicaoPlayerMapaYCheck, int posicaoPlayerMapaXCheck) {

	//para que o player nao saia do cenario
	if (posicaoPlayerMapaYCheck < 0) {
		posicaoPlayerMapaY++;
		return false;
	}
	if (posicaoPlayerMapaXCheck < 0) {
		posicaoPlayerMapaX++;
		return false;
	}
	if (posicaoPlayerMapaYCheck > 9) {
		posicaoPlayerMapaY--;
		return false;
	}
	if (posicaoPlayerMapaXCheck > 9) {
		posicaoPlayerMapaX--;
		return false;
	}

	//se o boneco quiser andar para o tile 7 que � a pedra n�o vai conseguir
	if (mapa[posicaoPlayerMapaYCheck][posicaoPlayerMapaXCheck] == 6) {

		posicaoPlayerMapaX = posicaoAnteriorPlayerMapaX;
		posicaoPlayerMapaY = posicaoAnteriorPlayerMapaY;

		return false;
	}

	return true;

}

void SceneManager::do_movement()
{
	int posicaoTexturaSpritesheetPlayer = 0;

	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {



		posicaoAnteriorPlayerMapaX = posicaoPlayerMapaX;
		posicaoAnteriorPlayerMapaY = posicaoPlayerMapaY;

		posicaoPlayerMapaX--;
		posicaoPlayerMapaY--;

		if (check_colision(posicaoPlayerMapaY, posicaoPlayerMapaX)) {

			posicaoPlayerTelaY -= offsetMovimentacaoPlayer;

			posicaoTexturaSpritesheetPlayer = 2;

			offsetYTexturaPlayer = 5.0f / 10.0f;
		}

	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

		posicaoAnteriorPlayerMapaX = posicaoPlayerMapaX;
		posicaoAnteriorPlayerMapaY = posicaoPlayerMapaY;

		posicaoPlayerMapaX--;
		posicaoPlayerMapaY++;

		if (check_colision(posicaoPlayerMapaY, posicaoPlayerMapaX)) {

			posicaoPlayerTelaX -= offsetMovimentacaoPlayer;

			posicaoTexturaSpritesheetPlayer = 1;

			offsetYTexturaPlayer = 7.0f / 10.0f;

		}
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

		posicaoAnteriorPlayerMapaX = posicaoPlayerMapaX;
		posicaoAnteriorPlayerMapaY = posicaoPlayerMapaY;

		posicaoPlayerMapaX--;

		if (check_colision(posicaoPlayerMapaY, posicaoPlayerMapaX)) {

			posicaoPlayerTelaX -= (offsetMovimentacaoPlayer);
			posicaoPlayerTelaY -= (offsetMovimentacaoPlayer / 2);

			posicaoTexturaSpritesheetPlayer = 8;

			offsetYTexturaPlayer = 6.0f / 10.0f;

		}
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

		posicaoAnteriorPlayerMapaX = posicaoPlayerMapaX;
		posicaoAnteriorPlayerMapaY = posicaoPlayerMapaY;

		posicaoPlayerMapaX++;
		posicaoPlayerMapaY--;

		if (check_colision(posicaoPlayerMapaY, posicaoPlayerMapaX)) {

			posicaoPlayerTelaX += offsetMovimentacaoPlayer;

			posicaoTexturaSpritesheetPlayer = 5;

			offsetYTexturaPlayer = 3.0f / 10.0f;

		}
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

		posicaoAnteriorPlayerMapaX = posicaoPlayerMapaX;
		posicaoAnteriorPlayerMapaY = posicaoPlayerMapaY;

		posicaoPlayerMapaX++;
		posicaoPlayerMapaY++;

		if (check_colision(posicaoPlayerMapaY, posicaoPlayerMapaX)) {

			posicaoPlayerTelaY += offsetMovimentacaoPlayer;

			posicaoTexturaSpritesheetPlayer = 4;

			offsetYTexturaPlayer = 9.0f / 10.0f;

		}

	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

		posicaoAnteriorPlayerMapaX = posicaoPlayerMapaX;
		posicaoAnteriorPlayerMapaY = posicaoPlayerMapaY;

		posicaoPlayerMapaX++;

		if (check_colision(posicaoPlayerMapaY, posicaoPlayerMapaX)) {

			posicaoPlayerTelaX += offsetMovimentacaoPlayer;
			posicaoPlayerTelaY += offsetMovimentacaoPlayer / 2;

			posicaoTexturaSpritesheetPlayer = 3;

			offsetYTexturaPlayer = 2.0f / 10.0f;

		}

	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

		posicaoAnteriorPlayerMapaX = posicaoPlayerMapaX;
		posicaoAnteriorPlayerMapaY = posicaoPlayerMapaY;

		posicaoPlayerMapaY++;

		if (check_colision(posicaoPlayerMapaY, posicaoPlayerMapaX)) {

			posicaoPlayerTelaX -= offsetMovimentacaoPlayer;
			posicaoPlayerTelaY += offsetMovimentacaoPlayer / 2;

			posicaoTexturaSpritesheetPlayer = 6;

			offsetYTexturaPlayer = 8.0f / 10.0f;

		}

	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

		posicaoAnteriorPlayerMapaX = posicaoPlayerMapaX;
		posicaoAnteriorPlayerMapaY = posicaoPlayerMapaY;

		posicaoPlayerMapaY--;

		if (check_colision(posicaoPlayerMapaY, posicaoPlayerMapaX)) {

			posicaoPlayerTelaX += offsetMovimentacaoPlayer;
			posicaoPlayerTelaY -= offsetMovimentacaoPlayer / 2;

			posicaoTexturaSpritesheetPlayer = 7;

			offsetYTexturaPlayer = 4.0f / 10.0f;
		}

	}

	if (posicaoTexturaSpritesheetPlayer != 0) {
		/*8 - w , 1 - w+a, 2 - w+d,  3 - s, 4 - s+a, 5 - s+d, 6 - a - 7 - d */
		if (posicaoTexturaSpritesheetPlayer != posicaoAnteriorTexturaSpritesheetPlayer) {
			offsetXTexturaPlayer = 0.0f;
			posicaoAnteriorTexturaSpritesheetPlayer = posicaoTexturaSpritesheetPlayer;
		}
		else {//9
			if (offsetXTexturaPlayer == 14.0f)
				offsetXTexturaPlayer = 0.0f;
			else {
				offsetXTexturaPlayer += 1.0f / 14.0f;
			}
		}
	}
	else {
		offsetXTexturaPlayer = 0.0f;
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
	for (int i = 0; i < tamanhoMapaX; i++)
	{
		for (int j = 0; j < tamanhoMapaY; j++)
		{
			// Create transformations 
			model = glm::mat4();

			xi = ((j - i) * wtTile / 2.0f) + comecaMontagemCenarioX;
			yi = ((i + j) * htTile / 2.0f) + comecaMontagemCenarioY;

			model = glm::translate(model, glm::vec3(xi, yi, 0.0));

			// Get their uniform location
			GLint modelLoc = glGetUniformLocation(shader->Program, "model");

			// Pass them to the shaders
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			GLint offsetLoc = glGetUniformLocation(shader->Program, "offsetUV");
			GLfloat TexX = 0.0f;
			GLfloat TexY = 0.0f;
			if (mapa[i][j] > 20) {
				TexY = 2.0f / 11.0f;
				TexX = (mapa[i][j] / 11.0f) - 2;
			}
			else if (mapa[i][j] > 10) {
				TexY = 1.0f / 11.0f;
				TexX = (mapa[i][j] / 11.0f) - 1;
			}
			else {
				TexY = 0.0f;
				TexX = mapa[i][j] * (1.0f / 11.0f);
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

	model = glm::translate(model, glm::vec3(posicaoPlayerTelaX, posicaoPlayerTelaY, 0.0));

	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(shader->Program, "model");

	// Pass them to the shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	GLint offsetLoc = glGetUniformLocation(shader->Program, "offsetUV");

	glm::vec2 offset(offsetXTexturaPlayer, offsetYTexturaPlayer);
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
	//Render Scene
	setupScene();

	//GAME LOOP
	while (!glfwWindowShouldClose(window))
	{

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		//Update method(s)
		do_movement();
		check_colision_clickMouse();
		setupScene();
		renderBackGround();

		//Render Player
		setupPlayer();
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
		//positions                 // colors           // texture coords
		wtTile / 2, htTile,   0.0f,   1.0f, 0.0f, 0.0f,   0.0f,         vTextureTile, // top 
		wtTile,   htTile / 2, 0.0f,   0.0f, 1.0f, 0.0f,   uTextureTile, vTextureTile, // right
		0.0f,	  htTile / 2, 0.0f,   0.0f, 0.0f, 1.0f,	0.0f,         0.0f, // left
		wtTile / 2, 0.0f,     0.0f,   1.0f, 1.0f, 0.0f,	uTextureTile, 0.0f  // down
	};

	unsigned int indices[] = {
		0, 1, 2, // first triangle
		3, 1, 2  // second triangle
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

void SceneManager::setupPlayer()
{
	float vertices[] = {
		// positions          // colors           // texture coords
		wtPlayer,  htPlayer, 0.0f,   1.0f, 0.0f, 0.0f,   uTexturePlayer, vTexturePlayer, // top right
		wtPlayer,  0.0f,     0.0f,   0.0f, 1.0f, 0.0f,   uTexturePlayer, 0.0f, // bottom right
		0.0f,      0.0f,     0.0f,   0.0f, 0.0f, 1.0f,   0.0f,           0.0f, // bottom left
		0.0f,      htPlayer, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f,           vTexturePlayer  // top left 
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
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

void SceneManager::InitScene()
{
	setupScene();
	renderBackGround();
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

	if (textura == 1)
		data = stbi_load("../textures/tileset.png", &width, &height, &nrChannels, 0);
	else
		data = stbi_load("../textures/cowboy.png", &width, &height, &nrChannels, 0);

	if (data)
	{
		if (textura == 1) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
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

void SceneManager::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

		glfwGetCursorPos(window, &clickMousePosicaoX, &clickMousePosicaoY);

		std::cout << "Clique em: " << clickMousePosicaoX << " : " << clickMousePosicaoY << std::endl;
	}
}

void SceneManager::check_colision_clickMouse()
{
	if (clickMousePosicaoY != 0 && clickMousePosicaoX != 0) {
		if ((clickMousePosicaoY >= comecaMontagemCenarioY) && (clickMousePosicaoY <= (comecaMontagemCenarioY + (20 * 10)))) {
			if (((clickMousePosicaoX <= (comecaMontagemCenarioX + (40 * 9))) && (clickMousePosicaoX >= (comecaMontagemCenarioX - (40 * 11))))) {



				//calcula distancia Y
				float distanciaCadaMetadeTrianguloY = htTile / 2.0f; // 40/2 == 20.0f
				int quantidadeMetadesTriangulosY = tamanhoMapaY * 2; // 10*2 == 20
				int posicaoClickMetadeTrianguloMapaY = 0;
				float indiceMenorCalculoMetadeTrianguloY = 100.0f;
				float indiceMaiorCalculoMetadeTrianguloY = indiceMenorCalculoMetadeTrianguloY + distanciaCadaMetadeTrianguloY;
				float matrizLinhaVerticalTriangulo[20][40]; //x == 20  y == 40

				for (int i = 0; i < quantidadeMetadesTriangulosY; i++) {
					if (clickMousePosicaoY >= indiceMenorCalculoMetadeTrianguloY && clickMousePosicaoY < indiceMaiorCalculoMetadeTrianguloY) {
						posicaoClickMetadeTrianguloMapaY = i;
						i = quantidadeMetadesTriangulosY;
					}
					indiceMenorCalculoMetadeTrianguloY = indiceMenorCalculoMetadeTrianguloY + distanciaCadaMetadeTrianguloY;
					indiceMaiorCalculoMetadeTrianguloY = indiceMaiorCalculoMetadeTrianguloY + distanciaCadaMetadeTrianguloY;
				}

				/////////////// TODO ver se esta em relacao a 20!!!!!!!!

				//calcula distancia X
				float distanciaCadaMetadeTrianguloX = wtTile / 2.0f; //80/2 == 40.0f
				int quantidadeMetadeTriangulosComecoX = 2;
				int posicaoClickMetadeTrianguloMapaX = 0;

				int quantidadeMetadesTriangulosLoopAtualX = 0;
				int quantidadeLosangosX = 0;
				//posicaoClickMetadeTrianguloMapaY;

				if (posicaoClickMetadeTrianguloMapaY < 10) {
					quantidadeLosangosX = posicaoClickMetadeTrianguloMapaY;
					quantidadeMetadesTriangulosLoopAtualX = quantidadeLosangosX * quantidadeMetadeTriangulosComecoX;
				}
				else if (posicaoClickMetadeTrianguloMapaY >= 10 && posicaoClickMetadeTrianguloMapaY < 20) {
					quantidadeLosangosX = 20 - posicaoClickMetadeTrianguloMapaY;
					quantidadeMetadesTriangulosLoopAtualX = quantidadeLosangosX * quantidadeMetadeTriangulosComecoX;
				}

				float indiceMenorCalculoMetadeTrianguloX_Inicial = 420.0f;

				float indiceMenorCalculoMetadeTrianguloX = indiceMenorCalculoMetadeTrianguloX_Inicial - (distanciaCadaMetadeTrianguloX * quantidadeLosangosX);

				float indiceMaiorCalculoMetadeTrianguloX = indiceMenorCalculoMetadeTrianguloX + distanciaCadaMetadeTrianguloX;


				for (int j = 0; j < quantidadeMetadesTriangulosLoopAtualX; j++) {
					if (clickMousePosicaoX >= indiceMenorCalculoMetadeTrianguloX && clickMousePosicaoX < indiceMaiorCalculoMetadeTrianguloX) {
						posicaoClickMetadeTrianguloMapaX = j;
						j = quantidadeMetadesTriangulosLoopAtualX;
					}
					indiceMenorCalculoMetadeTrianguloX = indiceMenorCalculoMetadeTrianguloX + distanciaCadaMetadeTrianguloX;
					indiceMaiorCalculoMetadeTrianguloX = indiceMaiorCalculoMetadeTrianguloX + distanciaCadaMetadeTrianguloX;
				}

				//preciso validar se esta mesmo dentro da metade do triangulo

				//comecaMontagemCenarioX == 500 -> COMECA NA PONTA ESQUERDA do triangulo
				//comecaMontagemMapaY == 100

				//int parOuImparY = posicaoClickMetadeTrianguloMapaY % 2;
				//int parOuImparX = posicaoClickMetadeTrianguloMapaX % 2;

				//
				//int valorPontaDeBaixo_LinhaHipotenusaY = comecaMontagemCenarioY - (distanciaCadaMetadeTrianguloY * posicaoClickMetadeTrianguloMapaY);
				//
				//int valorPontaDeCima_LinhaHipotenusaY = (comecaMontagemCenarioY - (distanciaCadaMetadeTrianguloY * posicaoClickMetadeTrianguloMapaY)) + distanciaCadaMetadeTrianguloY;

				//int valoresLinhaHipotenusaY[20]; //20, 1 de cada vez

				////cima esquerda 
				//if(parOuImparY == 1 && parOuImparX == 1){

				//	valoresLinhaHipotenusaY[0] = valorPontaDeBaixo_LinhaHipotenusaY;
				//	for (int i = 1; i < 20; i++) {
				//		valorPontaDeBaixo_LinhaHipotenusaY = +1.0f;
				//		valoresLinhaHipotenusaY[i] = valorPontaDeBaixo_LinhaHipotenusaY;
				//	}

				////cima direita
				//} else if (parOuImparY == 1 && parOuImparX == 0) {

				//	valoresLinhaHipotenusaY[0] = valorPontaDeCima_LinhaHipotenusaY;
				//	for (int i = 1; i < 20; i++) {
				//		valorPontaDeCima_LinhaHipotenusaY = -1.0f;
				//		valoresLinhaHipotenusaY[i] = valorPontaDeCima_LinhaHipotenusaY;
				//	}

				////baixo esquerda
				//} else if (parOuImparY == 0 && parOuImparX == 1) {

				//	valoresLinhaHipotenusaY[0] = valorPontaDeCima_LinhaHipotenusaY;
				//	for (int i = 1; i < 20; i++) {
				//		valorPontaDeCima_LinhaHipotenusaY = -1.0f;
				//		valoresLinhaHipotenusaY[i] = valorPontaDeCima_LinhaHipotenusaY;
				//	}

				////baixo direita
				//} else if (parOuImparY == 0 && parOuImparX == 0) {

				//	valoresLinhaHipotenusaY[0] = valorPontaDeBaixo_LinhaHipotenusaY;
				//	 for (int i = 1; i < 20; i++) {
				//		 valorPontaDeBaixo_LinhaHipotenusaY = +1.0f;
				//		 valoresLinhaHipotenusaY[i] = valorPontaDeBaixo_LinhaHipotenusaY;
				//	 }

				//}

				//

				//int valorComecoCalculoX = comecaMontagemCenarioX + (distanciaCadaMetadeTrianguloX * posicaoClickMetadeTrianguloMapaX);

				//float valoresLinhaHipotenusaX[20]; //40, 2 de cada vez

				//valoresLinhaHipotenusaX[0] = valorComecoCalculoX;
				//for (int i = 1; i < 20; i++) {

				//	valoresLinhaHipotenusaX[i] = + 2.0f ;

				//}


				//int ultimoValorValidoX = 0;

				//for (int i = 0; i < 20; i++) {
				//	if (clickMousePosicaoX <= valoresLinhaHipotenusaX[i]){
				//		ultimoValorValidoX = i;
				//		i = 20;
				//	}
				//}

				//bool clicou;
				//
				//if (clickMousePosicaoY <= valoresLinhaHipotenusaY[ultimoValorValidoX]) {
				//	clicou = true;
				//} else {
				//	clicou = false;
				//}




				//consigo dizer qual o item qual tile est� sendo clicado

				////////////TODO colocar um validador pra ver se realmente o valor vem pra ca !!!!!!!

				int valorX = posicaoClickMetadeTrianguloMapaY;
				int valorY = posicaoClickMetadeTrianguloMapaX;

				posicaoTileMudarX = valorX / 2;
				posicaoTileMudarY = valorY / 2;

				if (valorY % 2 == 1) {

					posicaoTileMudarY = posicaoTileMudarY +1;
				}
				if (valorX % 2 == 1) {

					posicaoTileMudarX = posicaoTileMudarX +1;

				}

				posicaoTileMudarX = posicaoTileMudarX - 1;
				posicaoTileMudarY = posicaoTileMudarY - 1;

				mapa[posicaoTileMudarX][posicaoTileMudarY] = 7;

				std::cout << "x: " << posicaoTileMudarX << " : y :" << posicaoTileMudarY << std::endl;

			}
		}
	}

}
