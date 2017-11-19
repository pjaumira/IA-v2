#include "Node.h"

Node::Node() {
	cost = 1;
	AccCost = 0;
	id = 0;
	position = Vector2D(0, 0);
	RightNeighbor = nullptr;
	LeftNeighbor = nullptr;
	TopNeighbor = nullptr;
	BottomNeighbor = nullptr;
	PreviousNode = nullptr;
}

Node::Node(int identificator, Vector2D pos) {
//Node::Node(int identificator) {
	cost = 1;
	AccCost = 0;
	id = identificator;
	position = pos;
	RightNeighbor = nullptr;
	LeftNeighbor = nullptr;
	TopNeighbor = nullptr;
	BottomNeighbor = nullptr;
	PreviousNode = nullptr;
}

Node::~Node() {

}