#pragma once

#include "uniform_generator_base.h"
#include <random>

namespace spiritsaway::system::random_util
{
	class uniform_generator_by_std: public uniform_generator_base
	{
	private:
		std::minstd_rand m_rand_gen;
		std::uniform_real_distribution<double> m_real_dist;
	public:
		uniform_generator_by_std()
			:uniform_generator_base()
			, m_rand_gen(std::random_device{}())
			, m_real_dist(0.0, 1.0)

		{

		}
		double operator()(const double min, const double max) override
		{
			return m_real_dist(m_rand_gen) *(max - min) + min;
		}
		~uniform_generator_by_std()
		{

		}
	};
}
