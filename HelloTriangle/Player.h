#pragma once

#include "Texture.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Player
{
public:
	float PosInicialx = 500.0f;
	float PosInicialy = 132.0f;
	float uTexturePlayer = 1.0f / 14.0f, vTexturePlayer = 1.0f / 10.0f;
	float wtPlayer = 160.0f, htPlayer = 80.0f;
	float offsetMovimentacaoPlayer = 40.0f;
	float offsetXTexturaPlayer = 0.0f;
	float offsetYTexturaPlayer = 0.0f;
	int gabiarraDoMovimentoAnterior;
	Texture *textura;

	Player(float PosInicialx, float PosInicialy, float uTexturePlayer, float vTexturePlayer);
	~Player();

	void doMoviment();
	void setupTexture(int textura);
	void setupPlayer();
	void renderPlayer();
};