#pragma once

#include <iostream>

#include <utils/enum.h>
#include <utils/input/mouse.h>
#include <utils/math/geometry/circle.h>

#include <utils/MS/window/window.h>
#include <utils/MS/window/style.h>
#include <utils/MS/window/input/mouse.h>
#include <utils/MS/graphics/d2d/window.h>

#include "common.h"
#include "button.h"
#include "../window.h"
#include "../containers/input.h"

namespace UI::widgets
	{
	class dropdown : public button
		{
		public:
			dropdown(core::element_own&& dropdown_root, utils::MS::graphics::d2d::device& d2d_device, button::layers&& layers) : 
				button{[this]() { spawn_window(); }, std::move(layers)}, 
				root{std::move(dropdown_root)},
				d2d_device{d2d_device}
				{}


			core::element_own root;

		private:
			utils::MS::graphics::d2d::device d2d_device;

			void spawn_window()
				{
				std::jthread thread{[this]()
					{
					std::function<void(const utils::MS::graphics::d2d::device_context&)> draw;

					utils::MS::window::base window
						{
						utils::MS::window::base::create_info
							{
							.title{L"George G. Goof"},
							//.position{{10, 60}},
							//.size{{800, 600}}
							},
						utils::MS::window::style::create_info
							{
							.transparency{utils::MS::window::style::transparency_t::composition_attribute},
							.borders     {utils::MS::window::style::value_t::disable}
							},
						utils::MS::graphics::d2d::window::composition_swap_chain::create_info
							{
							//.d2d_factory{d2d_factory},
							.d2d_device{d2d_device},
							.draw_callback{[&](const utils::MS::window::base& base, const utils::MS::graphics::d2d::device_context& context)
								{
								context->SetTransform(D2D1::IdentityMatrix());
								context->Clear(D2D1_COLOR_F{0.f, 0.f, 0.f, 0.f});

								if (draw) { draw(context); }
								}}
							},
						utils::MS::window::input::mouse::create_info{}
						};

					auto window_mouse_ptr{window.get_module_ptr<utils::MS::window::input::mouse>()};

					auto input_root{std::make_unique<UI::containers::input<UI::core::wrapper_obs>>(window_mouse_ptr->default_mouse)};
					input_root->element = this->root.get();

					auto& window_ui{window.emplace_module_from_create_info(UI::window<>::create_info{.root{std::move(input_root)}})};
					window_ui.resize(window_ui.get_root()->get_size_prf());

					long ll{static_cast<long>(core::element::get_rect().ll)};
					long up{static_cast<long>(core::element::get_rect().dw)};
					long rr{static_cast<long>(ll + window_ui.get_root()->get_size_prf().x)};
					long dw{static_cast<long>(up + window_ui.get_root()->get_size_prf().y)};

					window.window_rect = utils::MS::window::rect_t{.ll{ll}, .up{up}, .rr{rr}, .dw{dw}};

					draw = [&window_ui](const utils::MS::graphics::d2d::device_context& context) { window_ui.get_root()->draw(context); };

					auto window_graphics{window.get_module_ptr<utils::MS::graphics::d2d::window::composition_swap_chain>()};
					window.show();
					while (window.is_open())
						{
						window.wait_event();

						if (window_ui.get_root()->get_should_redraw()) { window_graphics->draw(); }
						}
					}};
				}
		};
	}