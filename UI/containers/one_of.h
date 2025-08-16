#pragma once

#include "common.h"
#include "overlay.h"

namespace UI::containers
	{
	template <core::concepts::container container_t = core::container_own<0>>
	class one_of : public overlay<container_t>
		{
		public:
			virtual void debug_draw(const utils::MS::graphics::d2d::device_context& context, const core::debug_brushes& brushes) const noexcept override
				{
				overlay<container_t>::debug_draw_rect(context, brushes.cont_bg, brushes.cont_br);
				auto elements_view{container_t::get_elements_view()};
				if (current_index < elements_view.size()) { elements_view[current_index].debug_draw(context, brushes); }
				}
			virtual void _draw(const utils::MS::graphics::d2d::device_context& context) const noexcept override
				{
				auto elements_view{container_t::get_elements_view()};
				if (current_index < elements_view.size()) { elements_view[current_index].draw(context); }
				}

			virtual utils::observer_ptr<const core::element> get_should_redraw() const noexcept
				{
				if (auto ret{core::element::get_should_redraw()}) { return ret; }
				auto elements_view{container_t::get_elements_view()};
				if (current_index < elements_view.size()) { return elements_view[current_index].get_should_redraw(); }
				return nullptr;
				}

			virtual core::widget_obs get_mouseover(core::vec2f position) noexcept override
				{
				auto elements_view{container_t::get_elements_view()};
				if (current_index < elements_view.size()) { return elements_view[current_index].get_mouseover(position); }
				return nullptr; 
				}

			auto remove(core::element_obs element_ptr) noexcept
				{
				auto elements_view{container_t::get_elements_view()};
				for (size_t i : utils::indices(elements_view))
					{
					const auto& element{elements_view[i]};
					if (std::addressof(element) == element_ptr)
						{
						if (i < current_index) { current_index--; }
						}
					}
				return overlay<container_t>::remove(element_ptr);
				}

			size_t get_current_index() const noexcept { return current_index; }
			void   set_current_index(size_t value) noexcept { current_index = value; core::element::should_redraw = true; }

		private:
			size_t current_index{0};
		};
	}