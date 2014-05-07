#ifndef _COLOR_H_
#define _COLOR_H_

#include <cstdlib>
#include <glut.h>

namespace cs {
	struct ColorRGB {
		GLfloat r, g, b;
		ColorRGB() { }
		ColorRGB(GLfloat r, GLfloat g, GLfloat b) : r(r), g(g), b(b) { }
		ColorRGB(ColorRGB const& rhs) : r(rhs.r), b(rhs.b), g(rhs.g) { }
	};

	enum ColorSaturation { CSATURATION_HIGH = 0, CSATURATION_NORMAL, CSATURATION_LOW };
	enum Colors { C_RED = 0, C_GREEN, C_BLUE, C_CYAN, C_MAGENTA, C_YELLOW, C_GRAY };

	ColorRGB generate_random_color();
}

#endif