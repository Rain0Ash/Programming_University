#pragma once

#include <cstdint>
#include <unordered_map>

struct anticheat
{
private:
    static std::unordered_map<unsigned int, anticheat>* container_;
	
    uint64_t last_ = 0;
    unsigned int cheat_requests_ = 0;

public:
    static std::unordered_map<unsigned int, anticheat>& container();
	
    anticheat();
    uint64_t get_last_time() const;
    uint64_t set_last_time();
    uint64_t set_last_time(uint64_t time);
    unsigned int get_cheat_requests() const;
    unsigned int increase_cheat_requests();
};
