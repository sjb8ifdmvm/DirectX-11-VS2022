#include "KeyboardClass.h"

KeyboardClass::KeyboardClass()
	:
	autoRepeatKeys(false),
	autoRepeatChars(false)
{
	for (int i = 0; i < 256; i++)
	{
		this->keyStates[i] = false;
	}
}

bool KeyboardClass::KeyIsPressed(const unsigned char keycode)
{
	return this->keyStates[keycode];
}

bool KeyboardClass::KeyBufferIsEmpty()
{
	return this->keyBuffer.empty();
}

bool KeyboardClass::CharBufferIsEmpty()
{
	return this->charBuffer.empty();
}

KeyboardEvent KeyboardClass::ReadKey()
{
	if (this->keyBuffer.empty())
		return KeyboardEvent();
	else
	{

		KeyboardEvent KeyBuf = this->keyBuffer.front();//從隊列獲取第一個鍵
		this->keyBuffer.pop();//移除第一個元素
		return KeyBuf;	//返回該鍵
	}
}

unsigned char KeyboardClass::ReadChar()
{
	if (this->charBuffer.empty())
		return 0u;
	else
	{
		unsigned char CharBuf = this->charBuffer.front();//從隊列獲取第一個字符
		this->charBuffer.pop();	//移除第一個字符
		return CharBuf;//返回該字符
	}
}

void KeyboardClass::OnKeyPressed(const unsigned char key)
{
	this->keyStates[key] = true;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::OnKeyReleased(const unsigned char key)
{
	this->keyStates[key] = false;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void KeyboardClass::OnChar(const unsigned char key)
{
	this->charBuffer.push(key);
}

void KeyboardClass::EnableAutoRepeatKeys()
{
	this->autoRepeatKeys = true;
}

void KeyboardClass::DisableAutoRepeatKeys()
{
	this->autoRepeatKeys = false;
}

void KeyboardClass::EnableAutoRepeatChars()
{
	this->autoRepeatChars = true;
}

void KeyboardClass::DisableAutoRepeatChars()
{
	this->autoRepeatChars = false;
}

bool KeyboardClass::IsKeyAutoRepeat()
{
	return this->autoRepeatKeys;
}

bool KeyboardClass::IsCharsAutoRepeat()
{
	return this->autoRepeatChars;
}