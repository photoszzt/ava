#include "c_dsl.h"

#include <absl/strings/ascii.h>

#include "result.h"

namespace ava {
namespace cava {

absl::flat_hash_set<absl::string_view> kKnownConstants = {
    {"NW_SYNC"},
    {"NW_ASYNC"},
    {"NW_FLUSH"},
    {"NW_NONE"},
    {"NW_HANDLE"},
    {"NW_OPAQUE"},
    {"NW_BUFFER"},
    {"NW_CALLBACK"},
    {"NW_CALLBACK_REGISTRATION"},
    {"NW_FILE"},
    {"NW_ZEROCOPY_BUFFER"},
    {"AVA_NONE"},
    {"AVA_COUPLED"},
    {"AVA_STATIC"},
    {"AVA_CALL"},
    {"AVA_MANUAL"},
    {"NULL"},
    {"malloc"},
    {"free"
     "ava_zerocopy_alloc"},
    {"ava_zerocopy_free"},
};

absl::flat_hash_set<bool> kBooleanConstants = {{false}, {true}};

template <typename T>
absl::optional<absl::flat_hash_set<T>> value_set_union(absl::optional<absl::flat_hash_set<T>> a,
                                                       absl::optional<absl::flat_hash_set<T>> b) {
  if (!a || !b) {
    return absl::nullopt;
  }
  // TODO
}

template <typename T>
bool value_set_isdisjoint(absl::optional<absl::flat_hash_set<T>> a, absl::optional<absl::flat_hash_set<T>> b) {
  if (!a || !b) {
    return false;
  }
  for (auto &&val : a) {
    auto found = b.find(val);
    if (found != b.end()) {
      return false;
    }
  }
  return true;
}

ava::Result<bool> parse_bool(absl::string_view v) {
  auto lower_str = absl::AsciiStrToLower(v);
  if (lower_str.compare("true") == 0 || lower_str.compare("1")) {
    return ava::Result<bool>(true);
  }
  if (lower_str.compare("false") == 0 || lower_str.compare("0")) {
    return ava::Result<bool>(false);
  }
  return ava::Result<bool>(Status(StatusCode::Invalid, "Boolean value expected"));
}

}  // namespace cava
}  // namespace ava
