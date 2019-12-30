#pragma once
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Odin.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <stdio.h>
#include <winsock2.h>
#include <thread>
#include "ClientPlayer.h"
#include "ClientProjectil.h"
#include "ClientMap.h"
#include "ClientBar.h"
#include <mutex>
#pragma comment (lib, "ws2_32.lib")
const float32	CLIENT_TIMEOUT = 3.0f;

enum class Client_Message : uint8 {
	Join,
	Leave,
	Input,
	GameStats
};
enum class Server_Message : uint8 {
	JoinResult,
	State,
	GameStats
};

enum class Join_Results : uint8 {
	DENIED,
	ACCEPTED
};

enum class Game_Object_Type : uint8
{
	Player,
	Projectil
};
enum class Player_Input : uint8 
{
	NONE,
	DOWN = 1,
	UP = 2,
	RIGHT = 4,
	LEFT = 8,
	FIRE = 16
};

class Client
{
public:
	Client(sf::RenderWindow* window);
	bool Init(std::string playerName, std::string IPAddress = "127.0.0.1", uint16 port = 49153);
	void Run();
	void Listen();
	void ParseBuffer(int8* buffer);
	bool TryToConnect();
	void setIsRunning(bool isRunning);
	void setWasConnected(bool connected);
	void Disconnect();
	void HandleState(int8* buffer, int32& bytes_read);
	void HandleGameStats(int8* buffer, int32& bytes_read);
	void Draw();
	void ReadInput();
	void Update(double deltaTime);
	void SendInput(uint8 input);
	virtual ~Client();
	//bool testingDummy = false;
private:
	sf::RenderWindow* m_window;
	SOCKET sock;
	SOCKADDR_IN server_address;
	uint16 PORT = 49153;
	int8 buffer[SOCKET_BUFFER_SIZE];
	int8 listenBuffer[SOCKET_BUFFER_SIZE];
	float32 time_since_heard_from_player[MAX_CLIENTS];
	uint16 slot;
	ClientPlayer* player_objects[MAX_CLIENTS];
	ClientProjectil projectil_objects[MAX_CLIENTS * MAX_PROJECTILES];
	ClientMap* m_clientMap;
	ClientBar* m_clientBar;
	bool m_isRunning = true;
	bool m_wasConnected = false;
	float32 heard_from_server;
	sf::Texture* projectilTexture;
	std::string m_playerName = "Player";
};

