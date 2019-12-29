#pragma once
#include "SFML/Graphics.hpp"
class DrawableObject
{
public:
	DrawableObject(sf::Vector2f bodySize, sf::Vector2f bodyPosition);
	void setCoords(sf::Vector2f vector);
	void draw(sf::RenderWindow* window);
	void setPosition(sf::Vector2f position);
	void setOrigin(sf::Vector2f origin);
	void setSize(sf::Vector2f size);
	bool isActive();
	virtual ~DrawableObject() = default;
protected:
	sf::RectangleShape body;
};

