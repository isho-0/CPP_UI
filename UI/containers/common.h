#pragma once

#include <utils/index_range.h>

#include "../core/core.h"

namespace UI::containers::details
	{
	struct constraints_t 
		{
		float min;
		float prf;
		float max;
		float max_finite;

		static constraints_t hor(const core::element& element) noexcept
			{
			return 
				{
				element.get_size_min().x,
				element.get_size_prf().x,
				element.get_size_max().x
				};
			}

		static constraints_t ver(const core::element& element) noexcept
			{
			return
				{
				element.get_size_min().y,
				element.get_size_prf().y,
				element.get_size_max().y
				};
			}

		constraints_t(float min, float prf, float max) noexcept :
			min{min}, prf{prf}, max{max}, 
			max_finite{max != core::finf ? max : prf}
			{}
		};

	template <typename constraints_iterable_t>
	inline std::vector<float> calc_sizes(float available, const constraints_iterable_t& constraints) noexcept
		{
		std::vector<float> ret; ret.resize(constraints.size());

		// Calc total sums and available without minimum offset
		float mins_sum{0.f};
		float prfs_sum{0.f};
		float maxs_sum{0.f};
		float maxs_max_finite{0.f};
		float maxs_finite_sum{0.f};
		float maxs_infinite_count{0};

		for (auto& constraint : constraints)
			{
			mins_sum += constraint.min;
			prfs_sum += constraint.prf;

			maxs_sum += constraint.max;
			maxs_finite_sum += constraint.max_finite;
			maxs_max_finite = std::max(constraint.max_finite, maxs_max_finite);

			if (constraint.max == core::finf) { maxs_infinite_count++; }
			}

		float maxs_finite_sum_plus_infinites_as_max{maxs_finite_sum + (maxs_max_finite * maxs_infinite_count)};

		if (available < mins_sum) 
			{
			std::cout << "ERR!\n";
			return ret;
			} //TODO error case
		
		auto loop{[&constraints, &ret, &available](float bound_lower, float bound_upper, auto getter_lower, auto getter_upper)
			{
			float available_range{available - bound_lower};
			float range_sum{bound_upper - bound_lower};

			for (auto index : utils::indices(constraints))
				{
				const auto& constraint{constraints[index]};
				float range{getter_upper(constraint) - getter_lower(constraint)};

				float proportion{range / range_sum};

				ret[index] = getter_lower(constraint) + (available_range * proportion);
				}
			}};

		if (available < prfs_sum)
			{
			loop(mins_sum, prfs_sum, [](const auto& c) { return c.min; }, [](const auto& c) { return c.prf; });
			}
		else if (available == prfs_sum)
			{
			for (auto index : utils::indices(constraints)) { ret[index] = constraints[index].prf; }
			}
		else if(available < maxs_finite_sum)
			{
			loop(prfs_sum, maxs_finite_sum, [](const auto& c) { return c.prf; }, [](const auto& c) { return c.max_finite; });
			}
		else
			{
			float available_range{available - maxs_finite_sum};
			float add_to_each_infinite{available_range / maxs_infinite_count};

			for (auto index : utils::indices(constraints))
				{
				const auto& constraint{constraints[index]};
				ret[index] = constraint.max_finite + ((constraint.max != core::finf) ? 0 : add_to_each_infinite);
				}
			}
		return ret;


		float ranges_sum{maxs_sum - mins_sum};
		float to_fill{std::min(available, maxs_sum)};
		float left_for_infinites{available - to_fill};
		float add_to_each_infinite{left_for_infinites / maxs_infinite_count};

		for (auto index : utils::indices(constraints))
			{
			const auto& constraint{constraints[index]};
			float my_max{constraint.max != core::finf ? constraint.max : constraint.prf};
			float range{my_max - constraint.min};

			float proportion{range / ranges_sum};

			ret[index] = constraint.min + (to_fill * proportion);
			if (constraint.max == core::finf)
				{
				ret[index] += add_to_each_infinite;
				}
			}

		return ret;
		}
	}