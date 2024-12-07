#include "trigger_by_prob.h"
#include "uniform_generator_imp.h"
#include <iostream>
using namespace spiritsaway::system::random_util;

void print_trig_stat(trigger_by_prob_interface& cur_trigger, int num)
{
	std::vector<int> num_stat(num, 0);
	int last_suc_seq = -1;
	int suc_count = 0;
	int try_count = 0;
	while (suc_count < num)
	{
		try_count++;
		if (cur_trigger.try_trigger())
		{
			num_stat[try_count - last_suc_seq]++;
			last_suc_seq = try_count;
			suc_count++;
		}
	}
	for (std::uint32_t i = 0; i < num; i++)
	{
		if (num_stat[i])
		{
			std::cout << "|" << i << "|" << num_stat[i] << "|"<<std::endl;
		}
	}
}
void test_1(double prob, int num)
{
	auto cur_rand_generator = uniform_generator_by_std();
	auto independent_trigger = trigger_by_prob_independent(cur_rand_generator);
	independent_trigger.reset(prob);
	auto pseudo_trigger = trigger_by_prob_pseudo_random(cur_rand_generator);
	pseudo_trigger.reset(prob);
	std::cout << "result for independent_trigger" << std::endl;
	print_trig_stat(independent_trigger, num);
	std::cout << "result for pseudo_trigger" << std::endl;
	print_trig_stat(pseudo_trigger, num);
	
}

int main()
{
	test_1(0.25,100);
}