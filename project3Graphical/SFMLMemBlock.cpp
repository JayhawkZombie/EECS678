#include "SFMLMemBlock.h"



SFMLMemBlock::SFMLMemBlock()
{
	BlockSizeInt = 0;
	WastedInt = 0;
	RectColor = sf::Color::Blue;
	RectPosition = sf::Vector2f(0.0f, 0.0f);
	RectSize = sf::Vector2f(0.0f, 0.0f);
	is_head = 0;
	IN_USE = 0;
	ValueChar = 'N';
	BlockSizeText.setString(sf::String(std::to_string(0)));
	MemWastedText.setString(sf::String(std::to_string(0)));
}


SFMLMemBlock::~SFMLMemBlock()
{
}

void SFMLMemBlock::ApplyValues()
{
	Rect.setFillColor(RectColor);
	Rect.setSize(RectSize);
	Rect.setPosition(RectPosition);

	MemWastedText.setString(sf::String("Wasted:" + std::to_string(WastedInt)));
	BlockSizeText.setString(sf::String("Block Size: " + std::to_string(BlockSizeInt)));
	ValueText.setString(sf::String("Used: " + std::to_string(ValueChar) ));

	Rect.setOutlineThickness(5.0f);
	Rect.setOutlineColor(sf::Color::Black);
	
	BlockSizeText.setPosition(Rect.getPosition().x, Rect.getPosition().y);
	BlockSizeText.setScale(sf::Vector2f(0.5, 0.5));
	BlockSizeText.setCharacterSize(20);
	BlockSizeText.setColor(sf::Color::White);

	MemWastedText.setPosition(Rect.getPosition().x, Rect.getPosition().y + RectSize.y / 4);
	MemWastedText.setScale(sf::Vector2f(0.5, 0.5));
	MemWastedText.setCharacterSize(20);
	MemWastedText.setColor(sf::Color::White);

	ValueText.setPosition(Rect.getPosition().x, Rect.getPosition().y + 2 * RectSize.y / 4);
	ValueText.setScale(sf::Vector2f(0.5, 0.5));
	ValueText.setCharacterSize(20);
	ValueText.setColor(sf::Color::White);
}
