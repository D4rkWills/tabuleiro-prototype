// sistema de manipulação de eventos

#include "event-handler.h"

#include <iostream>

#include "global.h"
#include "config.h"

#include "assets/maths.h"
#include "assets/square.h"
#include "assets/piece.h"

void onMouseWheel(SDL_Event* event) {
	if (event->wheel.y < 0) { // diminui o zoom
		cam_scale-= SCALE_MOD_RATE;
	} else { // aumenta o zoom
		cam_scale+= SCALE_MOD_RATE;
	}

	if (SQUARE_SIZE * cam_scale < MINIMUM_SQUARE_SIZE) cam_scale = (double)MINIMUM_SQUARE_SIZE / SQUARE_SIZE;
}

void onMouseMove(SDL_Event* event) {
	pointer_x = event->motion.x;
	pointer_y = event->motion.y;

	if (!mouseReleasedRight) {
		cam_x = last_cam_x - (event->motion.x - last_click_x_right) * 1 / cam_scale;
		cam_y = last_cam_y - (event->motion.y - last_click_y_right) * 1 / cam_scale;
	}
}

void onMouseClick(SDL_Event* event, int dir) {
	int w_width, w_height;

	SDL_GetWindowSize(window, &w_width, &w_height); // captura dimensões da janela

	double pointer_map_x = cam_x + (pointer_x - w_width / 2) * 1 / cam_scale;
	double pointer_map_y = cam_y + (pointer_y - w_height / 2) * 1 / cam_scale;

	Square_data square = findSquare(pointer_map_x, pointer_map_y);

	if (event->button.button == SDL_BUTTON_RIGHT)
		mouseReleasedRight = (dir == 1);
	else
		mouseReleasedLeft = (dir == 1);

	//

	if (event->button.button == SDL_BUTTON_RIGHT) { // movimento do mapa
		if (dir == -1) {
			// salvando últimas posições

			last_click_x_right = event->button.x;
			last_click_y_right = event->button.y;

			// marcando última posição da câmera

			last_cam_x = cam_x;
			last_cam_y = cam_y;
		}
	} else if (event->button.button == SDL_BUTTON_LEFT) { // movendo personagens
		if (dir == -1) {
			if (pieceSellected == NULL) {
				for (size_t i = 0; i < pieceList.size(); i++) { // buscando peça
					if (pieceList[i]->x == square.x && pieceList[i]->y == square.y) {
						pieceSellected = pieceList[i];

						pieceSellected->transparent = true;
						enlighten_pointer_square = true;
						piece_moving = true;

						break;
					}
				}
			} else {
				bool over_piece = false;
				bool in_range = false;

				for (Piece* piece : pieceList) { // verificando se está sobre outra peça
					if (piece->x == square.x && piece->y == square.y) {
						over_piece = true;
						break;
					}
				}

				// verificando distância máxima

				int dist = distance(square.x, square.y, pieceSellected->x, pieceSellected->y);

				in_range = (dist <= pieceSellected->walk_dist);

				if (!over_piece && in_range) {
					//atualizando posição

					pieceSellected->x = square.x;
					pieceSellected->y = square.y;

					// resetando estado

					pieceSellected->transparent = false;

					pieceSellected = NULL;
					enlighten_pointer_square = false;
					piece_moving = false;
				}
			}

		}
	}
}

//

void onKeyboard(SDL_Event* event, int dir) {
	// cancelando movimento de peças

	if (event->key.keysym.sym == SDLK_ESCAPE && pieceSellected && dir == 1) {
		pieceSellected->transparent = false;

		pieceSellected = NULL;
		enlighten_pointer_square = false;
		piece_moving = false;
	}
}

//

void handleEvents(SDL_Event* event) {
	switch (event->type) {
		case SDL_MOUSEWHEEL:
			onMouseWheel(event);
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			onMouseClick(event, (event->type == SDL_MOUSEBUTTONDOWN)? -1 : 1);
			break;

		case SDL_MOUSEMOTION:
			onMouseMove(event);
			break;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			onKeyboard(event, (event->type == SDL_KEYDOWN)? -1 : 1);
			break;

	}
}
