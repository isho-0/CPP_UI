#pragma once

#include <vector>
#include <optional>

#include "core/core.h"
#include "widgets/window_drag.h"

namespace UI
	{
	class initializer
		{
		public:
			initializer(utils::MS::graphics::d2d::device_context context) : context_ptr{context}, debug_brushes { context } {}

			const core::debug_brushes& get_debug_brushes() const noexcept { return debug_brushes; }

		private:
			utils::MS::graphics::d2d::device_context context_ptr;
			core::debug_brushes debug_brushes;
		};
	}