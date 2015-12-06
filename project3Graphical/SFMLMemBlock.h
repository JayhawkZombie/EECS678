#pragma once

#include <SFML\Graphics.hpp>

#include <string>

class SFMLMemBlock
{
public:
	SFMLMemBlock();
	~SFMLMemBlock();

	void ApplyValues();

	sf::Vector2f RectSize;
	sf::Vector2f RectPosition;
	sf::RectangleShape Rect;
	sf::Text BlockSizeText;
	sf::Text ValueText;
	sf::Text MemWastedText;
	int is_head;
	sf::Color RectColor;

	unsigned int BlockSizeInt;

	unsigned int WastedInt;
	int ValueChar;
	unsigned int IN_USE;
};

