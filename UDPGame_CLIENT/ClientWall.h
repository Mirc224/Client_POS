#pragma once
#include <iostream>
#include "DrawableObject.h"
class ClientWall : public DrawableObject
{
public:
	ClientWall(sf::Vector2f bodysize, sf::Vector2f bodyPosition, sf::Texture* texture = nullptr);
	virtual ~ClientWall();
private:
	sf::Texture* m_texture;
};

