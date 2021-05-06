#pragma once
	
#include <ctime>
#include <string>


struct fishing_entry
{
private:
	unsigned long long int id_;
	unsigned int count_;
	time_t created_at_;
public:
	fishing_entry();
	fishing_entry(unsigned long long int id, unsigned int count, time_t created_at);
	unsigned long long int get_id() const;
	unsigned int get_count() const;
	time_t get_create_date() const;
};