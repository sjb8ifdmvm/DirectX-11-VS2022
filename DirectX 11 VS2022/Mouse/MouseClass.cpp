#include "MouseClass.h"

MouseClass::MouseClass()
	:
	LeftIsDown(false),
	RightIsDown(false),
	MiddleIsDown(false),
	x(0),
	y(0)
{
}

void MouseClass::OnLeftPressed(int x, int y)
{
	this->LeftIsDown = true;
	this->EventBuffer.push(MouseEvent(MouseEvent::EventType::LeftPress, x, y));
}

void MouseClass::OnLeftReleased(int x, int y)
{
	this->LeftIsDown = false;
	this->EventBuffer.push(MouseEvent(MouseEvent::EventType::LeftRelease, x, y));
}

void MouseClass::OnRightPressed(int x, int y)
{
	this->RightIsDown = true;
	this->EventBuffer.push(MouseEvent(MouseEvent::EventType::RightPress, x, y));
}

void MouseClass::OnRightReleased(int x, int y)
{
	this->RightIsDown = false;
	this->EventBuffer.push(MouseEvent(MouseEvent::EventType::RightRelease, x, y));
}

void MouseClass::OnMiddlePressed(int x, int y)
{
	this->MiddleIsDown = true;
	this->EventBuffer.push(MouseEvent(MouseEvent::EventType::MiddlePress, x, y));
}

void MouseClass::OnMiddleReleased(int x, int y)
{
	this->MiddleIsDown = false;
	this->EventBuffer.push(MouseEvent(MouseEvent::EventType::MiddleRelease, x, y));
}

void MouseClass::OnWheelUp(int x, int y)
{
	this->EventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
}

void MouseClass::OnWheelDown(int x, int y)
{
	this->EventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
}

void MouseClass::OnMouseMove(int x, int y)
{
	this->x = x;
	this->y = y;
	this->EventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));
}

void MouseClass::OnMouseMoveRaw(int x, int y)
{
	this->EventBuffer.push(MouseEvent(MouseEvent::EventType::RAW_MOVE, x, y));
}

bool MouseClass::IsLeftDown()
{
	return this->LeftIsDown;
}

bool MouseClass::IsRightDown()
{
	return this->RightIsDown;
}

bool MouseClass::IsMiddleDown()
{
	return this->MiddleIsDown;
}

int MouseClass::GetPosX()
{
	return this->x;
}

int MouseClass::GetPosY()
{
	return this->y;
}

MousePoint MouseClass::GetPos()
{
	return { this->x,this->y };
}

bool MouseClass::EventBufferIsEmpty()
{
	return this->EventBuffer.empty();
}

MouseEvent MouseClass::ReadEvent()
{
	if (this->EventBuffer.empty())
		return MouseEvent();
	else
	{
		MouseEvent MouseEvent = this->EventBuffer.front();
		this->EventBuffer.pop();
		return MouseEvent;
	}
}