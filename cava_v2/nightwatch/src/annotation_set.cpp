#include <absl/container/flat_hash_map.h>

namespace ava {
namespace cava {
absl::flat_hash_map<std::string, bool> combine_annotations = {
    {"depends_on", true},
    {"unsupported", true},
    {"object_depends_on", true},
    {"object_record_for", true},
};

}  // namespace cava
}  // namespace ava
