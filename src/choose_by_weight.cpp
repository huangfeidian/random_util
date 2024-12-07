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
	* 下面我们来提出一种高效的算法来从`A`构造出`B`，这个构造方法可以避免每次扫描整个数组:

		1. 首先从`A`数组构造两个新数组`vector<pair<uint32_t, float>> C,D`， `C`中存储原来高度大于`1`的所有元素的索引和高度， `D`中存储原来高度小于等于`1`的所有元素的索引和高度
		2. 每次从`D`的末尾弹出一个元素`E`，
			1. 如果`E.second==1`，则代表此时不需要其他样本进行切分，执行`B[E.first][0] = {E.first, E.second}`
			2. 如果`E.second<1`，此时从`C`获取末尾元素`F`，从`F`中切除一部分`1-E.second`，使得`E.first`对应的区域被填充为`1`,以这个切分去构造`B[E.first]`
		3. 判断`F.second`是否小于等于`1`，如果小于等于`1`则将`F`从`C`的末尾转移到`D`的末尾
		4. 如果`D`不为空，则回溯到步骤`2`再次执行；如果`D`为空，返回`B`作为结果。
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
			// 避免浮点误差预先填充
			m_rect_split_infos[i][0].self_index = i;
			m_rect_split_infos[i][0].self_prob = 1.0;
		}

		for (std::uint32_t i = 0; i <m_weights.size(); i++) // 对应步骤1
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
		while (!weights_le_1.empty()) // 对应步骤2
		{
			auto temp_le_1_back = weights_le_1.back();
			weights_le_1.pop_back();
			if (temp_le_1_back.second >= 0.999) // 已经是1了 不需要再处理填充
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

			if (temp_gt_1_back.second < 1) // 对应步骤3
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