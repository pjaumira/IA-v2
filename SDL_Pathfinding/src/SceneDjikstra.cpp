#include "SceneDjikstra.h"

using namespace std;

SceneDjikstra::SceneDjikstra()
{
	draw_grid = true;

	num_cell_x = SRC_WIDTH / CELL_SIZE;
	num_cell_y = SRC_HEIGHT / CELL_SIZE;
	initMaze();
	initNodes();
	loadTextures("../res/maze.png", "../res/coin.png");

	srand((unsigned int)time(NULL));

	Agent *agent = new Agent;
	agent->loadSpriteTexture("../res/soldier.png", 4);
	agents.push_back(agent);

	//// set agent position coords to the center of a random cell
	//Vector2D rand_cell(6, 3);
	///*while (!isValidCell(rand_cell))
	//	rand_cell = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));*/
	//agents[0]->setPosition(cell2pix(rand_cell));

	//// set the coin in a random cell (but at least 3 cells far from the agent)
	//coinPosition = Vector2D(2, 2);
	///*while ((!isValidCell(coinPosition)) || (Vector2D::Distance(coinPosition, rand_cell)<3))
	//	coinPosition = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));*/


	// set agent position coords to the center of a random cell
	Vector2D rand_cell(-1, -1);
	while (!isValidCell(rand_cell))
		rand_cell = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));
	agents[0]->setPosition(cell2pix(rand_cell));

	// set the coin in a random cell (but at least 3 cells far from the agent)
	coinPosition = Vector2D(-1, -1);
	while ((!isValidCell(coinPosition)) || (Vector2D::Distance(coinPosition, rand_cell)<3))
		coinPosition = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));

	// PathFollowing next Target
	currentTarget = Vector2D(0, 0);
	currentTargetIndex = -1;

	//calculamos la ruta óptima con BFS
	Algorithm_Djisktra();

}

SceneDjikstra::~SceneDjikstra()
{
	if (background_texture)
		SDL_DestroyTexture(background_texture);
	if (coin_texture)
		SDL_DestroyTexture(coin_texture);

	for (int i = 0; i < (int)agents.size(); i++)
	{
		delete agents[i];
	}

	for (int i = 0; i < num_cell_x; i++)
	{
		for (int j = 0; j < num_cell_y; j++)
		{
			delete maze_nodes[i][j];
		}
	}
}

void SceneDjikstra::update(float dtime, SDL_Event *event)
{
	/* Keyboard & Mouse events */
	switch (event->type) {
	case SDL_KEYDOWN:
		if (event->key.keysym.scancode == SDL_SCANCODE_SPACE)
			draw_grid = !draw_grid;
		break;
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONDOWN:
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			Vector2D cell = pix2cell(Vector2D((float)(event->button.x), (float)(event->button.y)));
			if (isValidCell(cell))
			{
				if (path.points.size() > 0)
					if (path.points[path.points.size() - 1] == cell2pix(cell))
						break;

				path.points.push_back(cell2pix(cell));
			}
		}
		break;
	default:
		break;
	}

	//


	if ((currentTargetIndex == -1) && (path.points.size()>0))
		currentTargetIndex = 0;

	if (currentTargetIndex >= 0)
	{
		float dist = Vector2D::Distance(agents[0]->getPosition(), path.points[currentTargetIndex]);
		if (dist < path.ARRIVAL_DISTANCE)
		{
			if (currentTargetIndex == path.points.size() - 1)
			{
				if (dist < 3)
				{
					path.points.clear();
					currentTargetIndex = -1;
					agents[0]->setVelocity(Vector2D(0, 0));
					// if we have arrived to the coin, replace it ina random cell!
					if (pix2cell(agents[0]->getPosition()) == coinPosition)
					{
						coinPosition = Vector2D(-1, -1);
						while ((!isValidCell(coinPosition)) || (Vector2D::Distance(coinPosition, pix2cell(agents[0]->getPosition()))<3))
							coinPosition = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));
					}
				}
				else
				{
					Vector2D steering_force = agents[0]->Behavior()->Arrive(agents[0], currentTarget, path.ARRIVAL_DISTANCE, dtime);
					agents[0]->update(steering_force, dtime, event);
				}
				return;
			}
			currentTargetIndex++;
		}

		currentTarget = path.points[currentTargetIndex];
		Vector2D steering_force = agents[0]->Behavior()->Seek(agents[0], currentTarget, dtime);
		agents[0]->update(steering_force, dtime, event);
	}
	else
	{
		agents[0]->update(Vector2D(0, 0), dtime, event);
	}
}

void SceneDjikstra::draw()
{
	drawMaze();
	drawCoin();


	if (draw_grid)
	{
		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 255, 255, 255, 127);
		for (int i = 0; i < SRC_WIDTH; i += CELL_SIZE)
		{
			SDL_RenderDrawLine(TheApp::Instance()->getRenderer(), i, 0, i, SRC_HEIGHT);
		}
		for (int j = 0; j < SRC_HEIGHT; j = j += CELL_SIZE)
		{
			SDL_RenderDrawLine(TheApp::Instance()->getRenderer(), 0, j, SRC_WIDTH, j);
		}
	}

	for (int i = 0; i < (int)path.points.size(); i++)
	{
		draw_circle(TheApp::Instance()->getRenderer(), (int)(path.points[i].x), (int)(path.points[i].y), 15, 255, 255, 0, 255);
		if (i > 0)
			SDL_RenderDrawLine(TheApp::Instance()->getRenderer(), (int)(path.points[i - 1].x), (int)(path.points[i - 1].y), (int)(path.points[i].x), (int)(path.points[i].y));
	}

	draw_circle(TheApp::Instance()->getRenderer(), (int)currentTarget.x, (int)currentTarget.y, 15, 255, 0, 0, 255);

	agents[0]->draw();
}

const char* SceneDjikstra::getTitle()
{
	return "SDL Steering Behaviors :: PathFinding1 Demo";
}

void SceneDjikstra::drawMaze()
{
	if (draw_grid)
	{

		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 0, 0, 255, 255);
		for (unsigned int i = 0; i < maze_rects.size(); i++)
			SDL_RenderFillRect(TheApp::Instance()->getRenderer(), &maze_rects[i]);
	}
	else
	{
		SDL_RenderCopy(TheApp::Instance()->getRenderer(), background_texture, NULL, NULL);
	}
}

void SceneDjikstra::drawCoin()
{
	Vector2D coin_coords = cell2pix(coinPosition);
	int offset = CELL_SIZE / 2;
	SDL_Rect dstrect = { (int)coin_coords.x - offset, (int)coin_coords.y - offset, CELL_SIZE, CELL_SIZE };
	SDL_RenderCopy(TheApp::Instance()->getRenderer(), coin_texture, NULL, &dstrect);
}

void SceneDjikstra::initMaze()
{

	// Initialize a list of Rectagles describing the maze geometry (useful for collision avoidance)
	SDL_Rect rect = { 0, 0, 1280, 32 };
	maze_rects.push_back(rect);
	rect = { 608, 32, 64, 32 };
	maze_rects.push_back(rect);
	rect = { 0, 736, 1280, 32 };
	maze_rects.push_back(rect);
	rect = { 608, 512, 64, 224 };
	maze_rects.push_back(rect);
	rect = { 0,32,32,288 };
	maze_rects.push_back(rect);
	rect = { 0,416,32,320 };
	maze_rects.push_back(rect);
	rect = { 1248,32,32,288 };
	maze_rects.push_back(rect);
	rect = { 1248,416,32,320 };
	maze_rects.push_back(rect);
	rect = { 128,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 288,128,96,32 };
	maze_rects.push_back(rect);
	rect = { 480,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 736,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 896,128,96,32 };
	maze_rects.push_back(rect);
	rect = { 1088,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 128,256,64,32 };
	maze_rects.push_back(rect);
	rect = { 288,256,96,32 };
	maze_rects.push_back(rect);
	rect = { 480, 256, 320, 32 };
	maze_rects.push_back(rect);
	rect = { 608, 224, 64, 32 };
	maze_rects.push_back(rect);
	rect = { 896,256,96,32 };
	maze_rects.push_back(rect);
	rect = { 1088,256,64,32 };
	maze_rects.push_back(rect);
	rect = { 128,384,32,256 };
	maze_rects.push_back(rect);
	rect = { 160,512,352,32 };
	maze_rects.push_back(rect);
	rect = { 1120,384,32,256 };
	maze_rects.push_back(rect);
	rect = { 768,512,352,32 };
	maze_rects.push_back(rect);
	rect = { 256,640,32,96 };
	maze_rects.push_back(rect);
	rect = { 992,640,32,96 };
	maze_rects.push_back(rect);
	rect = { 384,544,32,96 };
	maze_rects.push_back(rect);
	rect = { 480,704,32,32 };
	maze_rects.push_back(rect);
	rect = { 768,704,32,32 };
	maze_rects.push_back(rect);
	rect = { 864,544,32,96 };
	maze_rects.push_back(rect);
	rect = { 320,288,32,128 };
	maze_rects.push_back(rect);
	rect = { 352,384,224,32 };
	maze_rects.push_back(rect);
	rect = { 704,384,224,32 };
	maze_rects.push_back(rect);
	rect = { 928,288,32,128 };
	maze_rects.push_back(rect);

	// Initialize the terrain matrix (for each cell a zero value indicates it's a wall)

	// (1st) initialize all cells to 1 by default
	for (int i = 0; i < num_cell_x; i++)
	{
		vector<int> terrain_col(num_cell_y, 1);
		terrain.push_back(terrain_col);
	}
	// (2nd) set to zero all cells that belong to a wall
	int offset = CELL_SIZE / 2;
	for (int i = 0; i < num_cell_x; i++)
	{
		for (int j = 0; j < num_cell_y; j++)
		{
			Vector2D cell_center((float)(i*CELL_SIZE + offset), (float)(j*CELL_SIZE + offset));
			for (unsigned int b = 0; b < maze_rects.size(); b++)
			{
				if (Vector2DUtils::IsInsideRect(cell_center, (float)maze_rects[b].x, (float)maze_rects[b].y, (float)maze_rects[b].w, (float)maze_rects[b].h))
				{
					terrain[i][j] = 0;
					break;
				}
			}

		}
	}

}

void SceneDjikstra::initNodes() {
	//primero de todo, dimensionamos la matriz que contendrá todos los nodos
	//maze_nodes.resize(num_cell_x);
	for (int i = 0; i < num_cell_x; i++)
	{
		vector<Node*> node_col(num_cell_y, nullptr);
		maze_nodes.push_back(node_col);
	}

	/*Node *node = new Node;
	maze_nodes[2][3] = node;*/

	//ahora que tenemos una matriz del tamaño del mapa, añadimos nodos allá donde no haya muros
	//cada vez que creamos un nodo le añadimos un número identificativo único
	int identificator = 1;  //empezamos por el identificador 1
	for (int i = 0; i < num_cell_x; i++)
	{
		for (int j = 0; j < num_cell_y; j++)
		{
			if (terrain[i][j] != 0)
			{
				Node *node = new Node(identificator, Vector2D(i, j));
				maze_nodes[i][j] = node;
				identificator++;
			}
		}
	}

	//ahora que tenemos una matriz rellena de nodos y espacios vacios, hay que linkear los nodos entre ellos

	for (int i = 0; i < num_cell_x; i++)
	{
		for (int j = 0; j < num_cell_y; j++)
		{
			//primero de todo, comprobamos si la casilla en cuestión es un nodo o una pared
			if (maze_nodes[i][j] != nullptr)
			{
				//ahora comprobaremos si el nodo tiene vecinos. Si los tiene los vincularemos al nodo

				//COMPROBAMOS ARRIBA
				if (i > 0)
				{
					//si hay un nodo, vinculamos el actual con el vecino de arriba
					if (maze_nodes[i - 1][j] != nullptr) maze_nodes[i][j]->LeftNeighbor = maze_nodes[i - 1][j];
				}
				else
				{
					//si sale 0 es que no existe nodo superior, no haremos nada
				}

				//COMPROBAMOS ABAJO
				if (i < num_cell_x - 1)
				{
					//si hay un nodo, vinculamos el actual con el vecino de abajo
					if (maze_nodes[i + 1][j] != nullptr) maze_nodes[i][j]->RightNeighbor = maze_nodes[i + 1][j];
				}
				else
				{
					//si sale 0 es que no existe nodo inferior, no haremos nada
				}

				//COMPROBAMOS IZQUIERDA
				if (j > 0)
				{
					//si hay un nodo, vinculamos el actual con el vecino de la izquierda
					if (maze_nodes[i][j - 1] != nullptr) maze_nodes[i][j]->TopNeighbor = maze_nodes[i][j - 1];
				}
				else
				{
					//si sale 0 es que no existe nodo a la izquierda, no haremos nada
				}

				//COMPROBAMOS DERECHA
				if (j < num_cell_y - 1)
				{
					//si hay un nodo, vinculamos el actual con el vecino de la derecha
					if (maze_nodes[i][j + 1] != nullptr) maze_nodes[i][j]->BottomNeighbor = maze_nodes[i][j + 1];
				}
				else
				{
					//si sale 0 es que no existe nodo a la derecha, no haremos nada
				}
			}
			else
			{
				//si es un muro no haremos nada
			}
		}
	}

	//deberiamos tener todos los nodos linkeados entre ellos
	//Solo faltara linkear los nodos excepcionales que forman el tunel

	//linkeamos la entrada de la izquierda
	maze_nodes[0][10]->LeftNeighbor = maze_nodes[num_cell_x - 1][10];
	maze_nodes[0][11]->LeftNeighbor = maze_nodes[num_cell_x - 1][11];
	maze_nodes[0][12]->LeftNeighbor = maze_nodes[num_cell_x - 1][12];

	//linkeamos la entrada de la derecha
	maze_nodes[num_cell_x - 1][10]->RightNeighbor = maze_nodes[0][10];
	maze_nodes[num_cell_x - 1][11]->RightNeighbor = maze_nodes[0][11];
	maze_nodes[num_cell_x - 1][12]->RightNeighbor = maze_nodes[0][12];
}

//función para checkear si un nodo concreto está contenido en el vector
bool SceneDjikstra::CheckVector(Node* node, std::vector<Node*> vec) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i] == node)
		{
			return true;
		}
	}
	return false;
}

void SceneDjikstra::Algorithm_Djisktra()
{
	//inicializamos la frontera con la posición inicial del jugador
	//para ello lo convertimos a pair (para que se nos auto ordene usando las priority_queues)
	std::pair <int, Node*> element(
		maze_nodes[pix2cell(agents[0]->getPosition()).x][pix2cell(agents[0]->getPosition()).y]->AccCost,
		maze_nodes[pix2cell(agents[0]->getPosition()).x][pix2cell(agents[0]->getPosition()).y]);
	
	nodos_frontera.push(element);
	
	//mientras la frontera no este vacia...
	while (nodos_frontera.size() != 0)
	{

		//obtenemos el primer nodo de la frontera
		//element = nodos_frontera.top();
		Node* nodo = nodos_frontera.top().second;

		//comprobamos si es el nodo destino
		if (nodo->position == coinPosition) //si no va, quiza probar comparando direccion de memoria con &?
		{
			//si efectivamente es el nodo destino paramos el bucle
			break;
		}
		else
		{
			if (CheckVector(nodo, nodos_visitados))
			{
				//si ya se ha visitado, no lo comprobaremos. lo borramos.
				nodos_frontera.pop();
			}
			else
			{
				//si no se ha visitado, lo visitamos

				//borramos el nodo de la frontera
				nodos_frontera.pop();

				//comprobamos los vecinos de tal nodo y los añadimos a la frontera
				//Seguimos el sentido horario
				if (nodo->TopNeighbor != nullptr)
				{
					//comprobamos que no ha sido visitado
					if (CheckVector(nodo->TopNeighbor, nodos_visitados))
					{
						//si ya se ha visitado, no lo comprobaremos.
					}
					else
					{
						
						//linkeamos el nodo actual al siguiente nodo
						nodo->TopNeighbor->PreviousNode = nodo;
						//antes de guardarlo, actualizamos el coste acumulado con el coste para ir al siguiente nodo
						nodo->TopNeighbor->AccCost = nodo->TopNeighbor->cost + nodo->AccCost;
						nodos_frontera.push(std::pair <int, Node*>(nodo->TopNeighbor->AccCost, nodo->TopNeighbor));
					}
				}
				if (nodo->RightNeighbor != nullptr)
				{
					//comprobamos que no ha sido visitado
					if (CheckVector(nodo->RightNeighbor, nodos_visitados))
					{
						//si ya se ha visitado, no lo comprobaremos.
					}
					else
					{
						//linkeamos el nodo actual al siguiente nodo
						nodo->RightNeighbor->PreviousNode = nodo;
						//antes de guardarlo, actualizamos el coste acumulado con el coste para ir al siguiente nodo
						nodo->RightNeighbor->AccCost = nodo->RightNeighbor->cost + nodo->AccCost;
						nodos_frontera.push(std::pair <int, Node*>(nodo->RightNeighbor->AccCost, nodo->RightNeighbor));
					}
				}
				if (nodo->BottomNeighbor != nullptr)
				{
					//comprobamos que no ha sido visitado
					if (CheckVector(nodo->BottomNeighbor, nodos_visitados))
					{
						//si ya se ha visitado, no lo comprobaremos.
					}
					else
					{
						//linkeamos el nodo actual al siguiente nodo
						nodo->BottomNeighbor->PreviousNode = nodo;
						//antes de guardarlo, actualizamos el coste acumulado con el coste para ir al siguiente nodo
						nodo->BottomNeighbor->AccCost = nodo->BottomNeighbor->cost + nodo->AccCost;
						nodos_frontera.push(std::pair <int, Node*>(nodo->BottomNeighbor->AccCost, nodo->BottomNeighbor));
					}
				}
				if (nodo->LeftNeighbor != nullptr)
				{
					//comprobamos que no ha sido visitado
					if (CheckVector(nodo->LeftNeighbor, nodos_visitados))
					{
						//si ya se ha visitado, no lo comprobaremos.
					}
					else
					{
						//linkeamos el nodo actual al siguiente nodo
						nodo->LeftNeighbor->PreviousNode = nodo;
						//antes de guardarlo, actualizamos el coste acumulado con el coste para ir al siguiente nodo
						nodo->LeftNeighbor->AccCost = nodo->LeftNeighbor->cost + nodo->AccCost;
						nodos_frontera.push(std::pair <int, Node*>(nodo->LeftNeighbor->AccCost, nodo->LeftNeighbor));
					}
				}

				//añadimos a la lista de visitados el nodo visitado
				nodos_visitados.push_back(nodo);
			}
		}

	}

	//el nodo destino es el primer nodo de la frontera
	Node* nodo = nodos_frontera.top().second;
	//añadimos al camino a recorrer el nodo destino
	camino_a_recorrer.push_back(nodo);

	//mientras no lleguemos al nodo origen
	Vector2D pos = pix2cell(agents[0]->getPosition());
	while (nodo->position != pos)
	{
		//avanzamos al nodo padre
		nodo = nodo->PreviousNode;
		//lo añadimos al camino a recorrer
		camino_a_recorrer.push_back(nodo);
	}
	//cuando salga tendremos un vector con el camino a recorrer invertido

	//añadimos al vector path.points el recorrido que debe hacer el agente, ordenado
	for (int i = 0; i < camino_a_recorrer.size(); i++)
	{
		path.points.insert(path.points.begin(), cell2pix(camino_a_recorrer[i]->position));
	}


}

bool SceneDjikstra::loadTextures(char* filename_bg, char* filename_coin)
{
	SDL_Surface *image = IMG_Load(filename_bg);
	if (!image) {
		cout << "IMG_Load: " << IMG_GetError() << endl;
		return false;
	}
	background_texture = SDL_CreateTextureFromSurface(TheApp::Instance()->getRenderer(), image);

	if (image)
		SDL_FreeSurface(image);

	image = IMG_Load(filename_coin);
	if (!image) {
		cout << "IMG_Load: " << IMG_GetError() << endl;
		return false;
	}
	coin_texture = SDL_CreateTextureFromSurface(TheApp::Instance()->getRenderer(), image);

	if (image)
		SDL_FreeSurface(image);

	return true;
}

Vector2D SceneDjikstra::cell2pix(Vector2D cell)
{
	int offset = CELL_SIZE / 2;
	return Vector2D(cell.x*CELL_SIZE + offset, cell.y*CELL_SIZE + offset);
}

Vector2D SceneDjikstra::pix2cell(Vector2D pix)
{
	return Vector2D((float)((int)pix.x / CELL_SIZE), (float)((int)pix.y / CELL_SIZE));
}

bool SceneDjikstra::isValidCell(Vector2D cell)
{
	if ((cell.x < 0) || (cell.y < 0) || (cell.x >= terrain.size()) || (cell.y >= terrain[0].size()))
		return false;
	return !(terrain[(unsigned int)cell.x][(unsigned int)cell.y] == 0);
}