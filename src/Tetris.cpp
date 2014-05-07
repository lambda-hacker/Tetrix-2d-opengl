#include "Tetris.h"

namespace cs {

	TetPiece::TetPiece()
	{ 
		rot_angle = 0.0; 
		position.x = position.y = 0.0; 
		colr = generate_random_color();
	}

	TetPiece::TetPiece(std::string name, size_t w, size_t h)
		: name(name), width(w), height(h) 
	{
		center.x = width / 2;
		center.y = height / 2;
		rot_angle = 0.0;
	}

	TetPiece& TetPiece::operator= (TetPiece const& rhs)
	{
		name = rhs.name;
		color = rhs.color;
		width = rhs.width;
		height = rhs.height;
		center_block_index = rhs.center_block_index;
		position = rhs.position;
		center = rhs.center;
		rot_angle = rhs.rot_angle;
		colr = rhs.colr;

		occupied_blocks.clear();
		for (size_t i = 0; i < rhs.occupied_blocks.size(); i++)
			occupied_blocks.push_back(rhs.occupied_blocks[i]);

		return *this;
	}
		
	void TetPiece::set_name(std::string const& s)
	{
		name = s;
	}
	void TetPiece::set_color(std::string const& s)
	{
		color = s;
	}
	void TetPiece::set_colr(ColorRGB const& c)
	{
		colr = c;
	}
	void TetPiece::set_dimensions(size_t w, size_t h)
	{
		width = w;
		height = h;
		center.x = w / 2;
		center.y = h / 2;
	}

	void TetPiece::set_position(Point2 pos)
	{
		position = pos;
	}
		
	std::string TetPiece::get_name() const
	{
		return name;
	}
	
	std::string TetPiece::get_color() const
	{
		return color;
	}

	ColorRGB TetPiece::get_colr() const
	{
		return colr;
	}
	
	Point2 TetPiece::get_center() const
	{
		return center;
	}
	
	size_t TetPiece::get_width() const
	{
		return width;
	}
	
	size_t TetPiece::get_height() const
	{
		return height;
	}

	Point2 TetPiece::get_position() const
	{
		return position;
	}

	size_t TetPiece::get_center_index() const
	{
		return center_block_index;
	}
	
	vector<Point2> const& TetPiece::piece_blocks() const
	{
		return occupied_blocks;
	}
		
	TetPiece& TetPiece::rotate()
	{
		if (name.compare("O") != 0)
		for (size_t i = 0; i < occupied_blocks.size(); i++)
			rotate_pt(occupied_blocks[i]);
		
		rot_angle += 90.0;
		if (rot_angle >= 360.0)
			rot_angle -= 360.0;

		return *this;
	}
		
	TetPiece& TetPiece::move_down()
	{
		position.y -= 1.0;
		return *this;
	}

	TetPiece& TetPiece::move_left()
	{
		position.x -= 1.0;
		return *this;
	}

	TetPiece& TetPiece::move_right()
	{
		position.x += 1.0;
		return *this;
	}
		
	void TetPiece::set_center_index(size_t i)
	{
		center_block_index = i;
	}
		
	TetPiece& TetPiece::add_block(Point2 p)
	{
		occupied_blocks.push_back(p);
		return *this;
	}
		
	size_t TetPiece::get_current_num_blocks() const
	{
		return occupied_blocks.size();
	}

	
	//Private Functions
	TetPiece& TetPiece::translate_pt(Point2& p, int dx, int dy)
	{
		GLfloat trans_matrix[3][3] = {{1.0f, 0.0f, dx},
									  {0.0f, 1.0f, dy},
									  {0.0f, 0.0f, 1.0f}};
		Point2 np;
		np.x = p.x * trans_matrix[0][0] + p.y * trans_matrix[0][1] + trans_matrix[0][2];
		np.y = p.x * trans_matrix[1][0] + p.y * trans_matrix[1][1] + trans_matrix[1][2];
		p = np;
		return *this;
	}

	TetPiece& TetPiece::rotate_pt(Point2& p)
	{
		//Rotation matrix with homogenous coordinate
		GLfloat rot_matrix[3][3] = {{0.0f, -1.0f, 0.0f},    //[p.x]
									{1.0f, 0.0f, 0.0f },    //[p.y]
									{0.0f, 0.0f, 1.0f }};   //[ 1 ]
		Point2 np;
		np.x = -p.y; //p.x * rot_matrix[0][0] + p.y * rot_matrix[0][1] + rot_matrix[0][2];
		np.y = p.x;  //p.x * rot_matrix[1][0] + p.y * rot_matrix[1][1] + rot_matrix[1][2];

		p = np;

		return *this;
	}

	/////////////////////////////////////////////////////////////
			
					// OCCUPANCY GRID //

	/////////////////////////////////////////////////////////////
    
    OccupancyGrid::OccupancyGrid(size_t width, size_t height) : width(width), height(height)
	{
		cells = new occupancy_cell*[height];
		for (size_t i = 0; i < height; i++)
			cells[i] = new occupancy_cell[width];

		initialize();
	}

	OccupancyGrid::~OccupancyGrid()
	{
		for (size_t i = 0; i < height; i++)
			delete [] cells[i];
		delete [] cells;
	}

	void OccupancyGrid::initialize()
	{
		for (size_t i = 0; i < height; i++)
		{
			for (size_t j = 0; j < width; j++)
			{
				cells[i][j].c = ColorRGB(0.0f, 0.0f, 0.0f);
				cells[i][j].v = -1;
			}
		}
	}

	bool OccupancyGrid::is_valid_cell(int r, int c) const
	{
		if (r < 0 || r > height - 1)
			return false;

		if (c < 0 || c > width - 1)
			return false;
		return true;
	}
	size_t OccupancyGrid::get_height() const 
	{
		return height;
	}
	size_t OccupancyGrid::get_width() const
	{
		return width;
	}
	occupancy_cell OccupancyGrid::get_cell(size_t i, size_t j) const
	{
		return cells[i][j];
	}

	int to_int(double v) { return (int)v; }
	void OccupancyGrid::deposit(TetPiece const& p)
	{
		Point2 pos = p.get_position();
		vector<Point2> const& p_blocks = p.piece_blocks();
		for (size_t i = 0; i < p_blocks.size(); i++)
		{
			int x = pos.x - 1 + p_blocks[i].x;
			int y = pos.y - 1 + p_blocks[i].y;
			cells[y][x].v = 1;
			cells[y][x].c = p.get_colr();
		}
	}

	size_t OccupancyGrid::check_lines()
	{
		size_t lines_cleared = 0;
		for (size_t i = 0; i < height; i++)
		{
			size_t cell_count = 0;
			for (size_t j = 0; j < width; j++)
			{
				if (cells[i][j].v == -1)
					break;
				cell_count++;
			}
			if (cell_count == width) 
			{
			//	animate_line(i);
				drop_down(i);
				i--;
				lines_cleared++;
			}
		}

		return lines_cleared;
	}

	//Private Functions
    void OccupancyGrid::drop_down(size_t line_no)
	{
		for (size_t i = line_no; i < height - 1; i++)
		{
			for (size_t j = 0; j < width; j++)
			{
				cells[i][j] = cells[i + 1][j];
			}
		}
		
		for (size_t i = 0; i < width; i++)
		{
			cells[height - 1][i].v = -1;
			cells[height - 1][i].c = ColorRGB(0.0f, 0.0f, 0.0f);
		}
	}
    
    // To Do
    /* void OccupancyGrid::animate_line(size_t ln)
	{
		bool done = false;
		while (!done)
		{
			for (size_t i = 0; i < width; i++)
			{
				ColorRGB clr = cells[ln][i].c;
				cells[ln][i].c = ColorRGB(clr.r - 0.005, clr.g - 0.005, clr.b - 0.005);
				if ((clr.r < 0.005) && (clr.g < 0.005) && (clr.b < 0.005))
				{
					done = true;
				}
			}

		}
	} */



    ///////////////////////////////////////////
    //          Static Functions
    //////////////////////////////////////////

    bool can_move(TetPiece const& p, int dx, int dy, OccupancyGrid const& og)
	{
		Point2 pos = p.get_position();
		pos.x -= 1;
		pos.y -= 1;

		vector<Point2> const& blocks = p.piece_blocks();
		for (size_t i = 0; i < blocks.size(); i++)
		{
			int r_index = pos.y + dy + blocks[i].y;
			int c_index = pos.x + dx + blocks[i].x;
			
			if (!og.is_valid_cell(r_index, c_index))
				return false;

			occupancy_cell cell = og.get_cell(r_index, c_index);
			if (cell.v != -1)
				return false;
		}
		
		return true;
	}

	bool can_rotate(TetPiece const& p, OccupancyGrid const& og)
	{
		//To Do
		TetPiece tp = p;
		tp.rotate();
		Point2 pos = tp.get_position();
		pos.x -= 1;
		pos.y -= 1;

		vector<Point2> const& blocks = tp.piece_blocks();
		for (size_t i = 0; i < blocks.size(); i++)
		{
			int r_index = pos.y + blocks[i].y;
			int c_index = pos.x + blocks[i].x;

			if (!og.is_valid_cell(r_index, c_index))
				return false;

			occupancy_cell cell = og.get_cell(r_index, c_index);
			if (cell.v != -1)
				return false;
		}
		return true;
	}
}