#pragma once
#include "Vector2D.h"

class Node {
public:
	Node();
	Node(int identificator);
	//Node(int identificator, Vector2D pos);
	~Node();

	int cost;
	int id;
//	Vector2D position;

	Node* RightNeighbor;
	Node* LeftNeighbor;
	Node* TopNeighbor;
	Node* BottomNeighbor;

	Node* PreviousNode;
	
};