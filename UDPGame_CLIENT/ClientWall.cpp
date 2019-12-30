#include "ClientWall.h"


ClientWall::ClientWall(sf::Vector2f bodysize, sf::Vector2f bodyPosition, sf::Texture * texture)
	: DrawableObject(bodysize, bodyPosition)
{
	if (texture != nullptr)
	{
		this->m_texture = texture;
		this->body.setTexture(texture);
	}
}

ClientWall::~ClientWall()
{
}
