#pragma once

#include <iostream>

#include <utils/enum.h>
#include <utils/math/geometry/circle.h>

#include "common.h"

namespace UI::drawables
	{
	struct text : shape
		{
		public:
			text(utils::MS::graphics::dw::factory dw_factory, utils::MS::graphics::dw::text_format format, utils::MS::graphics::d2d::brush brush, const std::wstring& string) :
				brush{brush},
				layout{dw_factory, string, format, {}}
				{
				}

			utils::MS::graphics::d2d::brush brush;

			virtual void _draw(const utils::MS::graphics::d2d::device_context& context) const noexcept override
				{
				context->DrawTextLayout(D2D1::Point2F(rect.ll, rect.up), layout.get(), brush.get());
				}

		protected:
			utils::MS::graphics::dw::text_layout  layout;

			virtual void on_resize() noexcept final override
				{
				layout->SetMaxWidth (rect.width ());
				layout->SetMaxHeight(rect.height());
				}
		};
	}