#include <cpp_httplib/httplib.h>
#include <common/logger.h>

#include "common/hooks.h"
#include "common/yandex.h"
#include "include/nlohmann/json.hpp"

using json = nlohmann::json;
using namespace httplib;

const std::string host = "localhost";
constexpr int port = 3000;

int main()
{
    Server srv;

    srv.Get("/webhooks", hooks_get);
    srv.Post("/webhooks", hooks_post);

    srv.Post("/alice", alice_yandex_hook);

    log(std::string("Сервер успешно запустился по адресу ") + host + std::string(u8":") + std::to_string(port), message_type::info);

    srv.listen("localhost", port);
}
