// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "anticheat.hpp"

#include "utils.hpp"

unsigned int anticheat::max_cheat_requests_ = 10;
unsigned int anticheat::cheat_request_timeout_ = 1000;

std::unordered_map<unsigned int, anticheat>* anticheat::container_ = new std::unordered_map<unsigned int, anticheat>;

std::unordered_map<unsigned, anticheat>& anticheat::container()
{
	return *container_;
}

unsigned anticheat::get_max_cheat_requests()
{
	return max_cheat_requests_;
}

void anticheat::set_max_cheat_requests(const unsigned int requests)
{
	max_cheat_requests_ = requests;
}

unsigned int anticheat::get_cheat_request_timeout()
{
	return cheat_request_timeout_;
}

void anticheat::set_cheat_request_timeout(const unsigned int timeout)
{
	cheat_request_timeout_ = timeout;
}

anticheat::anticheat()
{
	set_last_time();
}

uint64_t anticheat::get_last_time() const
{
	return this->last_;
}

uint64_t anticheat::set_last_time()
{
	return set_last_time(utils::time_epoch());
}

uint64_t anticheat::set_last_time(const uint64_t time)
{
	this->last_ = time;
	return time;
}

unsigned anticheat::get_cheat_requests() const
{
	return this->cheat_requests_;
}

unsigned anticheat::increase_cheat_requests()
{
	return ++this->cheat_requests_;
}
