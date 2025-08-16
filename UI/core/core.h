#pragma once
#include <ranges>
#include <algorithm>

#include <utils/math/geometry/aabb.h>
#include <utils/input/mouse.h>
#include <utils/MS/window/regions.h>

#include "base_types.h"

namespace UI::core
	{
	class element
		{
		public:
			virtual ~element() {}

			const utils::math::geometry::aabb& get_rect() const noexcept { return rect; }

			void debug_draw_rect(const utils::MS::graphics::d2d::device_context& context, const utils::MS::graphics::d2d::solid_brush& bg, const utils::MS::graphics::d2d::solid_brush& br) const noexcept
				{
				D2D1_RECT_F d2d_rect{.left{rect.left}, .top{rect.up}, .right{rect.right}, .bottom{rect.bottom}};

				context->FillRectangle(d2d_rect, bg.get());

				context->DrawRectangle(d2d_rect, br.get(), 1);
				}

			virtual void debug_draw(const utils::MS::graphics::d2d::device_context& context, const debug_brushes& brushes) const noexcept
				{
				debug_draw_rect(context, brushes.elem_bg, brushes.elem_br);
				}
			void draw(const utils::MS::graphics::d2d::device_context& context) const noexcept { should_redraw = false; _draw(context); }
			virtual utils::observer_ptr<const element> get_should_redraw() const noexcept { return should_redraw ? this : nullptr; }

			virtual widget_obs get_mouseover(vec2f position) noexcept { return nullptr; }

			// Dynamic UI
			void resize_checked(vec2f size)
				{
				vec2f min_a = get_size_min();
				vec2f max_a = get_size_max();
				if (size.x < min_a.x /*|| size.x > max.x*/ ||
					size.y < min_a.y /*|| size.y > max.y*/)
					{
					//throw std::runtime_error{"Invalid resizing values."};
					using namespace utils::output;
					std::cout << "Invalid resizing values\n"
						<< "min: " << min_a	<< "\n"
						<< "val: " << size  << "\n"
						<< "max: " << max_a << "\n";
					}
				size.x = std::min(size.x, max_a.x);
				size.y = std::min(size.y, max_a.y);
				resize(size);
				}

			void resize    (vec2f max_size)          { rect.size    () = max_size; on_resize    (); should_redraw = true; };
			void reposition(vec2f position) noexcept { rect.position() = position; on_reposition(); should_redraw = true; };

			custom_sizes_t sizes;

			vec2f get_size_min() const noexcept { vec2f tmp{_get_size_min()}; return {std::max(sizes.min.x, tmp.x), std::max(sizes.min.y, tmp.y)}; }
			vec2f get_size_max() const noexcept { vec2f tmp{_get_size_max()}; return {std::min(sizes.max.x, tmp.x), std::min(sizes.max.y, tmp.y)}; }
			vec2f get_size_prf() const noexcept 
				{ 
				vec2f tmp{_get_size_prf()}; 
				vec2f min{get_size_min()};
				vec2f max{get_size_max()};

				return
					{
					std::isnan(sizes.prf.x) ? std::clamp(tmp.x, min.x, max.x) : sizes.prf.x, 
					std::isnan(sizes.prf.y) ? std::clamp(tmp.y, min.y, max.y) : sizes.prf.y
					};
				}

			inline constexpr operator bool() const noexcept { return true; }

		protected:
			utils::math::geometry::aabb rect;
			mutable bool should_redraw{false};

			virtual vec2f _get_size_min() const noexcept { return {0.f , 0.f }; }
			virtual vec2f _get_size_max() const noexcept { return {finf, finf}; }
			virtual vec2f _get_size_prf() const noexcept
				{
				vec2f min{get_size_min()};
				vec2f max{get_size_max()};
				return
					{
					max.x == finf ? std::max(128.f, min.x) : min.x + ((max.x - min.x) / 2.f),
					max.y == finf ? std::max( 64.f, min.y) : min.y + ((max.y - min.y) / 2.f)
					};
				}

			virtual void _draw(const utils::MS::graphics::d2d::device_context& context) const noexcept {}

			virtual void on_resize    () {}
			virtual void on_reposition() {}
		};

	namespace details
		{
		template <typename VIEW_T, typename CVIEW_T>
		//requires(std::same_as<core::element_ref, decltype(*(VIEW_T{}.begin()))>)
		class container_elements_view_interface : public element
			{
			public:
				using  view_t =  VIEW_T;
				using cview_t = CVIEW_T;

				virtual        view_t get_elements_view()       noexcept = 0;
				virtual const cview_t get_elements_view() const noexcept = 0;

				virtual void debug_draw(const utils::MS::graphics::d2d::device_context& context, const debug_brushes& brushes) const noexcept override
					{
					debug_draw_rect(context, brushes.cont_bg, brushes.cont_br);
					for (const auto& element : get_elements_view())
						{
						element.debug_draw(context, brushes);
						}
					}

				virtual widget_obs get_mouseover(vec2f position) noexcept override
					{
					for (auto& element : get_elements_view())
						{
						if (widget_obs ret{element.get_mouseover(position)}) { return ret; }
						}
					return nullptr;
					}

				virtual utils::observer_ptr<const element> get_should_redraw() const noexcept
					{
					if (auto ret{element::get_should_redraw()}) { return ret; }

					utils::observer_ptr<const element> ret{nullptr};

					//if more than one sub-element must be redrawn, the whole container should be
					for (const auto& element : get_elements_view())
						{
						auto tmp{element.get_should_redraw()};
						if (ret && tmp) { return this; }
						ret = tmp;
						}

					return ret;
					}

			protected:
				virtual void _draw(const utils::MS::graphics::d2d::device_context& context) const noexcept override
					{
					for (const auto& element : get_elements_view())
						{
						element.draw(context);
						}
					}
			};
		
		class wrapper_element_view_interface : public element
			{
			public:
				virtual       element_ref get_element()       noexcept = 0;
				virtual const element_ref get_element() const noexcept = 0;

				virtual void debug_draw(const utils::MS::graphics::d2d::device_context& context, const debug_brushes& brushes) const noexcept override
					{
					element::debug_draw_rect(context, brushes.wrap_bg, brushes.wrap_br);
					get_element().debug_draw(context, brushes);
					}

				virtual widget_obs get_mouseover(vec2f position) noexcept override
					{
					return get_element().get_mouseover(position);
					}

				virtual utils::observer_ptr<const element> get_should_redraw() const noexcept
					{
					if (auto ret{element::get_should_redraw()}) { return ret; }
					return get_element().get_should_redraw();
					}

			protected:
				virtual void _draw(const utils::MS::graphics::d2d::device_context& context) const noexcept override
					{
					get_element().draw(context);
					}

				virtual core::vec2f _get_size_min() const noexcept override { return get_element().get_size_min(); }
				virtual core::vec2f _get_size_prf() const noexcept override { return get_element().get_size_prf(); }
				virtual core::vec2f _get_size_max() const noexcept override { return get_element().get_size_max(); }

				virtual void on_resize    () override { get_element().resize    (rect.size    ()); }
				virtual void on_reposition() noexcept { get_element().reposition(rect.position()); }
			};
		}

	template <typename VIEW_T, typename CVIEW_T>
	struct container_obs : details::container_elements_view_interface<VIEW_T, CVIEW_T>
		{
		using view_t = VIEW_T;
		view_t elements_view;

		container_obs(view_t view) : elements_view{view} {}

		virtual       view_t get_elements_view()       override { return elements_view; }
		virtual const view_t get_elements_view() const override { return elements_view; }
		};

	struct wrapper_obs : details::wrapper_element_view_interface
		{
		element_obs element{nullptr};
		virtual       element_ref get_element()       noexcept { return *element; };
		virtual const element_ref get_element() const noexcept { return *element; };
		};

	namespace details
		{
		template <size_t SLOTS_COUNT = 0>
		class container_own_inner
			{
			public:
				static constexpr size_t slots_count{SLOTS_COUNT};
		
			protected:
				static constexpr bool is_vector{slots_count == 0};
				static constexpr bool is_single{slots_count == 1};
				static constexpr bool is_array {slots_count >= 1};

			public:
				using elements_t = std::conditional_t
						<
						is_vector,
						std::vector<element_own>, 
						std::array<element_own, slots_count>
						>;
				elements_t elements;

			protected:
				static       core::element_ref extract_element      (      core::element_own& element_ptr) noexcept { return *element_ptr; }
				static const core::element_ref extract_element_const(const core::element_own& element_ptr) noexcept { return *element_ptr; }

			public:
				auto make_extract_view ()       { return elements | std::views::transform(&extract_element      ); }
				auto make_extract_cview() const { return elements | std::views::transform(&extract_element_const); }
			};

		template <size_t SC> using extract_view_t  = decltype(container_own_inner<SC>{}.make_extract_view ());
		template <size_t SC> using extract_cview_t = decltype(container_own_inner<SC>{}.make_extract_cview());
		}

	template <size_t SLOTS_COUNT = 0>
	class container_own : public details::container_own_inner<SLOTS_COUNT>, public details::container_elements_view_interface<details::extract_view_t<SLOTS_COUNT>, details::extract_cview_t<SLOTS_COUNT>>
		{
		public:
			static constexpr size_t slots_count{SLOTS_COUNT};

		protected:
			static constexpr bool is_vector{slots_count == 0};
			static constexpr bool is_single{slots_count == 1};
			static constexpr bool is_array{slots_count >= 1};

		public:
			using  view_t = typename details::extract_view_t <SLOTS_COUNT>;
			using cview_t = typename details::extract_cview_t<SLOTS_COUNT>;
			using container_view_interface = details::container_elements_view_interface<view_t, cview_t>;
			using details::container_own_inner<SLOTS_COUNT>::elements;

			virtual        view_t get_elements_view()       noexcept { return details::container_own_inner<SLOTS_COUNT>::make_extract_view (); };
			virtual const cview_t get_elements_view() const noexcept { return details::container_own_inner<SLOTS_COUNT>::make_extract_cview(); };
		
			template <typename T, typename ...Args>
			T& emplace(Args&&... args) noexcept
				requires(is_vector)
				{
				return push<T>(std::make_unique<T>(std::forward<Args>(args)...));
				}

			template <typename T, typename ...Args>
			T& emplace_at(size_t index, Args&&... args) noexcept
				{
				if constexpr (is_vector) { return insert<T>(index, std::make_unique<T>(std::forward<Args>(args)...)); }
				if constexpr (is_array ) { return set   <T>(index, std::make_unique<T>(std::forward<Args>(args)...)); }
				}
				
			template <typename T = core::element>
			T& push(std::unique_ptr<T>&& element) noexcept
				requires(is_vector)
				{
				utils::observer_ptr<T> ptr{element.get()};

				     if constexpr (is_vector) { elements.push_back(std::move(element)); }
				else if constexpr (is_single) { elements[0] = std::move(element); }

				return *ptr;
				}
				
			template <typename T = core::element>
			T& set(size_t index, std::unique_ptr<T>&& element) noexcept
				requires(is_array)
				{
				utils::observer_ptr<T> ptr{element.get()};

				elements[index] = std::move(element);

				return *ptr;
				}

			template <typename T = core::element>
			T& insert(size_t index, std::unique_ptr<T>&& element) noexcept
				requires(is_vector)
				{
				utils::observer_ptr<T> ptr{element.get()};

				elements.insert(elements.begin() + index, std::move(element));

				return *ptr;
				}

			core::element_own remove(core::element_obs element) noexcept
				requires(is_vector)
				{
				auto it{std::find_if(elements.begin(), elements.end(), [this, element](const core::element_own& ptr) -> bool { return ptr.get() == element; })};
				if (it != elements.end())
					{
					auto ret{std::move(*it)};
					     if constexpr (is_vector) { elements.erase(it); }
					else if constexpr (is_array ) { *it = nullptr; }

					return ret;
					}

				return nullptr;
				}
		};

	class wrapper_own : public details::wrapper_element_view_interface
		{
		public:
			element_own element;

			virtual       element_ref get_element()       noexcept { return *element; };
			virtual const element_ref get_element() const noexcept { return *element; };

			template <typename T, typename ...Args>
			T& emplace(Args&&... args) noexcept
				{
				return push<T>(std::make_unique<T>(std::forward<Args>(args)...));
				}
				
			template <typename T = core::element>
			T& push(std::unique_ptr<T>&& element) noexcept
				{
				utils::observer_ptr<T> ptr{element.get()};

				this->element = std::move(element);

				return *ptr;
				}
		};

	namespace concepts
		{
		template <typename T>
		concept container_own = std::derived_from<T, UI::core::container_own<T::slots_count>>;
		template <typename T>
		concept container_obs = std::derived_from<T, UI::core::container_obs<typename T::view_t, typename T::cview_t>>;
		template <typename T>
		concept container = container_own<T> || container_obs<T>;

		template <typename T>
		concept wrapper_own = std::derived_from<T, UI::core::wrapper_own>;
		template <typename T>
		concept wrapper_obs = std::derived_from<T, UI::core::wrapper_obs>;
		template <typename T>
		concept wrapper = wrapper_own<T> || wrapper_obs<T>;

		template <typename T>
		concept container_or_wrapper = container<T> || wrapper<T>;
		}

	struct widget
		{
		virtual bool on_focus_gain () { return false; }
		virtual bool on_focus_lose () { return false; }
		virtual bool on_mouse_enter() { return false; }
		virtual bool on_mouse_leave() { return false; }
		virtual bool on_mouse_move (vec2f position) { return false; }
		virtual bool on_mouse_button(const utils::input::mouse::button_id& id, const bool& state) { return false; }
		};

	class element_widget : public element, public widget
		{
		public:
			virtual void debug_draw(const utils::MS::graphics::d2d::device_context& context, const debug_brushes& brushes) const noexcept override
				{
				debug_draw_rect(context, brushes.widg_bg, brushes.widg_br);
				}

			virtual widget_obs get_mouseover(vec2f position) noexcept override
				{
				return rect.contains(position) ? this : nullptr;
				}

		};

	template <concepts::container_or_wrapper container_t>
	class container_widget : public container_t, public widget
		{
		public:
			virtual widget_obs get_mouseover(vec2f position) noexcept override
				{
				return container_t::rect.contains(position) ? this : nullptr;
				}
		};

	}