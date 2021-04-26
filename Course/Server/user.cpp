#include "user.hpp"

std::string user::peper_;
std::unordered_map<std::string, connection*>* connections::connections_by_apikey_ = new std::unordered_map<std::string, connection*>;
std::unordered_map<std::string, connection*>* connections::connections_by_login_ = new std::unordered_map<std::string, connection*>;