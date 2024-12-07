#include "trigger_by_prob.h"
#include <array>
#include <cmath>

namespace
{
	double PfromC(double C)
	{
		double pProcOnN = 0;
		double pProcByN = 0;
		double sumNpProcOnN = 0;

		int maxFails = (int)std::ceil(1 / C);
		for (int N = 1; N <= maxFails; ++N)
		{
			pProcOnN = std::min(1.0, N * C) * (1 - pProcByN);
			pProcByN += pProcOnN;
			sumNpProcOnN += N * pProcOnN;
		}

		return (1 / sumNpProcOnN);
	}

	double CfromP(double p)
	{
		double Cupper = p;
		double Clower = 0;
		double Cmid;
		double p1;
		double p2 = 1;
		while (true)
		{
			Cmid = (Cupper + Clower) / 2;
			p1 = PfromC(Cmid);
			if (std::abs(p1 - p2) <= 0) break;

			if (p1 > p)
			{
				Cupper = Cmid;
			}
			else
			{
				Clower = Cmid;
			}

			p2 = p1;
		}

		return Cmid;
	}
}
namespace spiritsaway::utility::random_util
{
	bool trigger_by_prob_pseudo_random::try_trigger()
	{
		if (m_pseudo_random_prob <= 0)
		{
			return 0;
		}
		if (m_pseudo_random_prob >= 1)
		{
			return 1;
		}
		auto cur_temp_prob = (m_fail_count + 1) * m_pseudo_random_prob;
		auto cur_rand_value = m_uniform_generator(0.0, 1.0);
		if (cur_rand_value < cur_temp_prob)
		{
			m_fail_count = 0;
			return true;
		}
		m_fail_count++;
		return false;
	}

	double trigger_by_prob_pseudo_random::gen_pseudo_random_prob(double in_prob)
	{
		static std::array<double, 100> m_prob_table = { 0.0 };

		auto cur_index = int(std::floor(in_prob * 100));
		if (cur_index <= 0)
		{
			return 0;
		}
		if(cur_index >= 100)
		{
			return 1;
		}
		if (m_prob_table[cur_index] != 0)
		{
			return m_prob_table[cur_index];
		}
		else
		{
			auto result = CfromP(cur_index * 0.01);
			m_prob_table[cur_index] = result;
			return result;
		}


	}
}