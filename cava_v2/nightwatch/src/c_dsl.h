#pragma once

#include "common.h"

namespace ava {
namespace cava {

template <class T>
class Expr {
 public:
  Expr(std::string code, absl::optional<absl::flat_hash_set<T>> value_set);
  virtual ~Expr();

  std::string code();
  bool is_true();
  bool is_false();
  std::string constant_value();
  bool is_constant(absl::optional<int> value);

  Expr<T> equals(absl::string_view other);
  Expr<T> one_of(absl::flat_hash_set<std::string> values);
  Expr<T> not_equals(absl::string_view other);
  Expr<T> invert();
  Expr<T> and_op(bool other);
  Expr<T> and_op(Expr &other);
  Expr<T> or_op(bool other);
  Expr<T> or_op(Expr &other);
  Expr<T> gt_op(int other);
  Expr<T> group();
  Expr<T> scope();
  Expr<T> if_then_else_expr(absl::string_view then_branch, absl::string_view else_branch);
};

}  // namespace cava
}  // namespace ava
