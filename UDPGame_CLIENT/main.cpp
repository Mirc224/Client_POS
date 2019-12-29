#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <SFML/Graphics.hpp>
#include "Client.h"
static const float VIEW_HEIGHT = 680;
static const float VIEW_WIDTH = 960;
static short gameStatus = 1;

void ResizeView(const sf::RenderWindow& window, sf::View& view)
{
	float aspectRation = float(window.getSize().x) / float(window.getSize().y);
	view.setSize(VIEW_HEIGHT * aspectRation, VIEW_HEIGHT);
}



void main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(VIEW_WIDTH, VIEW_HEIGHT), "Hra");
	Client klient(window);
	if (argc > 1)
	{
		klient.testingDummy = true;
	}
	sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT));
	float deltaTime = 0.0;
	sf::Clock clock;

	if (klient.Init("192.168.1.18") && klient.TryToConnect())
	{
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
					window->close();
					break;
				case sf::Event::Resized:
					//std::cout << "New wideo width: " << event.size.width << " event.size.height " << event.size.height << std::endl;
					ResizeView(*window, view);
					break;
				case sf::Event::TextEntered:
					//if (event.text.unicode < 128)
					//printf("%c\n", event.text.unicode);
					break;
				default:
					break;
				}

				if (event.type == sf::Event::Closed)
					window->close();
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
}