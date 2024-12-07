#include "choose_by_weight.h"
#include "uniform_generator_imp.h"
#include <iostream>

using namespace spiritsaway::utility::random_util;

double test_1(choose_by_weight_interface& cur_chooser, int num)
{
	const auto& origin_weights = cur_chooser.weights();
	std::vector<std::uint32_t> result_count(origin_weights.size(), 0);
	for (int i = 0; i < num; i++)
	{
		auto cur_result = cur_chooser.choose();
		result_count[cur_result]++;
	}
	std::vector<double> pre_weight_ratio(origin_weights.size(), 0);
	std::uint32_t total_weights = 0;
	
	for (auto one_item : origin_weights)
	{
		total_weights += one_item.item_weight;
	}
	for (std::uint32_t i = 0; i < origin_weights.size(); i++)
	{
		pre_weight_ratio[i] = origin_weights[i].item_weight * 1.0 / total_weights;
	}

	total_weights = 0;
	std::vector<double> new_weight_ratio(origin_weights.size(), 0);

	for (auto one_item : result_count)
	{
		total_weights += one_item;
	}
	for (std::uint32_t i = 0; i < result_count.size(); i++)
	{
		new_weight_ratio[i] = result_count[i]* 1.0 / total_weights;
	}

	double ratio_diff = 0;
	for (std::uint32_t i = 0; i < result_count.size(); i++)
	{
		ratio_diff += std::abs(new_weight_ratio[i] - pre_weight_ratio[i]);
	}
	return ratio_diff;
}

void test_2(std::vector<item_weight_config> in_config, int num)
{
	auto cur_rand_generator = uniform_generator_by_std();
	auto cur_naive_chooser = choose_by_weight_naive(cur_rand_generator);
	cur_naive_chooser.reset(in_config);
	std::cout << "diff for cur_naive_chooser is " << test_1(cur_naive_chooser, num) << std::endl;

	auto cur_naive_with_total_chooser = choose_by_weight_naive_with_total(cur_rand_generator);
	cur_naive_with_total_chooser.reset(in_config);
	std::cout << "diff for cur_naive_with_total_chooser is " << test_1(cur_naive_with_total_chooser, num) << std::endl;

	auto cur_prefix_sum_chooser = choose_by_weight_prefix_sum(cur_rand_generator);
	cur_prefix_sum_chooser.reset(in_config);
	std::cout << "diff for cur_prefix_sum_chooser is " << test_1(cur_prefix_sum_chooser, num) << std::endl;

	auto cur_sorted_chooser = choose_by_weight_sorted(cur_rand_generator);
	cur_sorted_chooser.reset(in_config);
	std::cout << "diff for cur_sorted_chooser is " << test_1(cur_sorted_chooser, num) << std::endl;

	auto cur_repeated_chooser = choose_by_weight_repeated(cur_rand_generator);
	cur_repeated_chooser.reset(in_config);
	std::cout << "diff for cur_repeated_chooser is " << test_1(cur_repeated_chooser, num) << std::endl;

	auto cur_alias_table_chooser = choose_by_weight_alias_table(cur_rand_generator);
	cur_alias_table_chooser.reset(in_config);
	std::cout << "diff for cur_alias_table_chooser is " << test_1(cur_alias_table_chooser, num) << std::endl;
}

std::vector<item_weight_config> gen_weight_config(int num)
{
	std::vector<item_weight_config> result(num);
	auto cur_rand_generator = uniform_generator_by_std();
	std::cout << "weights is : ";
	for (int i = 0; i < num; i++)
	{
		result[i].item_id = i;
		result[i].item_weight = cur_rand_generator(1, 50);
		std::cout << result[i].item_weight << ",";
	}
	std::cout << std::endl;
	return result;
}
std::vector<item_weight_config> gen_weight_config(std::vector<std::uint32_t> in_weigts)
{
	std::vector<item_weight_config> result(in_weigts.size());
	std::cout << "weights is : ";
	for (int i = 0; i < in_weigts.size(); i++)
	{
		result[i].item_id = i;
		result[i].item_weight = in_weigts[i];
		std::cout << result[i].item_weight << ",";
	}
	std::cout << std::endl;
	return result;
}


void test_4(int weight_num, int case_num)
{
	auto cur_weight_config = gen_weight_config(weight_num);
	// auto cur_weight_config = gen_weight_config(std::vector<std::uint32_t>{28, 31, 30, 47, 8, 43, 44, 33, 33, 13});
	test_2(cur_weight_config, case_num);
}
int main()
{
	test_4(20, 10000);
}
