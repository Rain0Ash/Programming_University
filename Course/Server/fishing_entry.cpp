// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "fishing_entry.hpp"

#include "utils.hpp"


fishing_entry::fishing_entry() : id_(0), count_(0), created_at_(0)
{
}

fishing_entry::fishing_entry(const unsigned long long id, const unsigned count, const time_t created_at) : id_(id), count_(count), created_at_(created_at)
{
}

unsigned long long fishing_entry::get_id() const
{
	return this->id_;
}

unsigned fishing_entry::get_count() const
{
	return this->count_;
}

time_t fishing_entry::get_create_date() const
{
	return this->created_at_;
}
