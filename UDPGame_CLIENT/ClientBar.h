#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "ClientPlayer.h"
class ClientBar
{
public:
	ClientBar(ClientPlayer* player, sf::RenderWindow& window,sf::Texture* texture = nullptr);
	void draw(sf::RenderWindow* window);
	void setProjectilTexture(sf::Texture* projectilTexture);
	virtual ~ClientBar();
private:
	sf::RectangleShape m_barBody;
	std::vector<sf::CircleShape*> m_playerAmmoShapes;
	ClientPlayer* m_player;
};

