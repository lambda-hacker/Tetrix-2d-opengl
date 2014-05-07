/*
	Author/Programmer: Syed M. Madani
*/

#include <iostream>
#include <cstdlib>
#include <glut.h>
#include <vector>
#include <ctime>
#include <conio.h>


#include "Tetris.h"
#include "Color.h"

using std::vector;
using namespace cs;

/* Globals */
double PI = 3.141592653589793;

vector<TetPiece> tetris_pieces;
TetPiece curr_piece, next_piece;

size_t board_w = 10, board_h = 20;
OccupancyGrid game_grid(board_w, board_h);

//Veiwing Window Globals
GLdouble vw_xmin, vw_xmax, vw_ymin, vw_ymax;

//Gameplay state variables
int drop_delay = 800; //milliseconds
bool fast_drop = false;
size_t lines_cleared = 0;
size_t level = 1;
size_t max_score = 0, score = 0;
bool shadow_enable = false;
bool is_game_over  = false;

// GLUT callback functions
void initialize();
void reshape_handler(int w, int h);
void display_handler();
void keyboard_handler(unsigned char key, int x, int y);
void special_keys_handler(int key, int x, int y);
void game_update_handler();

//Game Related Functions
int load_piece_config_file(std::string file_name, vector<TetPiece>& tetris_pieces);
int load_max_score(std::string file_name);
void save_max_score();
void game_reset();
void generate_random_piece(TetPiece& p);

int main(int argc, char** argv)
{
	
	/*if (argc < 2) {
		std::cout << "Usage: tetris <pieces file> [<shadow enable>]" << std::endl;
		exit(0);
	}*/

	if (argc == 3) {
		shadow_enable = atoi(argv[2]);
	}

	if (load_piece_config_file("PieceConfig.bin", tetris_pieces) == -1) 
	{
		std::cerr << "Error loading Pieces Configuration file" << std::endl;
		exit(1);
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Tetris Clone");

	vw_xmin = vw_ymin = 0.0;
	vw_xmax = 2 * (board_w + 2);
	vw_ymax = board_h + 6;

	generate_random_piece(curr_piece);
	generate_random_piece(next_piece);	
	load_max_score("max_score.bin");
	
	initialize();		
	glutDisplayFunc(display_handler);
	glutReshapeFunc(reshape_handler);
	glutKeyboardFunc(keyboard_handler);
	glutSpecialFunc(special_keys_handler);
	glutIdleFunc(game_update_handler);
	glutMainLoop();

	return 0;
}

void initialize()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(vw_xmin, vw_xmax, vw_ymin, vw_ymax);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void generate_random_piece(TetPiece& p)
{
	srand(time(NULL));
	size_t r1 = rand() % 211;
	size_t r2 = rand() % 31;
	size_t index = (r1 + r2) % tetris_pieces.size();
	p = tetris_pieces[index];
	p.set_position(Point2(board_w / 2, board_h - p.get_center().y + 1 ));
}


int load_piece_config_file(std::string file_name, vector<TetPiece>& tetris_pieces)
{
	FILE* piece_file = fopen(file_name.c_str(), "r");
	if (piece_file == NULL) {
	   	return -1;
	}

	char buffer[30];
	while (fscanf(piece_file, "%s", buffer) != EOF)
	{
		if (strcmp(buffer, "NUM_PIECES") == 0)
			break;
	}
	size_t n;
	size_t w, h;

	fscanf(piece_file, "%u", &n);
	//std::cout << n << std::endl;
	
	fscanf(piece_file, "%s", buffer);
	fscanf(piece_file, "%u", &w);
	//std::cout << w << std::endl;

	fscanf(piece_file, "%s", buffer);
	fscanf(piece_file, "%u", &h);
	//std::cout << h << std::endl;

	
	for (size_t i = 0; i < n; ++i)
	{
		TetPiece piece;
		piece.set_dimensions(w, h);
		std::string s;

		fscanf(piece_file, "%s", buffer);
		//std::cout << buffer << std::endl;
		//Reading piece name
		fscanf(piece_file, "%s", buffer);
		s = buffer;
		piece.set_name(s);
		//
		fscanf(piece_file, "%s", buffer);
		//Reading piece color
		fscanf(piece_file, "%s", buffer);
		s = buffer;
		piece.set_color(s);

		int v;
		for (size_t i = 0; i < h; i++)
		{
			for (size_t j = 0; j < w; j++)
			{
				fscanf(piece_file, "%d", &v);
				if (v == 1)
				{
					Point2 pt;
					pt.x = j - piece.get_center().x ;
					pt.y = i - piece.get_center().y;
					piece.add_block(pt);

					if (pt.x == 0.0f && pt.y == 0.0f) //Set the center index
						piece.set_center_index(piece.get_current_num_blocks() - 1);
				}
			}
		}
		tetris_pieces.push_back(piece);
	}
	
	fclose(piece_file);
	return 1;
}


int load_max_score(std::string file_name)
{
	FILE* score_file = fopen(file_name.c_str(), "rb");
	if (score_file == NULL) return -1;
	fread(&max_score, sizeof(size_t), 1, score_file);
	fclose(score_file);
	return 1;
}
void save_max_score()
{
	FILE* score_file = fopen("max_score.bin", "wb");
	if (score_file == NULL) return;
	fwrite(&max_score, sizeof(size_t), 1, score_file);
	fclose(score_file);
}
void draw_unit_block(GLfloat x, GLfloat y)
{
	glBegin(GL_POLYGON);
	glVertex2d(x + 0.1, y + 0.1);
	glVertex2d(x + 1.0, y + 0.1);
	glVertex2d(x + 1.0, y + 1.0);
	glVertex2d(x + 0.1 , y + 1.0);
	glEnd();
}

void draw_tet_piece(TetPiece const& curr_p)
{
	Point2 pos;
	pos = curr_p.get_position();
	vector<Point2> const& blocks = curr_p.piece_blocks();

	ColorRGB c = curr_p.get_colr();
	glColor3f(c.r, c.g, c.b);
	for (size_t i = 0; i < blocks.size(); i++)
	{
		draw_unit_block(pos.x + blocks[i].x, pos.y + blocks[i].y);
	}
	
}
void draw_occupancy_grid(OccupancyGrid const& og)
{
	for (size_t i = 0; i < og.get_height(); i++)
	{
		for (size_t j = 0; j < og.get_width(); j++)
		{
			occupancy_cell cell = og.get_cell(i, j);
			if (cell.v != -1)
			{
				glColor3f(cell.c.r, cell.c.g, cell.c.b);
				draw_unit_block(j + 1, i + 1);
			}
		}
	}
}
void draw_shadow(TetPiece const& curr_p, OccupancyGrid const& o)
{
	TetPiece p = curr_p;
	while (can_move(p, 0, -1, game_grid))
		p.move_down();
	p.set_colr(ColorRGB(0.03, 0.03, 0.03));
	draw_tet_piece(p);
}
void draw_stroked_string(char* str)
{
	glPushMatrix();
	glScalef(.0085, .0085, .0085);
	char* c = str;
	size_t i = 0;
	while (c[i] != '\0')
	{
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, c[i]);
		i++;
	}
	glPopMatrix();
}
void display_handler()
{
    glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	//Drawing Borders
	for (size_t i = 0; i <= board_w + 1; i++)
	{
		glColor3f(0.2, 0.2, 0.2);
		draw_unit_block(i, 0);
		glColor3f(1.0, 1.0, 1.0);
		draw_unit_block(i, board_h + 1);
	}
	for (size_t j = 1; j < board_h + 1; j++)
	{
		glColor3f(0.05 * j, 0.05 * j, 0.05 * j);
		draw_unit_block(0, j);
		draw_unit_block(board_w + 1, j);
	}

	//Drawing occupancy grid i.e. The current state of pieces fallen
	draw_occupancy_grid(game_grid);

	//Drawing Shadow if enabled
	if (shadow_enable && !is_game_over)
		draw_shadow(curr_piece, game_grid);

	//Drawing current piece i.e. the one that's falling
	if (!is_game_over) draw_tet_piece(curr_piece);

	//Drawing next piece
	glPushMatrix();
	glTranslated(0, 4, 0);	
	draw_tet_piece(next_piece);
	glPopMatrix();

	//Drawing Text, score, level, max score, keys
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(board_w / 2 + 4, board_h + 4, 0);
	glScalef(2.0, 2.0, 1.0);
	draw_stroked_string("TETRIX");
	glPopMatrix();


	if (is_game_over)
	{
		glPushMatrix();
		glTranslatef(board_w / 3 - 1, (board_h / 2) + 4, 0);
		glColor3f(1.0, 1.0, 1.0);
		draw_stroked_string("GAME OVER!");
		glPopMatrix();
	}

	glPushMatrix();

		glPushMatrix();
		glTranslatef(0, board_h + 4, 0);
		glColor3f(0.5, 0.5, 0.5);
		draw_stroked_string("Next ");
		glPopMatrix();

	glColor3f(0.7f, 0.5f, 0.4f);
	glTranslatef(board_w + 3, board_h, 0);
	draw_stroked_string("Max Score: ");

	char buff[10];
	sprintf(buff, "%d", max_score);
	glTranslatef(0, -1.5, 0);
	draw_stroked_string(buff);
	
	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(0, -3, 0);
	draw_stroked_string("Score: ");
	glPushMatrix();
		glTranslatef(5.5, 0, 0);
		sprintf(buff, "%d", score);
		draw_stroked_string(buff);
	glPopMatrix();

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(0, -2, 0);
	draw_stroked_string("Lines: ");
	glPushMatrix();
		glTranslatef(5.5, 0, 0);
		sprintf(buff, "%d", lines_cleared);
		draw_stroked_string(buff);
	glPopMatrix();


	glColor3f(0.0f, 1.0f, 0.0f);
	glTranslatef(0, -2, 0);
	draw_stroked_string("Level: ");
	glPushMatrix();
		glTranslatef(5.5, 0, 0);
		sprintf(buff, "%d", level);
		draw_stroked_string(buff);
	glPopMatrix();


	glColor3f(0.0f, 0.0f, 1.0f);
	glTranslatef(0, -3, 0);
	draw_stroked_string("----Keys----");

	glTranslatef(0, -1.5, 0);
	draw_stroked_string("Arrows: Move");

	glTranslatef(0, -1.5, 0);
	draw_stroked_string("Up: Rotate");

	glTranslatef(0, -1.5, 0);
	draw_stroked_string("Space: Drop");

	glTranslatef(0, -1.5, 0);
	draw_stroked_string("S: Shadow");

	glTranslatef(0, -1.5, 0);
	draw_stroked_string("ESC: Quit");

	glPopMatrix();

	glFlush();
}

void reshape_handler(int w, int h)
{
	glViewport(0, 0, 600, 600);
	glutPostRedisplay();
}

void keyboard_handler(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: exit(0);
		break;

	case 32: if (!is_game_over) {
		     fast_drop = true; 
		     drop_delay = 25;
		}
		      /*while (can_move(curr_piece, 0, -1, game_grid))
			 {
			     //curr_piece.move_down();
			     //glutPostRedisplay();
				  game_update_handler();				 
			  }*/
		break;

    case 13: if (is_game_over) game_reset();
        break; 

	case 'S':
	case 's': shadow_enable = !shadow_enable;
		break;


	case 'H':
	case 'h':
		std::cout << "----------------------KEYS----------------------" << std::endl;
		std::cout << "   LEFT,  RIGHT,  DOWN  ARROWS = TO MOVE        " << std::endl;
		std::cout << "   UP ARROW					 = TO ROTATE      " << std::endl;
		std::cout << "   SPACE						 = DROP PIECE     " << std::endl;
		std::cout << "   S							 = TOGGLE SHADOW  " << std::endl;
		std::cout << "   ESC						 = QUIT			  " << std::endl;
		
		break;
	default:
		break;
	};
}

void special_keys_handler(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: if (can_move(curr_piece, -1, 0, game_grid))
						{
							curr_piece.move_left();
							glutPostRedisplay();
						}
		break;

	case GLUT_KEY_RIGHT: if (can_move(curr_piece, 1, 0, game_grid))
						 {
							curr_piece.move_right();
							glutPostRedisplay();
						 }						
		break;

	case GLUT_KEY_DOWN: if (can_move(curr_piece, 0, -1, game_grid))
						{
							curr_piece.move_down();
							glutPostRedisplay();
						}
		break;

	case GLUT_KEY_UP:   
		if (can_rotate(curr_piece, game_grid) && !is_game_over)
		{
			curr_piece.rotate();
			glutPostRedisplay();
		}
		break;

	case GLUT_KEY_F1:
		break;
	 
	};
}


void game_reset()
{
	level = 1;
	drop_delay = 800;
	score = 0;
	lines_cleared = 0;
	is_game_over = false;
	if (shadow_enable) shadow_enable = false;
	game_grid.initialize();
	generate_random_piece(curr_piece);
	generate_random_piece(next_piece);	
	
}
void game_update_handler()
{

	if (is_game_over) return;

	static int prev_time = glutGet(GLUT_ELAPSED_TIME);
	int curr_time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed_time = curr_time - prev_time;

	if (elapsed_time <= drop_delay) return;
	prev_time = curr_time;

	if (!can_move(curr_piece, 0, 0, game_grid))
	{
		std::cout << "Game Over!!!" << std::endl;
		save_max_score();
 		is_game_over = true;
		display_handler();
		return;
		//exit(0);
	}

	if (can_move(curr_piece, 0, -1, game_grid))
	{
		curr_piece.move_down();  
	}
	else {
		game_grid.deposit(curr_piece);
		glutPostRedisplay();
		
		curr_piece = next_piece;
		generate_random_piece(next_piece);

		if (fast_drop) { 
			fast_drop = false;
			drop_delay = 900 - level * 100;
	    }

		size_t num_lines_cleared = game_grid.check_lines();
		if (num_lines_cleared == 0) return;
		lines_cleared += num_lines_cleared;

		if (lines_cleared >= 15) {
			lines_cleared = 0;
			level += 1;

			if (drop_delay > 100)
				drop_delay -= 100;
		}

		score += level * num_lines_cleared * num_lines_cleared * 10;
		
		if (score > max_score)
			max_score = score;
	}

	glutPostRedisplay();
}


 