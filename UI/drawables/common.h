#pragma once

#include "../core/core.h"

namespace UI::drawables
	{
	struct draw_shape_data
		{
		std::optional<utils::MS::graphics::d2d::brush> brush_fill_opt;
		std::optional<utils::MS::graphics::d2d::brush> brush_outline_opt;
		float outline_thickness{1.f};
		};

	struct shape : virtual core::element
		{
		draw_shape_data draw_shape_data;
		};
	}