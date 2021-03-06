#include <iostream>
#include <vector>
#include <queue>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "../SFMLMemBlock.h"

#define MAX_WIDTH 1900

#include <cassert>
#include <cmath>

#define MAX_ORDER 20
#define MIN_ORDER 12

#define TRUE 1
#define FALSE 0

typedef struct mem_block
{
	mem_block *prev;
	mem_block *next;
	unsigned int in_use;
	size_t used_space;
	size_t block_size;
	unsigned int order;
	char value;
	int wasted;
	int is_head;
};

typedef struct mem_list
{
	mem_block *head;

	size_t free_memory;
	size_t allocated_memory;
	size_t list_size;

} mem_list;

mem_list *lst;

mem_block *NULL_BLOCK;

mem_block *closest;

std::vector<SFMLMemBlock> MemBlocks;
std::vector<SFMLMemBlock> MemBlocks2;
std::vector<SFMLMemBlock> MemBlocks3;

int vectorNum = 0;

int numBlocks;

sf::Font glyphFont;

int get_block_order(mem_block *block)
{
	int order = 0;
	int size = block->block_size;

	while (size / 2 >= 1)
	{
		order += 1;
		size /= 2;
	}

	return order;
}

void set_block_order(mem_block *block)
{
	int order = 0;
	int size = block->block_size;

	while (size / 2 >= 1)
	{
		order += 1;
		size /= 2;
	}

	block->order = order;
}

void FillSFMLBlockVector(mem_list *lst)
{
	mem_block *current = lst->head;

	std::vector<SFMLMemBlock> *blocks;

	if (vectorNum == 0)
		blocks = &MemBlocks;
	else if (vectorNum == 1)
		blocks = &MemBlocks2;
	else
		blocks = &MemBlocks3;

	blocks->clear();
	blocks->reserve(lst->list_size);

	SFMLMemBlock block;
	std::cerr << "\n\n\n";
	int index = 0;
	while (current != NULL)
	{
		block.BlockSizeInt = current->block_size;
		block.WastedInt = current->wasted;
		block.RectColor = (current->in_use == FALSE ? sf::Color::Blue : sf::Color::Red);
		if (current->is_head == TRUE)
			block.RectColor = sf::Color::Green;
		block.RectSize = sf::Vector2f((current->block_size / pow(2, MAX_ORDER)) * MAX_WIDTH, 100);
		if (index == 0)
		{
			block.RectPosition = sf::Vector2f(0.0f, 0.0f + vectorNum * 100.0f);
		}
		else
		{
			
			block.RectPosition = sf::Vector2f(blocks->at(index - 1).RectPosition.x + blocks->at(index - 1).RectSize.x, 0.0f + vectorNum * 100.0f);
		}
		block.ValueChar = current->used_space;
		block.ApplyValues();
		block.BlockSizeText.setFont(glyphFont);
		block.MemWastedText.setFont(glyphFont);
		block.ValueText.setFont(glyphFont);
		blocks->push_back(block);
		index++;
		current = current->next;
	}
	std::cerr << "\n\n\n";
}

/* Will split a block into two blocks - basically insert two new blocks with half the size and replace the old block
Will return a pointer to the LEFT block */
mem_block *split_block(mem_block *to_split)
{
	if (to_split->block_size == pow(2, MIN_ORDER))
		return NULL;

	/* We will create an additional block and shrink the size of "to_split" */
	/* Make this new block point to the same NEXT that to_split was pointed to */
	/* THEN reassign to_split->next to be pointing to block_right */
	mem_block *block_right = new mem_block;
	block_right->block_size = to_split->block_size / 2;
	block_right->in_use = FALSE;
	block_right->prev = to_split;
	block_right->next = to_split->next;
	block_right->value = 'N';
	block_right->wasted = 0;
	block_right->order = to_split->order - 1;
	block_right->used_space = 0;
	block_right->is_head = FALSE;
	to_split->order -= 1;
	to_split->next = block_right;
	to_split->block_size = to_split->block_size / 2;
	to_split->wasted = 0;
	to_split->used_space = 0;
	to_split->is_head;
	lst->list_size += 1;

	return to_split;
}

void print_list(mem_list *lst)
{
	mem_block *current = lst->head;
	int i = 0;
	printf("List size: %d Allocated Memory: %d Free Memory: %d\n", lst->list_size, lst->allocated_memory, lst->free_memory);
	while (i < lst->list_size)
	{

		printf("%d: %d (V = %c) (U = %d) ", i, current->block_size, current->value, current->in_use);
		current = current->next;
		i++;
	}
	printf("\n");
}

void buddy_free(mem_block *block)
{
	//Sample function - we're just deleting the entire list
	mem_block *tmp = block;
	while (block->next != NULL)
	{
		tmp = block;
		block = block->next;
		delete tmp;
	}

}

void buddy_init()
{
	lst = new mem_list;
	lst->list_size = 0;
	lst->head = new mem_block;
	lst->free_memory = pow(2, MAX_ORDER);
	lst->head->in_use = FALSE;
	lst->allocated_memory = 0;
	lst->head->next = NULL;
	lst->head->prev = NULL;
	lst->head->block_size = pow(2, MAX_ORDER);
	lst->head->value = 'N';
	lst->head->wasted = 0;
	lst->head->order = MAX_ORDER;
	lst->head->used_space = 0;
	lst->head->is_head = TRUE;

	numBlocks = 0;

	SFMLMemBlock block;
	block.BlockSizeInt = pow(2, MAX_ORDER);
	block.RectPosition = sf::Vector2f(0.0f, 0.0f);
	block.IN_USE = FALSE;
	block.BlockSizeText.setString(sf::String(std::to_string(block.BlockSizeInt)));
	block.WastedInt = 0;
	block.MemWastedText.setString(sf::String(std::to_string(block.WastedInt)));
	block.RectColor = sf::Color::Blue;
	block.BlockSizeText.setFont(glyphFont);
	block.MemWastedText.setFont(glyphFont);
	block.ValueText.setFont(glyphFont);
	MemBlocks.reserve(1);
	MemBlocks.push_back(block);
}

int minimum_block_size_to_find(int size)
{
	/* Determine the size of block that needs to either be found or allocated */
	/* Loss of data from double to int conversion intended */
	int __size = pow(2, MAX_ORDER);
	while (__size / 2 > size)
		__size = __size / 2;

	return __size;
}

mem_block *find_free_block(int size)
{
	mem_block *current = lst->head;
	mem_block *closest = lst->head;
	//closest->block_size = lst->head->block_size;
	int closestBlockSize = pow(2, MAX_ORDER);

	/* First we'll look for a block with exactly that size */
	/* If one is not found, then we will find the closest one that we can and use that one */
	int index = 0;
	while (current != NULL)
	{
		if (current->in_use == FALSE)
		{
			if (current->block_size == size)
			{
				return current;
			}
			else if (current->block_size >= size && current->block_size <= closestBlockSize)
			{
				closestBlockSize = current->block_size;
				closest = current;
			}
				
		}
		index++;
		current = current->next;
		/*
		// If we found the perfect free block, just return it 
		if (current->block_size == size && current->in_use == FALSE)
			return current;

		// If we didn't, but the size of the block we are looking at is large enough, look more into it 
		else if (current->block_size >= size && current->in_use == FALSE)
		{
			// If the size of the block is >= size AND less than the closest one we've found so far AND it isn't in use, make THIS ONE the closest one 
			if (current->block_size >= size && current->block_size < closestBlockSize && current->in_use == FALSE)
			{
				closest = current;
			}
		}

		// Repeat the above steps as necessary 
		current = current->next; */
	}
	return closest;
}

void buddy_alloc(int size, char val)
{
	/* Look through the list and see if we have a block with exactly that size available */
	/* If not, we will have to split blocks until we get one of the correct size */

	if (size > lst->free_memory)
	{
		printf("Not enough memory to satify request.\n");
		return;
	}

	mem_block *current = lst->head;
	int found = 0;
	int block_size_to_find = minimum_block_size_to_find(size);
	mem_block *left_block = find_free_block(size);

	if (left_block == NULL)
	{
		printf("No suitable block found.\n");
		return;
	}


	while (left_block->block_size / 2 >= size && left_block->block_size / 2 >= pow(2, MIN_ORDER))
	{
		left_block = split_block(left_block);
	}

	/* Now te block is the correct size */
	left_block->in_use = TRUE;
	left_block->value = val;
	left_block->wasted = left_block->block_size - size;
	left_block->used_space = size;
	set_block_order(left_block);
	assert(left_block->block_size >= pow(2, MIN_ORDER) && left_block->block_size <= pow(2, MAX_ORDER));

	FillSFMLBlockVector(lst);

	vectorNum++;

}

int main()
{
	int maxWidth = MAX_ORDER * 20;
	sf::RenderWindow window(sf::VideoMode(MAX_WIDTH, 800), "SFML works!");
	sf::CircleShape shape(100.f);

	shape.setFillColor(sf::Color::Red);
	glyphFont.loadFromFile("C:\\Users\\Kurt\\Documents\\Visual Studio 2015\\Projects\\BuddyAllocatorGraphical\\Utils\\Fonts\\arial.ttf");
	buddy_init();
	//print_list(lst);
	buddy_alloc(89267, 'A');
	//print_list(lst);
	buddy_alloc(23423, 'B');
	//print_list(lst);
	buddy_alloc(89267, 'C');
	print_list(lst);

	sf::Text myText;
	myText.setString(sf::String("My String"));
	myText.setFont(glyphFont);
	myText.setPosition(500, 500);
	myText.setColor(sf::Color::White);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		
		window.clear();
		
		for (int i = 0; i < MemBlocks.size(); i++)
		{
			window.draw(MemBlocks[i].Rect);
			window.draw(MemBlocks[i].BlockSizeText);
			window.draw(MemBlocks[i].MemWastedText);
			window.draw(MemBlocks[i].ValueText);
		}

		for (int i = 0; i < MemBlocks2.size(); i++)
		{
			window.draw(MemBlocks2[i].Rect);
			window.draw(MemBlocks2[i].BlockSizeText);
			window.draw(MemBlocks2[i].MemWastedText);
			window.draw(MemBlocks2[i].ValueText);
		}

		for (int i = 0; i < MemBlocks3.size(); i++)
		{
			window.draw(MemBlocks3[i].Rect);
			window.draw(MemBlocks3[i].BlockSizeText);
			window.draw(MemBlocks3[i].MemWastedText);
			window.draw(MemBlocks3[i].ValueText);
		}
		window.draw(myText);
		window.display();
	} 
	buddy_free(lst->head);
	return 0;
}
