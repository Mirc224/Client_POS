#pragma once
#include "Odin.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string>
#include "DrawableObject.h"
class ClientProjectil : public DrawableObject
{
public:
	enum Projectil_Status : uint8 { ACTIVE, DISABLED };
	enum Projectil_Direction : uint8 { UP, LEFT, DOWN, RIGHT };
	ClientProjectil();
	ClientProjectil::Projectil_Status getProjectilStatus();
	ClientProjectil::Projectil_Direction getProjectilDirection();
	uint16 getOwnerSlot();
	uint8 getProjectilNumber();
	float getCordX();
	float getCordY();
	bool isActive();

	void setTexture(sf::Texture* texture);
	void setProjectilStatus(ClientProjectil::Projectil_Status status);
	void setProjectilDirection(ClientProjectil::Projectil_Direction direction);
	void setOwnerSlot(uint8 ownerSlot);
	void setProjectilNumber(uint8 projectilNum);
	void setCordX(float x);
	void setCordY(float y);


	void UploadState(int8* buffer, int32& bytes_written);
	void InsertState(int8* buffer, int32& bytes_read);
	virtual ~ClientProjectil();
private:
	uint8 m_playerSlot;
	uint8 m_projectilNumber;
	Projectil_Status m_projectilStatus = Projectil_Status::DISABLED;
	Projectil_Direction m_projectilDirection = Projectil_Direction::DOWN;
};

