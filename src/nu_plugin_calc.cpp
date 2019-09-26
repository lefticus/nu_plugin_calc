// TODO: Implement Me
//
//
#include <string_view>
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <iostream>
#include <sstream>

void print_good_response(const nlohmann::json &response)
{
  const auto json_response =
    nlohmann::json{ { "jsonrpc", "2.0" }, { "method", "response" }, { "params", { { "Ok", response } } } };

  puts(json_response.dump().c_str());
}

auto get_length(const nlohmann::json &string_value)
{
  const std::string string = string_value["item"]["Primitive"]["String"];
  const auto string_len = string.size();
  auto int_value = string_value;
  int_value["item"] = { { "Primitive", { { "Int", string_len } } } };
  return int_value;
}

int main()
{
  for (std::string line; std::cin.good(); std::getline(std::cin, line)) {
    //    std::cerr << "Trying to parse: '" << line << '\'' << std::endl;

    if (line.empty()) { continue; }

    const auto x = nlohmann::json::parse(line);
    const auto method = x["method"];
    if (method == "config") {
      const auto config = nlohmann::json{ { "name", "calc" },
        { "usage", "Return the length of a string" },
        { "positional", nlohmann::json::array() },
        { "named", nlohmann::json::object() },
        { "is_filter", true } };
      print_good_response(config);
    } else if (method == "begin_filter") {
      print_good_response(nlohmann::json::array());
    } else if (method == "filter") {
      const auto array =
        nlohmann::json::array({ nlohmann::json{ { "Ok", { { "Value", get_length(x["params"]) } } } } });
      print_good_response(array);
    } else if (method == "end_filter") {
      print_good_response(nlohmann::json::array());
    }
  }
}
