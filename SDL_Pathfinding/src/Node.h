#pragma once

class Node {
public:
	Node();
	~Node();

	int cost;

	Node* RightNeighbor;
	Node* LeftNeighbor;
	Node* TopNeighbor;
	Node* BottomNeighbor;
	
};