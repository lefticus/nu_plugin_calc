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
  std::stringstream ss;
  ss << R"({"jsonrpc": "2.0", "method": "response", "params": {"Ok": )" << response.dump() << "}}";

  puts(ss.str().c_str());
}

auto get_length(const nlohmann::json &string_value)
{
  const std::string string = string_value["item"]["Primitive"]["String"];
  const auto string_len = string.size();
  std::stringstream ss;
  ss << R"({"Primitive": {"Int": )" << string_len << "}}";
  const auto int_item = nlohmann::json::parse(ss.str());
  auto int_value = string_value;
  int_value["item"] = int_item;
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
      auto config = nlohmann::json::parse(R"({"name": "calc", "usage": "Return the length of a string", "positional": [], "named": {}, "is_filter": true})");
      print_good_response(config);
    } else if (method == "begin_filter") {
      print_good_response(nlohmann::json::parse("[]"));
    } else if (method == "filter") {
      const auto int_item = get_length(x["params"]);
      std::stringstream ss;
      ss << R"([ { "Ok" : { "Value" : )";
      ss << int_item;
      ss << "}}]";
      print_good_response(nlohmann::json::parse(ss.str()));
    } else if (method == "end_filter") {
      print_good_response(nlohmann::json::parse("[]"));
    }
  }
}

