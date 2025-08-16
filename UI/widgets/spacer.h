#pragma once

#include "common.h"

namespace UI::widgets
	{
	struct spacer : core::element
		{
		virtual void debug_draw(const utils::MS::graphics::d2d::device_context& context, const core::debug_brushes& brushes) const noexcept override
			{
			auto center{rect.get_center()};
			D2D1_POINT_2F hor_a{.x{rect.ll }, .y{center.y}};
			D2D1_POINT_2F hor_b{.x{rect.rr }, .y{center.y}};
			D2D1_POINT_2F ver_a{.x{center.x}, .y{rect.up }};
			D2D1_POINT_2F ver_b{.x{center.x}, .y{rect.dw }};
			context->DrawLine(hor_a, hor_b, brushes.elem_bg.get(), 3.f);
			context->DrawLine(ver_a, ver_b, brushes.elem_bg.get(), 3.f);
			}

		virtual core::widget_obs get_mouseover(utils::math::vec2f position) noexcept override { return nullptr; }

		core::vec2f _get_size_min() const noexcept final override { return {/**/     0,          0}; }
		core::vec2f _get_size_prf() const noexcept final override { return {/**/     0,          0}; }
		core::vec2f _get_size_max() const noexcept final override { return {core::finf, core::finf}; }
		};
	}