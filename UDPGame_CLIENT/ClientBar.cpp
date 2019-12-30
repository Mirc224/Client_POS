#include "ClientBar.h"


ClientBar::ClientBar(ClientPlayer * player, sf::RenderWindow& win, sf::Texture * texture)
	: m_player(player)
{
	sf::Vector2u windowSize = win.getSize();
	float height = 60;
	m_barBody.setSize(sf::Vector2f(win.getSize().x, height));
	m_barBody.setPosition(sf::Vector2f(-(win.getSize().x / 2.0f), (win.getSize().y / 2.0) - height/2));
	m_barBody.setFillColor(sf::Color(36, 41, 43, 255));

	sf::Vector2f playerBarPosition = m_barBody.getPosition();
	float halfPlayerBarY = height / 2.0;
	float ammoOffsetX = m_barBody.getSize().x * 0.25;
	float ammoRadius = height * 0.20;
	int naboje = MAX_AMMO;
	for (int i = 0; i < naboje; ++i)
	{
		sf::CircleShape* tmp = new sf::CircleShape(ammoRadius);
		tmp->setOrigin(ammoRadius, ammoRadius);
		tmp->setPosition(playerBarPosition.x + ammoOffsetX + 2 * i*ammoRadius,
			playerBarPosition.y + halfPlayerBarY);
		ammoOffsetX += 5;
		m_playerAmmoShapes.push_back(tmp);
	}


	if (texture != nullptr)
	{
		for (uint16 i = 0; i < MAX_AMMO; ++i)
		{
			m_playerAmmoShapes.at(i)->setTexture(texture);
		}
	}
}

void ClientBar::draw(sf::RenderWindow * window)
{
	window->draw(this->m_barBody);
	uint16 naboje = m_player->getPlayerAmmo();
	for (int i = 0; i < naboje; ++i)
	{
		window->draw(*m_playerAmmoShapes.at(i));
	}
}

void ClientBar::setProjectilTexture(sf::Texture * projectilTexture)
{
	for (uint16 i = 0; i < MAX_AMMO; ++i)
	{
		m_playerAmmoShapes.at(i)->setTexture(projectilTexture);
	}
}

ClientBar::~ClientBar()
{
	for (auto& o : m_playerAmmoShapes)
		delete o;
}
