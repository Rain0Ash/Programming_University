// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "fishing_record_entry.hpp"

#include "utils.hpp"

std::string* fishing_record_entry::records_cache_;
uint64_t fishing_record_entry::records_cache_time_;

fishing_record_entry::fishing_record_entry(std::string login, const uint64_t count) : login_(std::move(login)), count_(count)
{
}

const std::string& fishing_record_entry::get_login() const
{
	return this->login_;
}

uint64_t fishing_record_entry::get_count() const
{
	return this->count_;
}

std::string fishing_record_entry::get_cache()
{
	if (records_cache_)
	{
		return *records_cache_;
	}

	return std::string();
}

void fishing_record_entry::set_cache(const std::string& cache)
{
	delete records_cache_;
	records_cache_ = new std::string(cache);
	records_cache_time_ = utils::time_epoch();
}

uint64_t fishing_record_entry::get_cache_time()
{
	return records_cache_time_;
}