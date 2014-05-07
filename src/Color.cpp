#include "Color.h"

namespace cs {

	ColorRGB generate_random_color()
	{
		size_t index = rand() % 3;
		GLfloat sat = 1.0f;
		if (index == CSATURATION_HIGH) sat = 1.0f;
		else if (index == CSATURATION_NORMAL) sat = 0.6f;
		else if (index == CSATURATION_LOW) sat = 0.3f;

		GLfloat r, g, b;
		index = rand() % 7;

		switch (index)
		{
		case C_RED: 
			r = 1.0f; g = 0.0f, b = 0.0f;
			break;

		case C_GREEN: 
			r = 0.0f; g = 1.0f; b = 0.0f;
			break;

		case C_BLUE:
			r = 0.0f; g = 0.0f; b = 1.0f;
			break;

		case C_CYAN:
			r = 0.0f; g = 1.0f; b = 1.0f;
			break;

		case C_MAGENTA:
			r = 1.0f; g = 0.0f; b = 1.0f;
			break;

		case C_YELLOW:
			r = 1.0f; g = 1.0f; b = 0.0f;
			break;

		default:
			r = 1.0f; g = 1.0f; b = 1.0f;
			break;
		};

		return ColorRGB(sat * r, sat * g, sat * b);
	}
}