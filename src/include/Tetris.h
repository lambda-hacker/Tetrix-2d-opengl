#ifndef _TETRIS_H_
#define _TETRIS_H_

#include <vector>
#include "Color.h"

using std::vector;

namespace cs {
	struct Point2 {
		GLfloat x, y;
		Point2() { }
		Point2(GLfloat x, GLfloat y): x(x), y(y) { }
		Point2(Point2 const& rhs): x(rhs.x), y(rhs.y) { }
		Point2& operator= (Point2 const& rhs) { x = rhs.x; y = rhs.y; return *this; }		
	};

	class TetPiece 
	{
	public:
		TetPiece();
		TetPiece(std::string name, size_t w, size_t h);
		TetPiece& operator= (TetPiece const& rhs);
		
		void set_name(std::string const& s);
		void set_color(std::string const& s);
		void set_colr(ColorRGB const& c);
		void set_dimensions(size_t w, size_t h);
		void set_position(Point2 pos);
		void set_center_index(size_t i);
		
		std::string get_name() const;
		std::string get_color() const;
		ColorRGB get_colr() const;

		Point2 get_center() const;
		Point2 get_position() const;
		size_t get_center_index() const;

		size_t get_width()  const;
		size_t get_height() const;
		
		vector<Point2> const& piece_blocks() const;
		
		TetPiece& rotate(); // +90 degree rotation
		
		TetPiece& move_left();
		TetPiece& move_right();
		TetPiece& move_down();
		
		
		TetPiece& add_block(Point2 p);
		size_t get_current_num_blocks() const;

	private:
		std::string name;
		std::string color;
		size_t width, height;
		size_t center_block_index;
		vector<Point2> occupied_blocks; //Wrt local coordinates
		
		Point2 position;  //x and y position (of centers)
		Point2 center;
		double rot_angle; //either 0.0, +90.0, +180.0, OR +270.0
		ColorRGB colr;

		TetPiece& translate_pt(Point2& p, int dx, int dy);
		TetPiece& rotate_pt(Point2& p);
		
	};


	struct occupancy_cell {
		int v;
		ColorRGB c;
		occupancy_cell() { };
		occupancy_cell(occupancy_cell const& rhs)
		{
			*this = rhs;
		}
		occupancy_cell& operator= (occupancy_cell const& rhs)
		{
			v = rhs.v;
			c = rhs.c;
			return *this;
		}
	};

	class OccupancyGrid 
	{
	public:
		OccupancyGrid(size_t width, size_t height);
		~OccupancyGrid();
		void initialize();
		void deposit(TetPiece const& p);
		size_t check_lines();		
		bool is_game_over() const;
		bool is_valid_cell(int r, int c) const;
		size_t get_height() const;
		size_t get_width() const;
		occupancy_cell get_cell(size_t i, size_t j) const;

	private:
		size_t width, height;
		occupancy_cell** cells;

		void drop_down(size_t line_no);
		void animate_line(size_t line_no);
	};	


	

	//Static Functions
	bool can_move(TetPiece const& p, int dx, int dy, OccupancyGrid const& og);
	bool can_rotate(TetPiece const& p, OccupancyGrid const& og);
	
}

#endif