
#pragma once

#include <random>
#include <cstdint>
#include <vector>
#include <array>

#include "uniform_generator_base.h"
namespace spiritsaway::utility::random_util
{
	struct item_weight_config
	{
		std::uint32_t item_id;
		std::uint32_t item_weight;
	};

	class choose_by_weight_interface
	{
	protected:
		std::vector<item_weight_config> m_weights;
		uniform_generator_base& m_uniform_generator;
	public:
		choose_by_weight_interface(uniform_generator_base& in_uniform_generator)
			: m_uniform_generator(in_uniform_generator)
		{

		}
		virtual void reset(const std::vector<item_weight_config>& in_weights)
		{
			m_weights = in_weights;
		}
		const auto& weights() const
		{
			return m_weights;
		}
		virtual std::uint32_t choose() = 0;

		~choose_by_weight_interface()
		{

		}

	};

	class choose_by_weight_naive : public choose_by_weight_interface
	{
	public :
		using choose_by_weight_interface::choose_by_weight_interface;
		
		std::uint32_t choose() override;
	};

	class choose_by_weight_naive_with_total : public choose_by_weight_interface
	{
		std::uint32_t m_total_weight = 0;
	public:
		using choose_by_weight_interface::choose_by_weight_interface;
		void reset(const std::vector<item_weight_config>& in_weights) override;
		std::uint32_t choose() override;
	};

	class choose_by_weight_prefix_sum : public choose_by_weight_interface
	{
		std::vector<std::uint32_t> m_sum_weights;
	public:
		using choose_by_weight_interface::choose_by_weight_interface;
		void reset(const std::vector<item_weight_config>& in_weights) override;
		std::uint32_t choose() override;
		virtual ~choose_by_weight_prefix_sum()
		{

		}
	};

	class choose_by_weight_sorted : public choose_by_weight_interface
	{

		std::vector<std::uint32_t> m_sum_weights;
	public:
		using choose_by_weight_interface::choose_by_weight_interface;
		
		void reset(const std::vector<item_weight_config>& in_weights) override;
		std::uint32_t choose() override;
		virtual ~choose_by_weight_sorted()
		{

		}
	};

	class choose_by_weight_repeated : public choose_by_weight_interface
	{

		std::vector<double> m_relative_weights;
		double m_p_max = 0;
	public:
		using choose_by_weight_interface::choose_by_weight_interface;
		void reset(const std::vector<item_weight_config>& in_weights) override;
		std::uint32_t choose() override;
		virtual ~choose_by_weight_repeated()
		{

		}
	};

	class choose_by_weight_alias_table : public choose_by_weight_interface
	{
		// 描述[k,k+1)的拼接信息
		struct split_info
		{
			float self_prob = 0; // 每个小矩形的高度
			uint32_t self_index = 0; // 每个小矩形的原始样本id
		};
		std::vector<std::array<split_info, 2>> m_rect_split_infos;
	public:
		using choose_by_weight_interface::choose_by_weight_interface;
		void reset(const std::vector<item_weight_config>& in_weights) override;
		std::uint32_t choose() override;
		virtual ~choose_by_weight_alias_table()
		{

		}
	};
}