#include "ClientMap.h"



ClientMap::ClientMap(sf::RenderWindow* window)
{
	sf::Vector2f bgSize(window->getSize().x, window->getSize().y);
	m_backgroundImage.setSize(bgSize);
	m_backgroundImage.setOrigin(bgSize / 2.0f);
	m_backgroundTexture = new sf::Texture();
	sf::Texture* tmpTexture = new sf::Texture();
	tmpTexture->loadFromFile("tree.png");
	m_textures.insert(std::pair<std::string, sf::Texture*>("tree", tmpTexture));
}

void ClientMap::loadMap(uint16 mapNumber)
{
	for (uint16 i = 0; i < m_mapObjects.size(); ++i)
	{
		delete m_mapObjects[i];
		m_mapObjects[i] = nullptr;
	}
	m_mapObjects.clear();
	
	ClientWall* tmpDrawable = nullptr;
	std::string mapPath = "map" + std::to_string(mapNumber) + ".txt";
	std::ifstream mapfile(mapPath);
	std::string backgroundTexture;
	std::getline(mapfile, backgroundTexture);
	m_backgroundTexture->loadFromFile(backgroundTexture);
	m_backgroundImage.setTexture(m_backgroundTexture);

	int objType;
	float width = 0;
	float heigth = 0;
	float cordX = 0;
	float cordY = 0;
	std::string textureType;
	while (mapfile >> objType)
	{
		switch ((MapObjectType)objType)
		{
		case MapObjectType::TREE:
			mapfile >> width;
			mapfile >> heigth;
			mapfile >> cordX;
			mapfile >> cordY;
			mapfile >> textureType;
			m_mapObjects.push_back(new ClientWall(sf::Vector2f(width, heigth), sf::Vector2f(cordX, cordY), m_textures[textureType]));
		default:
			break;

		}
	}
	std::cout << "Map loading complete" << std::endl;
	mapfile.close();
}

void ClientMap::setClientPlayer(ClientPlayer * client, uint16 slot)
{
	if (slot < MAX_CLIENTS && slot >= 0)
	{
		m_player[slot] = client;
	}
}

void ClientMap::setClientProjectil(ClientProjectil * projectil, uint16 slot)
{
	if (slot < NUMBER_OF_PROJECTILES && slot >= 0)
	{
		m_projectil[slot] = projectil;
	}
}

void ClientMap::draw(sf::RenderWindow* window)
{
	window->draw(m_backgroundImage);
	ClientPlayer* tmpPlayer = nullptr;
	for (int player_i = 0; player_i < MAX_CLIENTS; ++player_i)
	{
		tmpPlayer = m_player[player_i];
		if (tmpPlayer->isActive())
		{
			tmpPlayer->draw(window);
		}
	}
	ClientProjectil* tmpProjectil = nullptr;
	for (uint16 player_i = 0; player_i < MAX_CLIENTS; ++player_i)
	{
		if (m_player[player_i]->isActive())
		{
			for (uint16 playerProjectil = 0; playerProjectil < MAX_PROJECTILES; ++playerProjectil)
			{
				tmpProjectil = m_projectil[player_i * MAX_PROJECTILES + playerProjectil];
				if (tmpProjectil->isActive())
					tmpProjectil->draw(window);

			}
		}
	}
	DrawableObject* tmpObject = nullptr;
	for (uint16 mapObject_i = 0; mapObject_i < m_mapObjects.size(); ++mapObject_i)
	{
		tmpObject = m_mapObjects.at(mapObject_i);
		if (tmpObject->isActive())
			tmpObject->draw(window);
	}
}


ClientMap::~ClientMap()
{
	delete m_backgroundTexture;
	for (uint16 playerSlot = 0; playerSlot < MAX_CLIENTS; ++playerSlot)
	{
		m_player[playerSlot] = nullptr;
	}
	for (uint16 projectil = 0; projectil < NUMBER_OF_PROJECTILES; ++projectil)
	{
		m_projectil[projectil] = nullptr;
	}
	for (int i = 0; i < m_mapObjects.size(); ++i)
		delete m_mapObjects[i];
	for (auto element : m_textures) {
		delete element.second;
	}
}
