#include <string_view>
#include <iostream>
#include <regex>
#include <sstream>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

void print_good_response(const nlohmann::json &response)
{
  const auto json_response =
    nlohmann::json{ { "jsonrpc", "2.0" }, { "method", "response" }, { "params", { { "Ok", response } } } };

  puts(json_response.dump().c_str());
}

struct Calculator
{
  Calculator(spdlog::logger &logger) : m_logger{ logger } {}

  std::reference_wrapper<spdlog::logger> m_logger;
  std::map<std::string, double> variables;


  static std::regex make_regex()
  {
    const std::string_view floating_point = R"(([+-]?([0-9]*[.])?[0-9]+))";
    const std::string_view operators = R"(([-*/+]))";
    const std::string_view identifier = R"(([a-z]+))";

    return std::regex{ fmt::format(R"(\s*({id}\s*=\s*)?({fp}|{id})\s*{op}\s*({fp}|{id})\s*)",
      fmt::arg("id", identifier),
      fmt::arg("fp", floating_point),
      fmt::arg("op", operators)) };
  }

  enum Regex_Matches {
    Result_Identifier = 2,
    Param_1 = 3,
    Param_1_Number = 4,
    Param_1_Identifier = 6,
    Operation = 7,
    Param_2 = 8,
    Param_2_Number = 9,
    Param_2_Identifier = 11,
  };

  std::regex expression_regex = make_regex();

  auto evaluate_expression(const char operation, const double lhs, const double rhs)
  {
    switch (operation) {
    case '-':
      return lhs - rhs;
    case '+':
      return lhs + rhs;
    case '*':
      return lhs * rhs;
    case '/':
      return lhs / rhs;
    default:
      m_logger.get().error("Unknown operator! '{}'", operation);
      return 0.0;
    }
  }

  auto calculate(const nlohmann::json &string_value)
  {
    const auto string = static_cast<std::string>(string_value["item"]["Primitive"]["String"]);

    const auto get_parameter_value = [&](const auto &value, const auto &identifier) {
      if (identifier.matched) {
        return variables[identifier];
      } else {
        return std::stod(value);
      }
    };

    const auto result = [&]() {
      if (std::smatch results; std::regex_match(string, results, expression_regex)) {
        for (const auto &match : results) { m_logger.get().trace("Regex result: '{}'", match.str()); }

        const auto lhs = get_parameter_value(results[Param_1_Number], results[Param_1_Identifier]);
        const auto operation = *results[Operation].first;
        const auto rhs = get_parameter_value(results[Param_2_Number], results[Param_2_Identifier]);


        const auto expression_result = evaluate_expression(operation, lhs, rhs);

        if (results[Result_Identifier].matched) { variables[results[Result_Identifier]] = expression_result; }

        m_logger.get().info("Parsed: '{}' = '{}' '{}' '{}'; Evaluated '{} {} {}' = {}",
          results[Result_Identifier].str(),
          results[Param_1].str(),
          operation,
          results[Param_2].str(),
          lhs,
          operation,
          rhs,
          expression_result);


        return expression_result;
      } else {
        m_logger.get().info("No expression parsed");
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
  auto console_logger{ spdlog::stderr_color_mt("console_logger") };
  console_logger->set_level(spdlog::level::info);
  console_logger->set_pattern("[%Y-%m-%d %H:%M:%S %z] [%n] [%^%l%$] %v");
  console_logger->info("nu_plugin_calc_starting");

  Calculator calculator{ *console_logger };

  for (std::string line; std::cin.good(); std::getline(std::cin, line)) {
    console_logger->trace("Trying to parse: '{}'", line);

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
