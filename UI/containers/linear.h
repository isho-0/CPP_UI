#pragma once

#include "common.h"

namespace UI::containers
	{
	template <core::orientation_t orientation, core::concepts::container container_t = core::container_own<0>>
	class linear : public container_t
		{
		public:
			using alignment_t = std::conditional_t<orientation == core::orientation_t::hor, core::align_ver, core::align_hor>;
			alignment_t alignment{};
			static constexpr bool is_hor{orientation == core::orientation_t::hor};
			static constexpr bool is_ver{orientation == core::orientation_t::ver};

		protected:

			virtual core::vec2f _get_size_min() const noexcept override
				{
				core::vec2f ret{0, 0};
				for (const auto& element : container_t::get_elements_view())
					{
					core::vec2f element_val{element.get_size_min()};

					if constexpr (orientation == core::orientation_t::hor)
						{
						ret.x += element_val.x;
						ret.y = std::max(ret.y, element_val.y);
						}
					else if constexpr (orientation == core::orientation_t::ver)
						{
						ret.x = std::max(ret.x, element_val.x);
						ret.y += element_val.y;
						}
					}
				return ret;
				}
			virtual core::vec2f _get_size_prf() const noexcept override
				{
				core::vec2f ret{0, 0};
				for (const auto& element : container_t::get_elements_view())
					{
					core::vec2f element_val{element.get_size_prf()};

					if constexpr (orientation == core::orientation_t::hor)
						{
						ret.x += element_val.x;
						ret.y = std::max(ret.y, element_val.y);
						}
					else if constexpr (orientation == core::orientation_t::ver)
						{
						ret.x = std::max(ret.x, element_val.x);
						ret.y += element_val.y;
						}
					}
				return ret;
				}
			virtual core::vec2f _get_size_max() const noexcept override
				{
				core::vec2f ret{0, 0};
				for (const auto& element : container_t::get_elements_view())
					{
					core::vec2f element_val{element.get_size_max()};

					if constexpr (orientation == core::orientation_t::hor)
						{
						ret.x += element_val.x;
						ret.y = std::max(ret.y, element_val.y);
						}
					else if constexpr (orientation == core::orientation_t::ver)
						{
						ret.x = std::max(ret.x, element_val.x);
						ret.y += element_val.y;
						}
					}
				return ret;
				}

			virtual void on_resize() final override
				{
				auto elements_view{container_t::get_elements_view()};

				auto sizes{calc_sizes_wrapper(orientation == core::orientation_t::hor ? container_t::rect.width() : container_t::rect.height())};

				for (size_t i : utils::indices(elements_view))
					{
					auto& element{elements_view[i]};
					const auto& size{sizes[i]};

					if constexpr (orientation == core::orientation_t::hor)
						{
						element.resize({size, std::min<float>(element.get_size_max().y, container_t::rect.height())});
						}
					else if constexpr (orientation == core::orientation_t::ver)
						{
						element.resize({std::min<float>(element.get_size_max().x, container_t::rect.width()), size});
						}
					}
				}
			virtual void on_reposition() noexcept
				{
				if constexpr (orientation == core::orientation_t::hor)
					{
					float x{0};
					for (auto& element : container_t::get_elements_view())
						{
						float y{0};
						switch (alignment)
							{
							case core::align_ver::top   : y = 0; break;
							case core::align_ver::middle: y = (container_t::rect.h() / 2.f) - (element.get_rect().h() / 2.f); break;
							case core::align_ver::bottom: y =  container_t::rect.h()        -  element.get_rect().h();        break;
							}

						element.reposition({container_t::rect.x() + x, container_t::rect.y() + y});
						x += element.get_rect().width();
						}
					}
				else if constexpr (orientation == core::orientation_t::ver)
					{
					float y{0};
					for (auto& element : container_t::get_elements_view())
						{
						float x{0};
						switch (alignment)
							{
							case core::align_hor::left  : x = 0; break;
							case core::align_hor::center: x = (container_t::rect.w() / 2.f) - (element.get_rect().w() / 2.f); break;
							case core::align_hor::right : x =  container_t::rect.w()        -  element.get_rect().w();        break;
							}

						element.reposition({container_t::rect.x() + x, container_t::rect.y() + y});
						y += element.get_rect().height();
						}
					}
				};

			static const details::constraints_t extract_constraints(const core::element& element) noexcept
				{
				if constexpr (orientation == core::orientation_t::hor)
					{
					return details::constraints_t::hor(element);
					}
				else if constexpr (orientation == core::orientation_t::ver)
					{
					return details::constraints_t::ver(element);
					}
				}

			virtual std::vector<float> calc_sizes_wrapper(float available)
				{
				return details::calc_sizes(available, container_t::get_elements_view() | std::views::transform(&extract_constraints));
				}
		};

	
	template <core::orientation_t orientation, core::concepts::container container_t = core::container_own<0>>
	class linear_manual_sizes : public linear<orientation, container_t>, public core::widget
		{
		public:
			using linear<orientation, container_t>::is_hor;
			using linear<orientation, container_t>::is_ver;

			linear_manual_sizes(utils::MS::graphics::d2d::brush brush_size_line_auto, utils::MS::graphics::d2d::brush brush_size_line_manual)
				: brush_size_line_auto{brush_size_line_auto}, brush_size_line_manual{brush_size_line_manual} {}

			float resizing_distance{2.f};

			virtual core::widget_obs get_mouseover(core::vec2f position) noexcept override
				{
				if (dragging) { return this; }

				auto elements_view{container_t::get_elements_view()};
				for (size_t i : utils::indices(elements_view))
					{
					auto& element{elements_view[i]};
					const auto& rect{element.get_rect()};
					if (!rect.contains(position)) { continue; }

					if (position.x <= rect.ll + resizing_distance && i > 0)
						{
						resizing_index = i - 1; 
						return this;
						}
					else if (position.x >= rect.rr - resizing_distance)
						{
						resizing_index = i;
						return this;
						}

					return element.get_mouseover(position);
					}
				return nullptr;
				}

			void refresh_manual_sizes() { manual_sizes.resize(container_t::get_elements_view().size(), core::fnan); }
			
			virtual bool on_focus_lose () override { dragging = false; return false; }
			virtual bool on_mouse_leave() override { set_cursor_default(); return false; }
			virtual bool on_mouse_enter() override { set_cursor_drag   (); return false; }
			virtual bool on_mouse_move(core::vec2f position) override
				{
				//set_cursor_drag(); //needed because WM_SETCURSOR for defwndproc resets the cursor to arrow on every event

				auto elements_view{container_t::get_elements_view()};
				if (resizing_index >= elements_view.size()) { return false; }
				const auto& element{elements_view[resizing_index]};
				const auto& element_rect{element.get_rect()};

				if (!dragging) 
					{
					dragging_from_position = is_hor ? position.x : position.y;
					
					if (std::isnan(manual_sizes[resizing_index]))
						{
						dragging_original_size = is_hor ? element_rect.width() : element_rect.height();
						}
					else
						{
						dragging_original_size = manual_sizes[resizing_index];
						}

					return false;
					}
				else
					{
					float delta{((is_hor ? position.x : position.y) - dragging_from_position)};

					if (delta > 0) //growing
						{
						//TODO check for available space before growing
						float available
							{
							is_hor ?
								core::element::get_rect().rr - elements_view[elements_view.size() - 1].get_rect().rr :
								core::element::get_rect().dw - elements_view[elements_view.size() - 1].get_rect().dw
							};
						if (delta > available) { delta = available; }
						}

					float drag_result{dragging_original_size + delta};
					float min{std::max(resizing_distance + 1, (is_hor ? element.get_size_min().x : element.get_size_min().y))};
					float max{is_hor ? element.get_size_max().x : element.get_size_max().y};

					manual_sizes[resizing_index] = std::clamp(drag_result, min, max);
					linear<orientation, container_t>::on_resize();
					linear<orientation, container_t>::on_reposition();
					core::element::should_redraw = true;
					return true;
					}
				}
			virtual bool on_mouse_button(const utils::input::mouse::button_id& id, const bool& state) override
				{
				if (id == utils::input::mouse::button_id::left)
					{
					dragging = state;
					return false;
					}
				
				dragging = false;

				if (id == utils::input::mouse::button_id::right)
					{//reset to no custom sizes, which lets the element's sizes through
					manual_sizes[resizing_index] = core::fnan;
					linear<orientation, container_t>::on_resize();
					linear<orientation, container_t>::on_reposition();
					core::element::should_redraw = true;
					return true;
					}

				return false;
				}

			utils::MS::graphics::d2d::brush brush_size_line_auto;
			utils::MS::graphics::d2d::brush brush_size_line_manual;

		protected:
			std::vector<float> manual_sizes;
			bool dragging{false};
			float dragging_from_position{0.f};
			float dragging_original_size{0.f};
			size_t resizing_index{0};
			void set_cursor_drag   () const noexcept { SetCursor(LoadCursor(NULL, IDC_SIZEWE)); }
			void set_cursor_default() const noexcept { SetCursor(LoadCursor(NULL, IDC_ARROW )); }
			
			template <auto get>
			core::vec2f _gets_f() const noexcept
				{
				core::vec2f ret{0, 0};
				auto elements_view{container_t::get_elements_view()};
				for (size_t i : utils::indices(elements_view))
					{
					const auto& element{elements_view[i]};
					core::vec2f element_val{get(element)};
					const auto& manual_size{manual_sizes[i]};

					if (std::isnan(manual_size))
						{
						if constexpr (orientation == core::orientation_t::hor)
							{
							ret.x += element_val.x;
							ret.y = std::max(ret.y, element_val.y);
							}
						else if constexpr (orientation == core::orientation_t::ver)
							{
							ret.x = std::max(ret.x, element_val.x);
							ret.y += element_val.y;
							}
						}
					else
						{
						if constexpr (orientation == core::orientation_t::hor)
							{
							ret.x += manual_size;
							ret.y = std::max(ret.y, element_val.y);
							}
						else if constexpr (orientation == core::orientation_t::ver)
							{
							ret.x = std::max(ret.x, element_val.x);
							ret.y += manual_size;
							}
						}
					}
				return ret;
				}

			virtual core::vec2f _get_size_min() const noexcept override
				{
				return _gets_f<[](const auto& element) -> core::vec2f { return element.get_size_min(); }>();
				}
			virtual core::vec2f _get_size_prf() const noexcept override
				{
				return _gets_f<[](const auto& element) -> core::vec2f { return element.get_size_prf(); }>();
				}
			virtual core::vec2f _get_size_max() const noexcept override
				{
				return _gets_f<[](const auto& element) -> core::vec2f { return element.get_size_max(); }>();
				}

			virtual std::vector<float> calc_sizes_wrapper(float available) override
				{
				//TODO make a view instead of creating a temporary vector
				auto elements_view{container_t::get_elements_view()};
				std::vector<details::constraints_t> constraints_container; constraints_container.reserve(elements_view.size());
				for (size_t i : utils::indices(elements_view))
					{
					const auto& element{elements_view[i]};
					const auto& manual_size{manual_sizes[i]};
					if (std::isnan(manual_size)) { constraints_container.push_back(linear<orientation, container_t>::extract_constraints(element)); }
					else { constraints_container.push_back(details::constraints_t(manual_size, manual_size, manual_size)); }
					}
				return details::calc_sizes(available, constraints_container);
				}

			virtual void _draw(const utils::MS::graphics::d2d::device_context& context) const noexcept override
				{
				linear<orientation, container_t>::_draw(context);
				
				auto elements_view{container_t::get_elements_view()};
				for (size_t i : utils::indices(elements_view))
					{
					const auto& element{elements_view[i]};
					const auto& manual_size{manual_sizes[i]};

					utils::MS::graphics::d2d::brush brush{std::isnan(manual_size) ? brush_size_line_auto : brush_size_line_manual};

					utils::math::vec2f a;
					utils::math::vec2f b;

					if constexpr (orientation == core::orientation_t::hor)
						{
						a.x = b.x = element.get_rect().rr;
						a.y = core::element::get_rect().up;
						b.y = core::element::get_rect().dw;
						}
					else if constexpr (orientation == core::orientation_t::ver)
						{
						a.x = core::element::get_rect().ll;
						b.x = core::element::get_rect().rr;
						a.y = b.y = element.get_rect().dw;
						}

					context->DrawLine({a.x, a.y}, {b.x, b.y}, brush.get(), 1.f);
					}
				}
		};

	template <bool manual_sizes = false, core::concepts::container container_t = core::container_own<0>> struct hor {};
	template <bool manual_sizes = false, core::concepts::container container_t = core::container_own<0>> struct ver {};

	template <core::concepts::container container_t> struct hor<false, container_t> : linear             <core::orientation_t::hor, container_t> { using linear             <core::orientation_t::hor, container_t>::linear             ; };
	template <core::concepts::container container_t> struct ver<false, container_t> : linear             <core::orientation_t::ver, container_t> { using linear             <core::orientation_t::ver, container_t>::linear             ; };
	template <core::concepts::container container_t> struct hor<true , container_t> : linear_manual_sizes<core::orientation_t::hor, container_t> { using linear_manual_sizes<core::orientation_t::hor, container_t>::linear_manual_sizes; };
	template <core::concepts::container container_t> struct ver<true , container_t> : linear_manual_sizes<core::orientation_t::ver, container_t> { using linear_manual_sizes<core::orientation_t::ver, container_t>::linear_manual_sizes; };
	}