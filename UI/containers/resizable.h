#pragma once

#include <iostream>

#include <utils/enum.h>
#include <utils/input/mouse.h>
#include <utils/math/geometry/circle.h>

#include "common.h"

namespace UI::containers
	{
	template <core::concepts::wrapper wrapper_t = core::wrapper_own>
	class resizable : public core::container_widget<wrapper_t>
		{
		private:
			enum class state_t { resizing_hor, resizing_ver, none };

		public:
			bool enable_horizontal{true};
			bool enable_vertical  {true};
			inline static float distance{4.f};

			virtual core::widget_obs get_mouseover(core::vec2f position) noexcept override
				{
				if (auto element_mouseover{wrapper_t::get_mouseover(position)}) { return element_mouseover; }
				if (get_region(position) != state_t::none) { return this; }
				return nullptr;
				}

			virtual bool on_focus_lose () override { pressed = false; return false; }
			virtual bool on_mouse_move(core::vec2f position) override 
				{
				std::cout << "pressed: " << pressed << ", state: " << static_cast<int>(state) << "\n";
				if (!pressed) 
					{
					origin = position;
					state = get_region(position);
					update_cursor(state);
					}
				else if(state != state_t::none)
					{
					float new_pos = (state == state_t::resizing_hor) ? position.x : position.y;
					float delta{new_pos - source_pos};
					float new_value{source_value + delta};
					std::cout << new_pos << ", " << delta << ", " << new_value << "\n";
					switch (state)
						{
						case state_t::resizing_hor: core::element::sizes.set_x(new_value, new_value, new_value); break;
						case state_t::resizing_ver: core::element::sizes.set_y(new_value, new_value, new_value); break;
						}
					}
				return false; 
				}
			virtual bool on_mouse_leave() override { update_cursor(state_t::none); return false; }
			
			virtual bool on_mouse_button(const utils::input::mouse::button_id& id, const bool& state) override
				{
				if (id == utils::input::mouse::button_id::left)
					{
					pressed = state;
					if (state && this->state != state_t::none)
						{
						source_value = (this->state == state_t::resizing_hor) ? core::element::rect.rr : core::element::rect.dw;
						source_pos   = (this->state == state_t::resizing_hor) ? origin.x               : origin.y;
						}

					return false;
					}
				else
				if (id == utils::input::mouse::button_id::right)
					{//reset to no custom sizes, which lets the element's sizes through
					if (this->state != state_t::none)
						{
						switch (this->state)
							{
							case state_t::resizing_hor:
								core::element::sizes.min_x = 0;
								core::element::sizes.prf_x = core::fnan;
								core::element::sizes.max_x = core::finf;
								break;
							case state_t::resizing_ver:
								core::element::sizes.min_y = 0;
								core::element::sizes.prf_y = core::fnan;
								core::element::sizes.max_y = core::finf;
								break;
							}
						//core::element::sizes = core::custom_sizes_t{};
						}
					}
				pressed = false;

				return false;
				}

			std::function<void()> callback;

		private:
			state_t state{state_t::none};
			core::vec2f origin;
			float source_value;
			float source_pos;

			bool pressed{false};

			state_t get_region(core::vec2f position)
				{
				if (position.x <= core::element::rect.rr + distance && position.x >= core::element::rect.rr - distance &&
				    position.x >= core::element::rect.up && position.y <= core::element::rect.dw)
					{
					return state_t::resizing_hor; 
					}
				if (position.x <= core::element::rect.rr && position.x >= core::element::rect.ll - distance &&
				    position.x >= core::element::rect.up - distance && position.y <= core::element::rect.up + distance)
					{
					return state_t::resizing_ver;
					}
				return state_t::none;
				}

			void update_cursor(state_t region)
				{
				HCURSOR cursor{nullptr};

				switch (region)
					{
					case state_t::resizing_hor:
						cursor = LoadCursor(NULL, IDC_SIZEWE);
						break;
					case state_t::resizing_ver:
						cursor = LoadCursor(NULL, IDC_SIZENS);
						break;
					default:
						cursor = LoadCursor(NULL, IDC_ARROW);
					}
				SetCursor(cursor);
				}

		};

	}