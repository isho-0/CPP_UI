#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <memory>
#include <stack>
#include <limits>
#include <type_traits>
#include <numeric>

#include <utils/math/vec2.h>
#include <utils/graphics/colour.h>

#include <utils/MS/graphics/d2d.h>

namespace UI::core
	{
#pragma region Support
	using vec2f = utils::math::vec2f;
#pragma endregion Support

#pragma region Types
	class element;
	using element_own  = std  ::unique_ptr  <element>;
	using element_obs  = utils::observer_ptr<element>;
	using elements_own = std::vector<element_own>;
	using elements_obs = std::vector<element_obs>;
	using element_ref  = element&;

	struct widget;
	using widget_own  = std  ::unique_ptr  <widget>;
	using widget_obs  = utils::observer_ptr<widget>;
	using widgets_own = std::vector<widget_own>;
	using widgets_obs = std::vector<widget_obs>;
	using widget_ref  = widget&;

	enum class orientation_t { hor, ver };
	enum class align_ver { top, middle, bottom };
	enum class align_hor { left, center, right };
#pragma endregion Types

#pragma region Constants
	inline const utils::graphics::colour::rgba elem_bg{0, 1, 1, .05};
	inline const utils::graphics::colour::rgba elem_br{0, 1, 1, .5 };
	inline const utils::graphics::colour::rgba wrap_bg{1, 1, 0, .05};
	inline const utils::graphics::colour::rgba wrap_br{1, 1, 0, .5 };
	inline const utils::graphics::colour::rgba cont_bg{1, 0, 0, .05};
	inline const utils::graphics::colour::rgba cont_br{1, 0, 0, .5 };
	inline const utils::graphics::colour::rgba widg_bg{0, 1, 0, .05};
	inline const utils::graphics::colour::rgba widg_br{0, 1, 0, .5 };
	inline const utils::graphics::colour::rgba focus  {1, 1, 1, .5 };
	inline const float finf{std::numeric_limits<float>::infinity()};
	inline const float fnan{std::numeric_limits<float>::quiet_NaN()};

	struct debug_brushes
		{
		debug_brushes(const utils::MS::graphics::d2d::device_context& context) :
			elem_bg{context, UI::core::elem_bg},
			elem_br{context, UI::core::elem_br},
			wrap_bg{context, UI::core::wrap_bg},
			wrap_br{context, UI::core::wrap_br},
			cont_bg{context, UI::core::cont_bg},
			cont_br{context, UI::core::cont_br},
			widg_bg{context, UI::core::widg_bg},
			widg_br{context, UI::core::widg_br},
			focus  {context, UI::core::focus  }
			{}

		utils::MS::graphics::d2d::solid_brush elem_bg;
		utils::MS::graphics::d2d::solid_brush elem_br;
		utils::MS::graphics::d2d::solid_brush wrap_bg;
		utils::MS::graphics::d2d::solid_brush wrap_br;
		utils::MS::graphics::d2d::solid_brush cont_bg;
		utils::MS::graphics::d2d::solid_brush cont_br;
		utils::MS::graphics::d2d::solid_brush widg_bg;
		utils::MS::graphics::d2d::solid_brush widg_br;
		utils::MS::graphics::d2d::solid_brush focus  ;
		};
#pragma endregion Constants

#pragma region inner_types

	class custom_sizes_t
		{
		public:
			custom_sizes_t() = default;
			custom_sizes_t(vec2f _min, vec2f _max)
				{
				if (_min.x > _max.x || _min.y > _max.y) { throw sizes_error; }
				this->_min = _min;
				this->_max = _max;
				}
			custom_sizes_t(vec2f _min, vec2f _prf, vec2f _max)
				{
				if (_min.x > _max.x || _min.y > _max.y || _prf.x < _min.x || _prf.x > _max.x || _prf.y < _min.y || _prf.y > _max.y) { throw sizes_error; }
				this->_min = _min;
				this->_max = _max;
				this->_prf = _prf;
				}

#pragma region Setters
			void set_min(vec2f val)
				{
				if ((!std::isnan(_prf.x) && val.x > _prf.x) ||
					(!std::isnan(_prf.y) && val.y > _prf.y) ||
					val.x > _max.x || val.y > _max.y)
					{
					throw sizes_error;
					}
				_min = val;
				}
			void set__prf(vec2f val)
				{
				if (val.x < _min.x || val.x > _max.x || val.y < _min.y || val.y > _max.y) { throw sizes_error; }
				_prf = val;
				}
			void set_max(vec2f val)
				{
				if ((!std::isnan(_prf.x) && val.x < _prf.x) ||
					(!std::isnan(_prf.y) && val.y < _prf.y) ||
					val.x < _min.x || val.y < _min.y)
					{
					throw sizes_error;
					}
				_max = val;
				}
			void set_min_x(float x)
				{
				if (!std::isnan(_prf.x)) { if (x > _prf.x) { throw sizes_error; } }
				else { if (x > _max.x) { throw sizes_error; } }
				_min.x = x;
				}
			void set_min_y(float y)
				{
				if (!std::isnan(_prf.y)) { if (y > _prf.y) { throw sizes_error; } }
				else { if (y > _max.y) { throw sizes_error; } }
				_min.y = y;
				}
			void set_prf_x(float x)
				{
				if (x < _min.x || x > _max.x) { throw sizes_error; }
				_prf.x = x;
				}
			void set_prf_y(float y)
				{
				if (y < _min.y || y > _max.y) { throw sizes_error; }
				_prf.y = y;
				}
			void set_max_x(float x)
				{
				if (!std::isnan(_prf.x)) { if (x < _prf.x) { throw sizes_error; } }
				else { if (x < _min.x) { throw sizes_error; } }
				_max.x = x;
				}
			void set_max_y(float y)
				{
				if (!std::isnan(_prf.y)) { if (y < _prf.y) { throw sizes_error; } }
				else { if (y < _min.y) { throw sizes_error; } }
				_max.y = y;
				}
#pragma endregion Setters
#pragma region Getters
			const vec2f get_prf() const noexcept { return _prf; }
			const vec2f get_min() const noexcept { return _min; }
			const vec2f get_max() const noexcept { return _max; }
			/**/  float get_min_x() const noexcept { return _min.x; }
			/**/  float get_min_y() const noexcept { return _min.y; }
			/**/  float get_prf_x() const noexcept { return _prf.x; }
			/**/  float get_prf_y() const noexcept { return _prf.y; }
			/**/  float get_max_x() const noexcept { return _max.x; }
			/**/  float get_max_y() const noexcept { return _max.y; }
#pragma endregion Getters
#pragma region Properties
			__declspec(property(get = get_min, put = set_min)) vec2f min;
			__declspec(property(get = get_prf, put = set_prf)) vec2f prf;
			__declspec(property(get = get_max, put = set_max)) vec2f max;

			__declspec(property(get = get_min_x, put = set_min_x)) float min_x;
			__declspec(property(get = get_min_y, put = set_min_y)) float min_y;
			__declspec(property(get = get_prf_x, put = set_prf_x)) float prf_x;
			__declspec(property(get = get_prf_y, put = set_prf_y)) float prf_y;
			__declspec(property(get = get_max_x, put = set_max_x)) float max_x;
			__declspec(property(get = get_max_y, put = set_max_y)) float max_y;
#pragma endregion Properties

			void set_x(float min, float prf, float max)
				{
				if (std::isnan(prf)) { if (min > max) { throw sizes_error; } }
				else if (min > prf || max < prf) { throw sizes_error; }
				_min.x = min;
				_prf.x = prf;
				_max.x = max;
				}
			void set_y(float min, float prf, float max)
				{
				if (std::isnan(prf)) { if (min > max) { throw sizes_error; } }
				else if (min > prf || max < prf) { throw sizes_error; }
				_min.y = min;
				_prf.y = prf;
				_max.y = max;
				}

		private:
			inline static const std::runtime_error sizes_error{"Trying to assign invalid custom sizes. The following must be true for both x and y: _min <= _max."};

			vec2f _min{/*****/0,    0}; //can be [0, _max]
			vec2f _max{/**/finf, finf}; //can be [_min, finf]
			vec2f _prf{/**/fnan, fnan}; //can be [_min, _max] or fnan
		};
#pragma endregion inner_types
	}