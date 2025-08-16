#pragma once
#include "../core/core.h"

namespace UI::containers
	{
	core::vec2f get_padding_aabb_tot(const utils::math::geometry::aabb& aabb) noexcept { return {aabb.rr + aabb.ll, aabb.up + aabb.dw}; }

	template <core::concepts::wrapper wrapper_t = core::wrapper_own>
	class padding : public wrapper_t, public utils::math::geometry::aabb
		{
		public:
			padding(const utils::math::geometry::aabb& init) : utils::math::geometry::aabb{init} {}

		protected:
			virtual core::vec2f _get_size_min() const noexcept final override { return wrapper_t::_get_size_min() + get_padding_aabb_tot(*this); }
			virtual core::vec2f _get_size_prf() const noexcept final override { return wrapper_t::_get_size_prf() + get_padding_aabb_tot(*this); }
			virtual core::vec2f _get_size_max() const noexcept final override { return wrapper_t::_get_size_max() + get_padding_aabb_tot(*this); }

			virtual void on_resize    () override { wrapper_t::get_element().resize    (core::element::rect.size    () - get_padding_aabb_tot(*this)); }
			virtual void on_reposition() noexcept { wrapper_t::get_element().reposition(core::element::rect.position() + utils::math::vec2f{ll, up}); }
		};

	template <core::concepts::wrapper wrapper_t = core::wrapper_own>
	class variable_padding : public wrapper_t
		{
		public:
			utils::math::geometry::aabb min;
			utils::math::geometry::aabb prf;
			utils::math::geometry::aabb max;

		protected:
			virtual core::vec2f _get_size_min() const noexcept final override { return wrapper_t::_get_size_min() + get_padding_aabb_tot(min); }
			virtual core::vec2f _get_size_prf() const noexcept final override { return wrapper_t::_get_size_prf() + get_padding_aabb_tot(prf); }
			virtual core::vec2f _get_size_max() const noexcept final override { return wrapper_t::_get_size_max() + get_padding_aabb_tot(max); }

			//virtual void on_resize    () override 
			//	{
			//	//TODO proper math
			//	wrapper_t::get_element().resize    (core::element::rect.size    () - get_padding_aabb_tot(min));
			//	}
			//
			//virtual void on_reposition() noexcept { wrapper_t::get_element().reposition(core::element::rect.position() + utils::math::vec2f{ll. up}); }
		};
	}