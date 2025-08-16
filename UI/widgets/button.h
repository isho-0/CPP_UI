#pragma once

#include <iostream>

#include <utils/enum.h>
#include <utils/input/mouse.h>
#include <utils/math/geometry/circle.h>

#include "common.h"
#include "../containers/one_of.h"

utils_disable_warnings_begin

namespace UI::widgets
	{
	class button : public core::container_widget<containers::one_of<core::container_own<3>>>
		{
		public:
			struct layers
				{
				core::element_own normal;
				core::element_own down  ;
				core::element_own hover ;
				};
			button(std::function<void()> callback, layers&& layers) : callback{callback}
				{
				containers::one_of<core::container_own<3>>::set(0, std::move(layers.normal));
				containers::one_of<core::container_own<3>>::set(1, std::move(layers.down  ));
				containers::one_of<core::container_own<3>>::set(2, std::move(layers.hover ));
				}

			virtual bool on_focus_lose () override { set_pressed(false); return true; }
			virtual bool on_mouse_enter() override { set_hovered(true ); return true; }
			virtual bool on_mouse_leave() override { set_hovered(false); return true; }
			
			virtual bool on_mouse_button(const utils::input::mouse::button_id& id, const bool& state) override
				{
				on_mouse_button_inner(id, state);
				return true;
				}

			std::function<void()> callback;

		private:
			containers::one_of<core::container_own<3>> graphics;

			bool pressed{false};
			bool hovered{false};

			void on_mouse_button_inner(const utils::input::mouse::button_id& id, const bool& state)
				{
				if (id == utils::input::mouse::button_id::left)
					{
					if (hovered)
						{
						if (state) { set_pressed(true); return; }
						else if (pressed) { callback(); }
						}
					}
				set_pressed(false);
				}

			void set_pressed(bool state) noexcept
				{
				pressed = state;
				update_index();
				}
			void set_hovered(bool state) noexcept
				{
				hovered = state;
				update_index();
				}

			void update_index() noexcept
				{
				     if (pressed && elements[1]) { set_current_index(1); }
				else if (hovered && elements[2]) { set_current_index(2); }
				else if (           elements[0]) { set_current_index(0); }
				}
		};
	}
utils_disable_warnings_end