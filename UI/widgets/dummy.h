#pragma once

#include <iostream>

#include <utils/enum.h>
#include <utils/input/mouse.h>
#include <utils/math/geometry/circle.h>

#include "common.h"

namespace UI::widgets
	{
	struct dummy : core::element_widget
		{
		virtual void _draw(const utils::MS::graphics::d2d::device_context& context) const noexcept override
			{
			utils::MS::graphics::d2d::solid_brush tmp{context, utils::graphics::colour::rgba_f{0.f, 0.f, 1.f, 1.f}};
			context->FillEllipse(D2D1_ELLIPSE{.point{.x{get_rect().center.x}, .y{get_rect().center.y}}, .radiusX{get_rect().width() / 2.f}, .radiusY{get_rect().height() / 2.f}}, tmp.get());
			}
		
		virtual bool on_focus_gain () override { std::cout << "on_focus_gain " << std::endl; return false; }
		virtual bool on_focus_lose () override { std::cout << "on_focus_lose " << std::endl; return false; }
		virtual bool on_mouse_enter() override { std::cout << "on_mouse_enter" << std::endl; return false; }
		virtual bool on_mouse_leave() override { std::cout << "on_mouse_leave" << std::endl; return false; }
		virtual bool on_mouse_button(const utils::input::mouse::button_id& id, const bool& state) override 
			{
			std::cout << "on_mouse_button " << utils::enums::enum_name<utils::input::mouse::button_id>(id) << " " << (state ? "pressed" : "released") << std::endl;
			return false;
			}
		};
	}