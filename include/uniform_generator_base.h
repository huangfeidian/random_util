#pragma once

#include <cstdint>

namespace spiritsaway::utility::random_util
{
	class uniform_generator_base
	{
	public:
		uniform_generator_base()
		{

		}
		virtual double operator()(const double min, const double max) = 0;
		virtual ~uniform_generator_base()
		{
			
		}
	};
}