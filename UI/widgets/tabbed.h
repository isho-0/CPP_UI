#pragma once

#include <iostream>

#include <utils/enum.h>
#include <utils/input/mouse.h>
#include <utils/math/geometry/circle.h>

#include "common.h"
#include "../containers/group_ver.h"
#include "../containers/group_hor.h"
#include "../containers/one_of.h"
#include "button.h"

namespace UI::widgets
	{
	class tabbed : public core::element
		{
		public:
			struct tab :  UI::inner::widgets::button
				{
				tab(layers&& layers) : UI::inner::widgets::button{[](){}, layers} {}
				core::element_own content;
				};

		private:
			std::vector<std::unique_ptr<tab>> tabs;

			static core::element_ref extract_handle (std::unique_ptr<tab>& tab) { return *tab; }
			static core::element_ref extract_content(std::unique_ptr<tab>& tab) { return *tab->content; }

			using handles_view_t  = decltype(tabs | std::views::transform(&extract_handle ));
			using contents_view_t = decltype(tabs | std::views::transform(&extract_content));

		public:

			tabbed() :
				handles{root.emplace_at<containers::group_hor<0, handles_view_t >>(0, tabs | std::views::transform(&extract_handle ))},
				content{root.emplace_at<containers::one_of   <0, contents_view_t>>(1, tabs | std::views::transform(&extract_content))}
				{
				}

			
			tab& push(std::unique_ptr<tab>&& element) noexcept
				{
				auto ptr{element.get()};
				tabs.push_back(std::move(element));
				update_views();
				return *ptr;
				}

			template <typename T = core::element>
			T& insert(size_t index, std::unique_ptr<tab>&& element) noexcept
				{
				auto ptr{element.get()};
				tabs.insert(tabs.begin() + index, std::move(element));
				update_views();
				return *ptr;
				}

			virtual ~tabbed() noexcept {}

			virtual core::vec2f _get_size_min() const noexcept final override { return root._get_size_min(); }
			virtual core::vec2f _get_size_prf() const noexcept final override { return root._get_size_prf(); }
			virtual core::vec2f _get_size_max() const noexcept final override { return root._get_size_max(); }
			virtual void on_resize    () noexcept final override { root.on_resize    (); }
			virtual void on_reposition() noexcept final override { root.on_reposition(); }

			virtual void debug_draw(const utils::MS::graphics::d2d::device_context& context, const core::debug_brushes& brushes) const noexcept override
				{
				root.debug_draw(context, brushes);
				}
			virtual void draw(const utils::MS::graphics::d2d::device_context& context) const noexcept override
				{
				root.draw(context);
				}

			virtual core::widget_obs get_mouseover(core::vec2f position) noexcept override
				{
				if (handles.get_rect().contains(position))
					{
					for (auto index : utils::indices(handles.elements))
						{
						if (handles.elements[index]->get_rect().contains(position))
							{
							content.current_index = index;
							}
						}
					}
				else if(content.get_rect().contains(position)) { return content.get_mouseover(position); }
				}

		private:
			containers::group_ver<2> root;
			containers::group_hor<0, handles_view_t >& handles;
			containers::one_of   <0, contents_view_t>& content;

			void update_views() noexcept
				{
				handles.elements = tabs | std::views::transform(&extract_handle );
				content.elements = tabs | std::views::transform(&extract_content);
				}
		};
	}