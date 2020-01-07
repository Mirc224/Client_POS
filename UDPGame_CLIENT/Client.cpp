#include "Client.h"

Client::Client(sf::RenderWindow * window)
	: m_window(window)
{
	this->m_clientMap = new ClientMap(window);
	std::string textures[MAX_CLIENTS] = { "animblack.png", "animgreen.png", "animred.png", "animblue.png" };
	projectilTexture = new sf::Texture();
	projectilTexture->loadFromFile("snowball.png");
	for (uint16 i = 0; i < MAX_PROJECTILES * MAX_CLIENTS; ++i)
	{
		projectil_objects[i].setOwnerSlot(i / (MAX_PROJECTILES));
		projectil_objects[i].setProjectilNumber(i % MAX_PROJECTILES);
	}

	for (uint16 i = 0; i < MAX_CLIENTS; ++i)
	{
		player_objects[i] = new ClientPlayer(sf::Vector2f(0.0, -200.0 + 70 * i), textures[i], 0.15, sf::Vector2u(3, 9));
		m_clientMap->setClientPlayer(player_objects[i], i);

	}

	for (uint16 i = 0; i < NUMBER_OF_PROJECTILES; ++i)
	{
		m_clientMap->setClientProjectil(&projectil_objects[i], i);
		projectil_objects[i].setTexture(projectilTexture);
	}
}

bool Client::Init(std::string playerName, std::string IPAddress, uint16 port)
{
	m_playerName = playerName;
	PORT = port;
	WORD winsock_version = 0x202;
	WSADATA winsock_data;
	if (WSAStartup(winsock_version, &winsock_data))
	{
		printf("WSAStartup failed: %d\n", WSAGetLastError());
		return false;
	}
	int address_family = AF_INET;
	int type = SOCK_DGRAM;
	int protocol = IPPROTO_UDP;
	sock = socket(address_family, type, protocol);
	if (sock == INVALID_SOCKET)
	{
		printf("socket failed: %d\n", WSAGetLastError());
		return false;
	}

	int octet[4];
	size_t pos = 0;
	std::string s = IPAddress;
	std::string delimiter = ".";
	std::string token;
	int i = 0;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		octet[i++] = std::stoi(token);
		s.erase(0, pos + delimiter.length());
	}
	octet[i] = std::stoi(s);
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.S_un.S_un_b.s_b1 = octet[0];
	server_address.sin_addr.S_un.S_un_b.s_b2 = octet[1];
	server_address.sin_addr.S_un.S_un_b.s_b3 = octet[2];
	server_address.sin_addr.S_un.S_un_b.s_b4 = octet[3];
	return true;
}

bool Client::TryToConnect()
{
	printf("Connecting to server.\n");
	int buffer_length = SOCKET_BUFFER_SIZE;
	int flags = 0;
	SOCKADDR* to = (SOCKADDR*)&server_address;
	int to_length = sizeof(server_address);

	ZeroMemory(buffer, SOCKET_BUFFER_SIZE);
	buffer[0] = 0;
	uint16 bytesWritten = 1;
	uint16 nameLength = this->m_playerName.length();
	memcpy(&buffer[bytesWritten], &nameLength, sizeof(nameLength));
	bytesWritten += sizeof(nameLength);
	this->m_playerName.copy(&buffer[bytesWritten], nameLength);
	bytesWritten += nameLength * sizeof(char);
	if (sendto(sock, buffer, bytesWritten + 1, flags, to, to_length) == SOCKET_ERROR)
	{
		printf("sendto failed: %d\n", WSAGetLastError());
		return false;
	}

	ZeroMemory(listenBuffer, SOCKET_BUFFER_SIZE);
	// Wait for response
	flags = 0;
	SOCKADDR_IN from;
	int from_size = sizeof(from);

	int bytes_received = recvfrom(sock, listenBuffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);

	if (bytes_received == SOCKET_ERROR)
	{
		printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d\n", WSAGetLastError());
		printf("Connection failed.\n");
		return false;
	}

	if ((int8)listenBuffer[1] == (int8)Join_Results::DENIED)
	{
		printf("Server is full.\n");
		printf("Connection failed.\n");
		return false;
	}
	memcpy(&slot, &listenBuffer[2], 2);
	uint16 bytesRead = 4;
	uint16 mapNumber = 0;
	this->m_clientBar = new ClientBar(player_objects[slot], *this->m_window, projectilTexture);
	memcpy(&mapNumber, &listenBuffer[bytesRead], sizeof(mapNumber));
	printf("Connection success.\n");
	m_clientMap->loadMap(mapNumber);
	return true;
}

void Client::setIsRunning(bool isRunning)
{
	this->m_isRunning = isRunning;
}

void Client::setWasConnected(bool connected)
{
	this->m_wasConnected = connected;
}

void Client::Disconnect()
{
	int buffer_length = SOCKET_BUFFER_SIZE;
	int flags = 0;
	SOCKADDR* to = (SOCKADDR*)&server_address;
	int to_length = sizeof(server_address);
	uint32 bytesWirtten = 0;
	ZeroMemory(buffer, SOCKET_BUFFER_SIZE);
	buffer[bytesWirtten++] = (uint8)Client_Message::Leave;
	memcpy(&buffer[bytesWirtten], &slot, sizeof(slot));
	bytesWirtten += sizeof(slot);
	if (sendto(sock, buffer, bytesWirtten + 1, flags, to, to_length) == SOCKET_ERROR)
	{
		printf("sendto failed: %d\n", WSAGetLastError());
		return;
	}
	uint8 client_message;
	bool statsReceived = false;
	int attempts = 0;
	SOCKADDR_IN from;
	int from_size = sizeof(from);
	int bytes_received = recvfrom(sock, listenBuffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);
	while (attempts < 200)
	{
		ZeroMemory(listenBuffer, SOCKET_BUFFER_SIZE);
		int flags = 0;
		int bytes_received = recvfrom(sock, listenBuffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);
		if (bytes_received == SOCKET_ERROR)
		{
			printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d\n", WSAGetLastError());
			return;
		}
		if (bytes_received > 0)
		{
			if (listenBuffer[0] == (uint8)Server_Message::GameStats)
			{
				statsReceived = true;
				break;
			}
		}
		++attempts;
	}
	if (statsReceived)
		this->ParseBuffer(listenBuffer);

	printf("Disconnected.\n");
}

void Client::HandleState(int8 * buffer, int32 & bytes_read)
{
	this->m_object_data_mutex.lock();
	memcpy(&this->m_numberOfUpdatedObjects, &buffer[bytes_read], sizeof(this->m_numberOfUpdatedObjects));
	bytes_read += sizeof(this->m_numberOfUpdatedObjects);
	for (uint16 i = 0; i < this->m_numberOfUpdatedObjects; ++i)
	{
		updatedGameStates[i].objectType = (Game_Object_Type)buffer[bytes_read++];

		switch (updatedGameStates[i].objectType)
		{
		case Game_Object_Type::Player:
			memcpy(&updatedGameStates[i].playerSlot, &buffer[bytes_read], sizeof(updatedGameStates[i].playerSlot));
			bytes_read += sizeof(updatedGameStates[i].playerSlot);
			updatedGameStates[i].status = buffer[bytes_read++];
			updatedGameStates[i].direction = buffer[bytes_read++];
			updatedGameStates[i].action = buffer[bytes_read++];
			updatedGameStates[i].ammo = buffer[bytes_read++];
			memcpy(&updatedGameStates[i].cordX, &buffer[bytes_read], sizeof(updatedGameStates[i].cordX));
			bytes_read += sizeof(updatedGameStates[i].cordX);
			memcpy(&updatedGameStates[i].cordY, &buffer[bytes_read],sizeof(updatedGameStates[i].cordY));
			bytes_read += sizeof(updatedGameStates[i].cordY);
			time_since_heard_from_player[updatedGameStates[i].playerSlot] = 0.0f;
			break;
		case Game_Object_Type::Projectil:
			memcpy(&updatedGameStates[i].playerSlot, &buffer[bytes_read], sizeof(updatedGameStates[i].playerSlot));
			bytes_read += sizeof(updatedGameStates[i].playerSlot);
			updatedGameStates[i].projectilNumber = buffer[bytes_read++];
			updatedGameStates[i].status = buffer[bytes_read++];
			updatedGameStates[i].direction = buffer[bytes_read++];
			memcpy(&updatedGameStates[i].cordX, &buffer[bytes_read], sizeof(updatedGameStates[i].cordX));
			bytes_read += sizeof(updatedGameStates[i].cordX);
			memcpy(&updatedGameStates[i].cordY, &buffer[bytes_read], sizeof(updatedGameStates[i].cordY));
			bytes_read += sizeof(updatedGameStates[i].cordY);

			break;
		default:
			break;
		}
	}
	this->m_object_data_mutex.unlock();
}

void Client::HandleGameStats(int8 * buffer, int32 & bytes_read)
{
	uint16 numberOfPlayers = 0;
	uint16 nameLength;
	uint16 kills;
	uint16 deaths;
	printf("====================GAME STATS=====================\n");
	printf("Name \t \t Kills \t \t Deaths\n");
	memcpy(&numberOfPlayers, &buffer[bytes_read], sizeof(numberOfPlayers));
	bytes_read += sizeof(numberOfPlayers);
	for (uint16 player_i = 0; player_i < numberOfPlayers; ++player_i)
	{
		memcpy( &nameLength, &buffer[bytes_read], sizeof(nameLength));
		bytes_read += sizeof(nameLength);
		std::string playerName(&listenBuffer[bytes_read], nameLength);
		bytes_read += sizeof(char) * nameLength;
		memcpy(&kills, &buffer[bytes_read], sizeof(kills));
		bytes_read += sizeof(kills);
		memcpy(&deaths, &buffer[bytes_read], sizeof(deaths));
		bytes_read += sizeof(deaths);
		if (nameLength > 6)
		{
			printf("%s \t %d \t \t %d \n", playerName.c_str(), kills, deaths);
		}
		else
		{
			printf("%s \t \t %d \t \t %d \n", playerName.c_str(), kills, deaths);
		}
	}
}

void Client::Draw()
{
	m_clientMap->draw(m_window);
	m_clientBar->draw(m_window);
}

void Client::ReadInput()
{
	uint8 input = 0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		input += (uint8)Player_Input::FIRE;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
	{
		input += (uint8)Player_Input::UP;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
	{
		input += (uint8)Player_Input::LEFT;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
	{
		input += (uint8)Player_Input::RIGHT;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
	{
		input += (uint8)Player_Input::DOWN;
	}
	SendInput(input);
}

void Client::Update(double deltaTime)
{

	for (uint16 player_i = 0; player_i < MAX_CLIENTS; ++player_i)
	{
		if (player_objects[player_i]->getPlayerActivity() == ClientPlayer::Activity::ACTIVE)
		{
			for (uint16 projectil_i = 0; projectil_i < MAX_PROJECTILES; ++projectil_i)
			{
				projectil_objects[player_i * MAX_PROJECTILES + projectil_i].setProjectilStatus(ClientProjectil::Projectil_Status::DISABLED);
			}
		}
	}
	this->m_object_data_mutex.lock();
	for (uint16 object_i = 0; object_i < this->m_numberOfUpdatedObjects; ++object_i)
	{
		switch (this->updatedGameStates[object_i].objectType)
		{
		case Game_Object_Type::Player:
			this->player_objects[updatedGameStates[object_i].playerSlot]->setAllPlayerData(this->updatedGameStates[object_i].status, this->updatedGameStates[object_i].direction, this->updatedGameStates[object_i].action,
				this->updatedGameStates[object_i].ammo, this->updatedGameStates[object_i].cordX, this->updatedGameStates[object_i].cordY);
			break;
		case Game_Object_Type::Projectil:
			this->projectil_objects[this->updatedGameStates[object_i].playerSlot * MAX_PROJECTILES + this->updatedGameStates[object_i].projectilNumber].setAllProjectilData(
				this->updatedGameStates[object_i].status, this->updatedGameStates[object_i].direction, this->updatedGameStates[object_i].cordX, this->updatedGameStates[object_i].cordY);
			break;
		default:
			break;
		}
	}
	this->m_numberOfUpdatedObjects = 0;
	this->m_object_data_mutex.unlock();

	for (uint16 i = 0; i < MAX_CLIENTS; ++i)
	{
		if (player_objects[i]->isActive())
		{
			if (time_since_heard_from_player[i] >= CLIENT_TIMEOUT)
			{
				player_objects[i]->setPlayerActivity(ClientPlayer::Activity::DISABLED);
				continue;
			}
			player_objects[i]->Update(deltaTime);
			time_since_heard_from_player[i] += deltaTime;
		}

	}
}

void Client::SendInput(uint8 input)
{
	int flags = 0;
	SOCKADDR* to = (SOCKADDR*)&server_address;
	int to_length = sizeof(server_address);
	ZeroMemory(buffer, SOCKET_BUFFER_SIZE);
	buffer[0] = (uint8)Client_Message::Input;
	int32 bytesWritten = 1;
	memcpy(&buffer[bytesWritten], &slot, sizeof(slot));
	bytesWritten += sizeof(slot);
	buffer[bytesWritten++] = (uint8)input;
	if (sendto(sock, buffer, bytesWritten + 1, flags, to, to_length) == SOCKET_ERROR)
	{
		printf("sendto failed: %d \n", WSAGetLastError());
		return;
	}
}

void Client::Run()
{

	if (TryToConnect())
	{
		printf("Connection success.\n");
	}
	else
	{
		printf("Connection failed.\n");
		return;
	}
}

void Client::Listen()
{
	u_long enabled = 1;
	ioctlsocket(sock, FIONBIO, &enabled);
	while (this->m_isRunning)
	{
		ZeroMemory(listenBuffer, SOCKET_BUFFER_SIZE);
		int flags = 0;
		SOCKADDR_IN from;
		int from_size = sizeof(from);

		while (true)
		{
			int bytes_received = recvfrom(sock, listenBuffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);
			if (bytes_received == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
				{
					printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d\n", error);
				}

				break;
			}
			if (bytes_received > 0)
				ParseBuffer(listenBuffer);
		}
	}
}

void Client::ParseBuffer(int8* buffer)
{
	Server_Message type_of_message = (Server_Message)buffer[0];
	int32 bytes_read = 1;
	switch (type_of_message)
	{
	case Server_Message::State:
		HandleState(buffer, bytes_read);
		break;
	case Server_Message::GameStats:
		HandleGameStats(buffer, bytes_read);
		break;
	default:
		break;
	}
}


Client::~Client()
{
	u_long enabled = 0;
	ioctlsocket(sock, FIONBIO, &enabled);
	if(this->m_wasConnected)
		Disconnect();
	WSACleanup();
	if (m_clientBar != nullptr)
		delete m_clientBar;
	for (uint16 i = 0; i < MAX_CLIENTS; ++i)
	{
		delete player_objects[i];
	}
	delete m_clientMap;
	delete projectilTexture;
}
