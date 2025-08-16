#pragma once

#include <iostream>

#include <utils/enum.h>
#include <utils/input/mouse.h>
#include <utils/math/geometry/circle.h>

#include "common.h"
#include "../containers/one_of.h"

utils_disable_warnings_begin
utils_disable_warning_msvc(4250)
namespace UI::widgets
	{
	class toggle : public core::container_widget<containers::one_of<core::container_own<6>>>
		{
		public:
			struct layers
				{
				core::element_own normal_false;
				core::element_own normal_true ;
				core::element_own down_false  ;
				core::element_own down_true   ;
				core::element_own hover_false ;
				core::element_own hover_true  ;
				};
			toggle(std::function<void(bool)> callback, layers&& layers) : callback{callback} 
				{
				containers::one_of<core::container_own<6>>::set(0, std::move(layers.normal_false));
				containers::one_of<core::container_own<6>>::set(1, std::move(layers.normal_true ));
				containers::one_of<core::container_own<6>>::set(2, std::move(layers.down_false  ));
				containers::one_of<core::container_own<6>>::set(3, std::move(layers.down_true   ));
				containers::one_of<core::container_own<6>>::set(4, std::move(layers.hover_false ));
				containers::one_of<core::container_own<6>>::set(5, std::move(layers.hover_true  ));
				}

			virtual bool on_focus_lose () override { set_pressed(false); return true; }
			virtual bool on_mouse_enter() override { set_hovered(true ); return true; }
			virtual bool on_mouse_leave() override { set_hovered(false); return true; }
			
			virtual bool on_mouse_button(const utils::input::mouse::button_id& id, const bool& state) override
				{
				on_mouse_button_inner(id, state);
				return true;
				}

			std::function<void(bool)> callback;

			void toggle_state() { set_state(!get_state()); }
			void set_state(bool new_state) { _state = new_state; callback(_state); }
			bool get_state() const { return _state; }

			__declspec(property(get = get_state, put = set_state)) bool state;

		private:
			bool pressed{false};
			bool hovered{false};
			bool _state {false};

			void on_mouse_button_inner(const utils::input::mouse::button_id& id, const bool& state)
				{
				if (id == utils::input::mouse::button_id::left)
					{
					if (hovered)
						{
						if (state) { set_pressed(true); return; }
						else if (pressed) { toggle_state(); }
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
				     if (pressed && !_state && elements[2]) { set_current_index(2); }
				else if (pressed &&  _state && elements[3]) { set_current_index(3); }
				else if (hovered && !_state && elements[4]) { set_current_index(4); }
				else if (hovered &&  _state && elements[5]) { set_current_index(5); }
				else if (           !_state && elements[0]) { set_current_index(0); }
				else if (            _state && elements[1]) { set_current_index(1); }
				}
		};
	}
utils_disable_warnings_end