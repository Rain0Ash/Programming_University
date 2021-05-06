#pragma once

#include <cstdint>
#include <unordered_map>

struct anticheat
{
private:
	static unsigned int max_cheat_requests_;
	static unsigned int cheat_request_timeout_;
	static std::unordered_map<unsigned int, anticheat>* container_;
	
	uint64_t last_ = 0;
	unsigned int cheat_requests_ = 0;

public:
	static std::unordered_map<unsigned int, anticheat>& container();
	static unsigned int get_max_cheat_requests();
	static void set_max_cheat_requests(unsigned int requests);
	static unsigned int get_cheat_request_timeout();
	static void set_cheat_request_timeout(unsigned int timeout);

	anticheat();
	uint64_t get_last_time() const;
	uint64_t set_last_time();
	uint64_t set_last_time(uint64_t time);
	unsigned int get_cheat_requests() const;
	unsigned int increase_cheat_requests();
};
