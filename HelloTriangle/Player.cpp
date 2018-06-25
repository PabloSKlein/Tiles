#include "Player.h"
#include "Texture.h"

Player::Player(float PosInicialx, float PosInicialy, float uTexturePlayer, float vTexturePlayer)
{
	this->PosInicialx = PosInicialx;
	this->PosInicialy = PosInicialy;

	this->textura = new Texture(uTexturePlayer, vTexturePlayer);
}
