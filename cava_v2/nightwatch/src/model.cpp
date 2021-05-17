#include <sstream>
#include <vector>

std::string lines(std::vector<std::string> strs) {
  std::stringstream ss;
  for (auto &&s : strs) {
    ss << s << "\n";
  }
  return ss.str();
}
