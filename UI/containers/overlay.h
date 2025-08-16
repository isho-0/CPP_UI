#pragma once

#include <ranges>

#include "common.h"

namespace UI::containers
	{
	template <core::concepts::container container_t = core::container_own<0>>
	struct overlay : public container_t
		{
		core::align_hor align_hor{core::align_hor::left};
		core::align_ver align_ver{core::align_ver::top };

		virtual core::widget_obs get_mouseover(core::vec2f position) noexcept override
			{
			for (auto& element : std::ranges::reverse_view{container_t::get_elements_view()})
				{
				if (auto ret{element.get_mouseover(position)}) { return ret; }
				}
			return nullptr;
			}

		virtual core::vec2f _get_size_min() const noexcept final override
			{
			core::vec2f ret{0, 0};
			for (const auto& element : container_t::get_elements_view())
				{
				core::vec2f element_val{element.get_size_min()};
				ret.x = std::max(ret.x, element_val.x);
				ret.y = std::max(ret.y, element_val.y);
				}
			return ret;
			}
		virtual core::vec2f _get_size_prf() const noexcept final override
			{
			core::vec2f ret{0, 0};
			for (const auto& element : container_t::get_elements_view())
				{
				core::vec2f element_val{element.get_size_prf()};
				ret.x = std::max(ret.x, element_val.x);
				ret.y = std::max(ret.y, element_val.y);
				}
			return ret;
			}
		virtual core::vec2f _get_size_max() const noexcept final override
			{
			core::vec2f ret{0, 0};
			for (const auto& element : container_t::get_elements_view())
				{
				core::vec2f element_val{element.get_size_max()};
				ret.x = std::max(ret.x, element_val.x);
				ret.y = std::max(ret.y, element_val.y);
				}
			return ret;
			}

		virtual void on_resize() final override
			{
			for (auto& element : container_t::get_elements_view())
				{
				element.resize(container_t::rect.size());
				}
			}
		virtual void on_reposition() noexcept override
			{
			for (auto& element : container_t::get_elements_view())
				{
				float x{0};
				switch (align_hor)
					{
					case core::align_hor::left:   x = 0; break;
					case core::align_hor::center: x = (container_t::rect.w() / 2.f) - (element.get_rect().w() / 2.f); break;
					case core::align_hor::right:  x =  container_t::rect.w()        -  element.get_rect().w();        break;
					}
				float y{0};
				switch (align_ver)
					{
					case core::align_ver::top:    y = 0; break;
					case core::align_ver::middle: y = (container_t::rect.h() / 2.f) - (element.get_rect().h() / 2.f); break;
					case core::align_ver::bottom: y =  container_t::rect.h()        -  element.get_rect().h();        break;
					}

				element.reposition({container_t::rect.x() + x, container_t::rect.y() + y});
				}
			};
		};
	}