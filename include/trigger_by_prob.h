#pragma once

#include "uniform_generator_base.h"

namespace spiritsaway::system::random_util
{
	class trigger_by_prob_interface
	{
	protected:
		double m_prob;
		uniform_generator_base& m_uniform_generator;
	public:
		trigger_by_prob_interface(uniform_generator_base& in_uniform_generator)
			: m_prob(0.0)
			, m_uniform_generator(in_uniform_generator)
			
		{

		}
		virtual bool try_trigger() = 0;
		virtual ~trigger_by_prob_interface()
		{

		}

		virtual void reset(double in_prob)
		{
			m_prob = in_prob;
		}
	};

	class trigger_by_prob_independent : public trigger_by_prob_interface
	{
	public:
		using trigger_by_prob_interface::trigger_by_prob_interface;
		
		bool try_trigger() override
		{
			if (m_prob <= 0)
			{
				return 0;
			}
			if (m_prob >= 1)
			{
				return 1;
			}
			auto cur_rand_value = m_uniform_generator(0.0, 1.0);
			if (cur_rand_value < m_prob )
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		~trigger_by_prob_independent()
		{

		}
	};

	class trigger_by_prob_pseudo_random : public trigger_by_prob_interface
	{
		std::uint32_t m_fail_count = 0;
		double m_pseudo_random_prob;
	public:
		trigger_by_prob_pseudo_random(uniform_generator_base& in_uniform_generator)
			: trigger_by_prob_interface(in_uniform_generator)
			, m_fail_count(0)
			, m_pseudo_random_prob(0)
		{

		}
		void reset(double in_prob) override
		{
			trigger_by_prob_interface::reset(in_prob);
			m_fail_count = 0;
			m_pseudo_random_prob = gen_pseudo_random_prob(m_prob);
		}
		static double gen_pseudo_random_prob(double in_prob);

		bool try_trigger() override;
	};
}