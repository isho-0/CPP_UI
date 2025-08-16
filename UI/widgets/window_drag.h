#pragma once

#include <iostream>

#include <utils/enum.h>

#include "common.h"
#include <utils/MS/window/window.h>

namespace UI::widgets
	{
	struct window_drag : core::element_widget
		{
		window_drag(utils::MS::window::base& window) : window_ptr{&window} {}

		virtual core::widget_obs get_mouseover(core::vec2f position) noexcept override
			{
			if (rect.contains(position))
				{
				return this;
				}
			return rect.contains(position) ? this : nullptr;
			}

		virtual bool on_mouse_button(const utils::input::mouse::button_id& id, const bool& state) override
			{
			if (id == utils::input::mouse::button_id::left && state)
				{
				ReleaseCapture();
				PostMessage(window_ptr->get_handle(), WM_SYSCOMMAND, SC_SIZE + 9/*undocumented SC_DRAG*/, 0);
				}
			return false;
			}
		
		utils::observer_ptr<utils::MS::window::base> window_ptr;
		};
	}