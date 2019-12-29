#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "ClientPlayer.h"
#include "ClientWall.h"
#include "ClientProjectil.h"
class ClientMap
{
public:
	enum MapObjectType : uint8 { TREE };

	ClientMap(sf::RenderWindow* window);
	void loadMap(uint16 mapNumber);
	void setClientPlayer(ClientPlayer* client, uint16 slot);
	void setClientProjectil(ClientProjectil* projectil, uint16 slot);
	void draw(sf::RenderWindow* window);
	virtual ~ClientMap();
private:

	ClientPlayer* m_player[MAX_CLIENTS];
	std::vector<DrawableObject*> m_mapObjects;
	ClientProjectil* m_projectil[NUMBER_OF_PROJECTILES];
	sf::RectangleShape m_backgroundImage;
	sf::Texture* m_backgroundTexture = nullptr;
};

