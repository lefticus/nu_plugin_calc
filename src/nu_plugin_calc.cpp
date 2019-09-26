// TODO: Implement Me
//
//
#include <string_view>
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <iostream>
#include <regex>
#include <sstream>

void print_good_response(const nlohmann::json &response)
{
  const auto json_response =
    nlohmann::json{ { "jsonrpc", "2.0" }, { "method", "response" }, { "params", { { "Ok", response } } } };

  puts(json_response.dump().c_str());
}

struct Calculator
{
  std::map<std::string, double> variables;

  static std::regex make_regex()
  {
    const std::string_view floating_point = R"(([+-]?([0-9]*[.])?[0-9]+))";
    const std::string_view operators = R"(([-*/+]))";
    const std::string_view identifier = R"(([a-z]+))";
    return std::regex{ fmt::format(R"(\s*({}\s*=\s*)?({}|{})\s*{}\s*({}|{})\s*)",
      identifier,
      floating_point,
      identifier,
      operators,
      floating_point,
      identifier) };
  }

  auto calculate(const nlohmann::json &string_value)
  {
    const static std::regex regex{ make_regex() };

    const std::string string = string_value["item"]["Primitive"]["String"];

    const auto get_value = [&](const auto &value, const auto &identifier) {
      if (identifier.matched) {
        return variables[identifier];
      } else {
        return std::stod(value);
      }
    };

    const auto result = [&]() {
      if (std::smatch results; std::regex_match(string, results, regex)) {
        for (const auto &match : results) { std::cerr << "Result: '" << match << "'\n"; }

        const auto param1 = get_value(results[4], results[6]);
        const auto operation = *results[7].first;
        const auto param2 = get_value(results[9], results[11]);

        const auto value = [&]() {
          switch (operation) {
          case '-':
            return param1 - param2;
          case '+':
            return param1 + param2;
          case '*':
            return param1 * param2;
          case '/':
            return param1 / param2;
          }
        }();

        if (results[2].matched) { variables[results[2]] = value; }

        return value;
      } else {
        return 0.0;
      }
    }();


    auto int_value = string_value;
    int_value["item"] = { { "Primitive", { { "Decimal", result } } } };
    return int_value;
  }
};

int main()
{
  Calculator calculator;

  for (std::string line; std::cin.good(); std::getline(std::cin, line)) {
    //    std::cerr << "Trying to parse: '" << line << '\'' << std::endl;

    if (line.empty()) { continue; }

    const auto x = nlohmann::json::parse(line);
    const auto method = x["method"];
    if (method == "config") {
      const auto config = nlohmann::json{ { "name", "calc" },
        { "usage", "Performs mathematical operations separated by new lines." },
        { "positional", nlohmann::json::array() },
        { "named", nlohmann::json::object() },
        { "is_filter", true } };
      print_good_response(config);
    } else if (method == "begin_filter") {
      print_good_response(nlohmann::json::array());
    } else if (method == "filter") {
      const auto array =
        nlohmann::json::array({ nlohmann::json{ { "Ok", { { "Value", calculator.calculate(x["params"]) } } } } });
      print_good_response(array);
    } else if (method == "end_filter") {
      print_good_response(nlohmann::json::array());
    }
  }
}
