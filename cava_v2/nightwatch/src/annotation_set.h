#pragma once
#include <absl/container/flat_hash_map.h>

#include <any>

#include "common/common.h"

namespace ava {
namespace cava {

class AnnotationSet {
 public:
 private:
  absl::flat_hash_map depends_on<std::any> depends_on{};
  absl::flat_hash_map depends_on<std::any> object_depends_on{};
  object_record{false};
  Expr<std::string> object_explicit_state_replace{Expr("NULL")};
  Expr<std::string> object_explicit_state_extract{Expr("NULL")};
  Expr<std::string> buffer_allocator{Expr("malloc")};
  Expr<std::string> buffer_deallocator{Expr("free")};
  bool input{false};
  bool output{false};
  bool allocates{false};
  bool deallocates{false};
  Expr<std::string> transfer{Expr("NW_OPAQUE")};
  int buffer{0};
  // type_cast=None,
  bool unsupported{false};
  bool userdata{false} Expr<std::string> callback_stub_function{Expr("NULL")};
  bool generate_timing_code{false};
  Expr<std::string> lifetime{Expr("AVA_CALL")};
  Expr<std::string> lifetime_coupled{Expr("NULL")};
  bool disable_native{false};
};

struct Conditional {
  std::any predicate;
  std::any then_branch;
  std::any else_branch;
}

}  // namespace cava
}  // namespace ava
