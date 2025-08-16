#pragma once

#include <utils/math/math.h>
#include "core/core.h"

namespace UI
	{
	template <bool owning = true>
	class window : public utils::MS::window::module
		{
		public:
			using root_t = std::conditional_t<owning, core::element_own, core::element_obs>;
			struct create_info
				{
				using module_type = window;
				root_t&& root;
				};

			window(utils::MS::window::base& base, const create_info& create_info) :
				module{base},
				root{std::move(create_info.root)}
				{
				}

			virtual utils::MS::window::procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override
				{
				if (root)
					{
					switch (msg)
						{
						case WM_SIZE:
							resize(utils::math::vec2u{LOWORD(lparam), HIWORD(lparam)});
							return utils::MS::window::procedure_result::next(0);

						case WM_GETMINMAXINFO:
							getminmaxinfo(lparam);
							return utils::MS::window::procedure_result::stop(0);
						}
					}
				return utils::MS::window::procedure_result::next();
				}

			const root_t& get_root() const noexcept { return root; }
			      root_t& get_root()       noexcept { return root; }

			void resize(utils::math::vec2f size)
				{
				root->resize(size);
				root->reposition(root->get_rect().ul());
				}

		private:
			root_t root{nullptr};

			void getminmaxinfo(LPARAM lparam)
				{
				auto decorations_size{get_base().window_rect.size() - get_base().client_rect.size()};

				auto size_min{root->get_size_min() + decorations_size};
				auto size_max{root->get_size_max() + decorations_size};

				LPMINMAXINFO lpMMI = (LPMINMAXINFO)lparam;
				lpMMI->ptMinTrackSize.x = utils::math::cast_clamp<LONG>(size_min.x);
				lpMMI->ptMinTrackSize.y = utils::math::cast_clamp<LONG>(size_min.y);
				lpMMI->ptMaxTrackSize.x = utils::math::cast_clamp<LONG>(size_max.x);
				lpMMI->ptMaxTrackSize.y = utils::math::cast_clamp<LONG>(size_max.y);
				}
		};
	}