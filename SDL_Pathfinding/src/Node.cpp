#include "Node.h"

Node::Node() {
	cost = 1;
	RightNeighbor = nullptr;
	LeftNeighbor = nullptr;
	TopNeighbor = nullptr;
	BottomNeighbor = nullptr;
}

Node::~Node() {
	delete RightNeighbor;
	delete LeftNeighbor;
	delete TopNeighbor;
	delete BottomNeighbor;
}