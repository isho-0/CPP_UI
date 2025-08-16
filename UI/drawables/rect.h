#pragma once

#include <iostream>

#include <utils/enum.h>
#include <utils/math/geometry/circle.h>
#include <utils/MS/cast.h>

#include "common.h"

namespace UI::drawables
	{
	struct rect : shape
		{
		virtual void _draw(const utils::MS::graphics::d2d::device_context& context) const noexcept override
			{
			if (draw_shape_data.brush_fill_opt)
				{
				context->FillRectangle(utils::MS::graphics::d2d::cast(get_rect()), draw_shape_data.brush_fill_opt->get());
				}
			if (draw_shape_data.brush_outline_opt)
				{
				context->DrawRectangle(utils::MS::graphics::d2d::cast(get_rect()), draw_shape_data.brush_outline_opt->get(), draw_shape_data.outline_thickness);
				}
			}
		};
	struct round_rect : shape
		{
		float rounding_radius{1.f};

		virtual void _draw(const utils::MS::graphics::d2d::device_context& context) const noexcept override
			{
			D2D1_ROUNDED_RECT d2d_roundrect{.rect{utils::MS::graphics::d2d::cast(get_rect())}, .radiusX{rounding_radius}, .radiusY{rounding_radius}};

			if (draw_shape_data.brush_fill_opt)
				{
				context->FillRoundedRectangle(d2d_roundrect, draw_shape_data.brush_fill_opt->get());
				}
			if (draw_shape_data.brush_outline_opt)
				{
				context->DrawRoundedRectangle(d2d_roundrect, draw_shape_data.brush_outline_opt->get(), draw_shape_data.outline_thickness);
				}
			}
		};
	}