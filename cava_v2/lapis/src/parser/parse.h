#pragma once

namespace ava {
namespace cava {
constexpr std::string_view grammer_file_name = "lapis.tx";

std::string preprocess_lapis(std::string_view include_path);
void parse(std::string_view fn, std::string_view include_path);
}  // namespace cava
}  // namespace ava
