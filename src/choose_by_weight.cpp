#include "choose_by_weight.h"
#include <cassert>

namespace spiritsaway::utility::random_util
{
	std::uint32_t choose_by_weight_naive::choose()
	{
		std::uint32_t total_weight = 0;
		for (const auto& one_weight_config : m_weights)
		{
			total_weight += one_weight_config.item_weight;
		}
		std::uint32_t temp_random_value = std::uint32_t(std::floor(m_uniform_generator(0, total_weight)));
		for (const auto& one_weight_config : m_weights)
		{
			if (temp_random_value < one_weight_config.item_weight)
			{
				return one_weight_config.item_id;
			}
			temp_random_value -= one_weight_config.item_weight;
		}
		return m_weights.back().item_id;
	}

	void choose_by_weight_naive_with_total::reset(const std::vector<item_weight_config>& in_weights)
	{
		choose_by_weight_interface::reset(in_weights);
		m_total_weight = 0;
		for (const auto& one_weight_config : m_weights)
		{
			m_total_weight += one_weight_config.item_weight;
		}
	}

	std::uint32_t choose_by_weight_naive_with_total::choose()
	{
		std::uint32_t temp_random_value = std::uint32_t(std::floor(m_uniform_generator(0, m_total_weight)));
		for (const auto& one_weight_config : m_weights)
		{
			if (temp_random_value < one_weight_config.item_weight)
			{
				return one_weight_config.item_id;
			}
			temp_random_value -= one_weight_config.item_weight;
		}
		return m_weights.back().item_id;
	}

	void choose_by_weight_prefix_sum::reset(const std::vector<item_weight_config>& in_weights)
	{
		choose_by_weight_interface::reset(in_weights);
		m_sum_weights.resize(m_weights.size());
		std::uint32_t total_weight = 0;
		for (std::uint32_t i = 0; i < m_weights.size(); i++)
		{
			total_weight += m_weights[i].item_weight;
			m_sum_weights[i] = total_weight;
		}
	}
	std::uint32_t choose_by_weight_prefix_sum::choose()
	{
		std::uint32_t temp_random_value = std::uint32_t(std::floor(m_uniform_generator(0, m_sum_weights.back())));
		for (std::uint32_t i = 0; i < m_sum_weights.size(); i++)
		{
			if (temp_random_value < m_sum_weights[i])
			{
				return m_weights[i].item_id;
			}
		}
		return m_weights.back().item_id;
	}

	void choose_by_weight_sorted::reset(const std::vector<item_weight_config>& in_weights)
	{
		choose_by_weight_interface::reset(in_weights);
		m_sum_weights.resize(m_weights.size());
		std::uint32_t total_weight = 0;
		for (std::uint32_t i = 0; i < m_weights.size(); i++)
		{
			total_weight += m_weights[i].item_weight;
			m_sum_weights[i] = total_weight;
		}
	}
	std::uint32_t choose_by_weight_sorted::choose()
	{
		std::uint32_t temp_random_value = std::uint32_t(std::floor(m_uniform_generator(0, m_sum_weights.back())));
		auto temp_iter = std::upper_bound(m_sum_weights.begin(), m_sum_weights.end(), temp_random_value);
		return m_weights[std::distance(m_sum_weights.begin(), temp_iter)].item_id;
	}

	void choose_by_weight_repeated::reset(const std::vector<item_weight_config>& in_weights)
	{
		choose_by_weight_interface::reset(in_weights);
		m_relative_weights.resize(m_weights.size());
		m_p_max = 0.0;
		std::uint32_t total_weight = 0;
		
		for (std::uint32_t i = 0; i < m_weights.size(); i++)
		{
			total_weight += m_weights[i].item_weight;
		}
		for (std::uint32_t i = 0; i < m_weights.size(); i++)
		{
			m_relative_weights[i] = m_weights[i].item_weight * 1.0f / total_weight;
			m_p_max = std::max(m_p_max, m_relative_weights[i]);
		}
	}
	std::uint32_t choose_by_weight_repeated::choose()
	{
		while (true)
		{
			std::uint32_t random_index = std::uint32_t(std::floor((m_uniform_generator(0, m_weights.size()))));
			float random_weight = m_uniform_generator(0, m_p_max);
			if (random_weight <= m_relative_weights[random_index])
			{
				return m_weights[random_index].item_id;
			}
		}
	}
	/*
	* �������������һ�ָ�Ч���㷨����`A`�����`B`��������췽�����Ա���ÿ��ɨ����������:

		1. ���ȴ�`A`���鹹������������`vector<pair<uint32_t, float>> C,D`�� `C`�д洢ԭ���߶ȴ���`1`������Ԫ�ص������͸߶ȣ� `D`�д洢ԭ���߶�С�ڵ���`1`������Ԫ�ص������͸߶�
		2. ÿ�δ�`D`��ĩβ����һ��Ԫ��`E`��
			1. ���`E.second==1`��������ʱ����Ҫ�������������з֣�ִ��`B[E.first][0] = {E.first, E.second}`
			2. ���`E.second<1`����ʱ��`C`��ȡĩβԪ��`F`����`F`���г�һ����`1-E.second`��ʹ��`E.first`��Ӧ���������Ϊ`1`,������з�ȥ����`B[E.first]`
		3. �ж�`F.second`�Ƿ�С�ڵ���`1`�����С�ڵ���`1`��`F`��`C`��ĩβת�Ƶ�`D`��ĩβ
		4. ���`D`��Ϊ�գ�����ݵ�����`2`�ٴ�ִ�У����`D`Ϊ�գ�����`B`��Ϊ�����
	*/
	void choose_by_weight_alias_table::reset(const std::vector<item_weight_config>& in_weights)
	{
		choose_by_weight_interface::reset(in_weights);

		m_rect_split_infos.resize(m_weights.size());
		std::vector<std::pair<uint32_t, float>> weights_gt_1;
		std::vector<std::pair<uint32_t, float>> weights_le_1;
		std::uint32_t total_weight = 0;
		for (std::uint32_t i = 0; i <m_weights.size(); i++)
		{
			total_weight += m_weights[i].item_weight;
			// ���⸡�����Ԥ�����
			m_rect_split_infos[i][0].self_index = i;
			m_rect_split_infos[i][0].self_prob = 1.0;
		}

		for (std::uint32_t i = 0; i <m_weights.size(); i++) // ��Ӧ����1
		{
			float relative_weight =m_weights.size() * m_weights[i].item_weight * 1.0f / total_weight;
			if (relative_weight > 1)
			{
				weights_gt_1.push_back(std::make_pair(i, relative_weight));
			}
			else
			{
				weights_le_1.push_back(std::make_pair(i, relative_weight));
			}
		}
		while (!weights_le_1.empty()) // ��Ӧ����2
		{
			auto temp_le_1_back = weights_le_1.back();
			weights_le_1.pop_back();
			if (temp_le_1_back.second >= 0.999) // �Ѿ���1�� ����Ҫ�ٴ������
			{
				m_rect_split_infos[temp_le_1_back.first][0].self_index = temp_le_1_back.first;
				m_rect_split_infos[temp_le_1_back.first][0].self_prob = 1.0;
				continue;
			}
			assert(!weights_gt_1.empty());
			auto& temp_gt_1_back = weights_gt_1.back();

			m_rect_split_infos[temp_le_1_back.first][0].self_index = temp_le_1_back.first;
			m_rect_split_infos[temp_le_1_back.first][0].self_prob = temp_le_1_back.second;
			m_rect_split_infos[temp_le_1_back.first][1].self_index = temp_gt_1_back.first;
			m_rect_split_infos[temp_le_1_back.first][1].self_prob = 1 - temp_le_1_back.second;

			temp_gt_1_back.second -= 1 - temp_le_1_back.second;

			if (temp_gt_1_back.second < 1) // ��Ӧ����3
			{
				weights_le_1.push_back(temp_gt_1_back);
				weights_gt_1.pop_back();
			}
		}
	}
	std::uint32_t choose_by_weight_alias_table::choose()
	{
		float temp_random_value = m_uniform_generator(0, m_rect_split_infos.size());
		uint32_t base_index = uint32_t(std::floor(temp_random_value));
		float remain_random_value = temp_random_value - base_index;
		if (remain_random_value <= m_rect_split_infos[base_index][0].self_prob)
		{
			return m_rect_split_infos[base_index][0].self_index;
		}
		else
		{
			return m_rect_split_infos[base_index][1].self_index;
		}
	}
}