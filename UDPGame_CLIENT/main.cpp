#define _CRTDBG_MAP_ALLOC
#include <iostream>
#include <string>
#include <stdio.h>
#include <crtdbg.h>
#include <SFML/Graphics.hpp>
#include "Client.h"
static const float VIEW_HEIGHT = 740;
static const float VIEW_WIDTH = 960;
static short gameStatus = 1;

bool isNumber(std::string s)
{
	for (int i = 0; i < s.length(); i++)
		if (isdigit(s[i]) == false)
			return false;

	return true;
}

bool isValidIPv4(std::string tmpIP)
{
	uint16 tmpNumber = 0;
	uint16 numberOfDots = 0;
	std::string tmpOctet = "";
	for (uint16 pos = 0; pos < tmpIP.length(); ++pos)
	{
		if (tmpIP[pos] == '.')
		{
			++numberOfDots;
			tmpNumber = std::stoi(tmpOctet);
			if (tmpNumber > 255 || tmpNumber < 0)
			{
				return false;
			}
			else
			{
				tmpOctet = "";
			}
		}
		else
		{
			if (isdigit(tmpIP[pos]))
			{
				tmpOctet += tmpIP[pos];
			}
			else
			{
				return false;
			}
		}
	}
	tmpNumber = std::stoi(tmpOctet);
	if (tmpNumber > 255 || tmpNumber < 0)
	{
		return false;
	}
	if(numberOfDots == 3)
		return true;

	return false;
}


void ResizeView(const sf::RenderWindow& window, sf::View& view)
{
	float aspectRation = float(window.getSize().x) / float(window.getSize().y);
	view.setSize(VIEW_HEIGHT * aspectRation, VIEW_HEIGHT);
}



int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::string ipAddress = "127.0.0.1";
	std::string tmpNumberString = "";
	std::string playerName = "Player";
	uint16 portNumber = 49153;
	switch (argc)
	{
	case 4:
		tmpNumberString = argv[3];
		if (!isNumber(tmpNumberString))
		{
			std::cout << "Zadali ste nespravne cislo portu" << std::endl;
		}
		else
		{
			portNumber = std::stoi(tmpNumberString);
			if (portNumber < 0)
			{
				std::cout << "Zadali ste nespravne cislo portu" << std::endl;
				portNumber = 49153;
			}
		}

	case 3:
		ipAddress = argv[2];
		if (!isValidIPv4(ipAddress))
		{
			std::cout << "Zadali ste nespravnu adresu" << std::endl;
			ipAddress = "127.0.0.1";
		}
	case 2:
		playerName = argv[1];
	case 1:
		printf("Hra sa spusti s nasledujucimi parametrami\n");
		printf("Meno hraca: %s\n", playerName.c_str());
		printf("IP adresa servera: %s\n", ipAddress.c_str());
		printf("PORT: %d\n", portNumber);
	default:
		break;
	}



	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(VIEW_WIDTH, VIEW_HEIGHT), "Hra");
	Client klient(window);
	
	sf::View view(sf::Vector2f(0.0f, 30.0f), sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT));
	float deltaTime = 0.0;
	sf::Clock clock;

	if (klient.Init(playerName, ipAddress, portNumber) && klient.TryToConnect())
	{
		klient.setWasConnected(true);
		std::thread listenThread(&Client::Listen, &klient);
		while (window->isOpen())
		{
			deltaTime = clock.restart().asSeconds();
			sf::Event event;
			while (window->pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
					klient.setIsRunning(false);
					break;
				case sf::Event::Resized:
					ResizeView(*window, view);
					break;
				case sf::Event::TextEntered:
					break;
				default:
					break;
				}

				if (event.type == sf::Event::Closed)
				{
					window->close();
					break;
				}
			}
			klient.ReadInput();
			window->setView(view);
			window->clear(sf::Color(0, 255, 255, 255));
			klient.Update(deltaTime);
			klient.Draw();
			window->display();
			Sleep(20);
			
		}
		listenThread.join();
	}
	delete window;

	return 0;
}