#pragma once
#include <string>


struct fishing_record_entry
{
private:
	std::string login_;
	uint64_t count_;
	static std::string* records_cache_;
	static uint64_t records_cache_time_;
public:
	fishing_record_entry(std::string login, uint64_t count);
	const std::string& get_login() const;
	uint64_t get_count() const;
	static std::string get_cache();
	static void set_cache(const std::string& cache);
	static uint64_t get_cache_time();
};
