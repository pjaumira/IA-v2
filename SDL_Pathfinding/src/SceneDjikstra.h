#pragma once
#include <vector>
#include <time.h>
#include <algorithm>
#include <utility>
#include <queue>
#include <functional>
#include "Scene.h"
#include "Agent.h"
#include "Path.h"
#include "Node.h"

class SceneDjikstra :
	public Scene
{
public:
	SceneDjikstra();
	~SceneDjikstra();
	void update(float dtime, SDL_Event *event);
	void draw();
	const char* getTitle();
private:
	std::vector<Agent*> agents;
	Vector2D coinPosition;
	Vector2D currentTarget;
	int currentTargetIndex;
	Path path;
	int num_cell_x;
	int num_cell_y;
	bool draw_grid;
	std::vector<SDL_Rect> maze_rects;
	void drawMaze();
	void drawCoin();
	SDL_Texture *background_texture;
	SDL_Texture *coin_texture;
	void initMaze();
	bool loadTextures(char* filename_bg, char* filename_coin);
	std::vector< std::vector<int> > terrain;
	Vector2D cell2pix(Vector2D cell);
	Vector2D pix2cell(Vector2D pix);
	bool isValidCell(Vector2D cell);


	//nodos
	std::vector< std::vector<Node*> >maze_nodes;
	void initNodes();
	bool CheckVector(Node* node, std::vector<Node*> vec);

	//Djikstra
	std::priority_queue<std::pair<int, Node*>, std::vector<std::pair<int, Node*>>, std::greater<std::pair<int, Node*>>> nodos_frontera;
	std::vector<Node*> nodos_visitados;
	std::vector<Node*> camino_a_recorrer;
	std::vector<SDL_Rect> terreno_pantanoso;
	void Algorithm_Djisktra();
	void Restart();

};
