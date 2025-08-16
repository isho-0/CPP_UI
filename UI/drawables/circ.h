#pragma once

#include <iostream>

#include <utils/enum.h>
#include <utils/math/geometry/circle.h>

#include "common.h"

namespace UI::drawables
	{
	struct circ : shape
		{
		virtual void _draw(const utils::MS::graphics::d2d::device_context& context) const noexcept override
			{
			//float radius = std::min(rect.width, rect.height) / 2.f;
			//utils::math::geometry::circle circle{.center{rect.center}, .radius{radius}};
			//
			//D2D1_ELLIPSE d2d_ellipse{.point{.x{circle.center.x}, .y{circle.center.y}}, .radiusX{circle.radius}, .radiusY{circle.radius}};
			//graphics::d2d::brush::solid_color _brush{rt, draw_shape_data.fill_colour};
			//rt->FillEllipse(d2d_ellipse, _brush.get());
			}
		};
	}