#pragma once
#include <cstdint>
#include <optional>
#include <string_view>

constexpr bool ASCRIBE_TYPES = false;
constexpr std::string_view _annotation_prefix = "ava_";
constexpr std::string_view NIGHTWATCH_PREFIX = "__NIGHTWATCH_";
constexpr std::string_view buffer_index_spelling = "ava_index";
constexpr std::string_view RET_ARGUMENT_NAME = "ret";

namespace ava {
namespace cava {

class Location {
 private:
  std::string_view file_name;
  uint32_t line;
  uint32_t column;
  uint32_t offset;
};

class Type {
 public:
  Type(std::string_view spelling);
  virtual ~Type();
  bool is_void();
  static std::string drop_const(std::string_view s);
  Type nonconst();
  std::string annotations();
  std::string attach_to(std::string_view name, std::string_view additional_inner_type_elements);
  std::string spelled_with(std::string_view additional_inner_type_elements);
  std::string ascribe_type(std::string_view v, std::string_view additional_inner_type_elements);
  std::string cast_type(std::string_view v, std::string_view additional_inner_type_elements);

  static absl::flat_hash_map<std::string, std::string> transfer_spellings = {
      {"NW_BUFFER", ""},       {"NW_OPAQUE", "opaque"},     {"NW_FILE", "file"},
      {"NW_HANDLE", "handle"}, {"NW_CALLBACK", "callback"}, {"NW_CALLBACK_REGISTRATION", "callback_registration"},
  };
  static absl::flat_hash_map<std::string, std::string> lifetime_spellings = {
      {"AVA_CALL", ""},
      {"AVA_COUPLED", ""},
      {"AVA_STATIC", "static"},
      {"AVA_MANUAL", "manual"},
  };
  static absl::flat_hash_set<std::string> hidden_annotations = {
      {"location"}, {"spelling"}, {"buffer_deallocator"}, {"callback_stub_function"}};

 protected:
  std::optional<std::variant<Expr, std::string>> success;
  std::optional<std::variant<Expr, std::string>> transfer;
  std::string spelling;
  std::optional<Type> pointee;
  absl::flat_hash_map<std::string, Type> fields;
  std::optional<std::variant<Expr, std::string>> buffer;
  std::optional<std::variant<Expr, std::string>> lifetime;
  std::optional<std::variant<Expr, std::string>> lifetime_coupled;
  std::variant<Expr, std::string> buffer_allocator;
  std::variant<Expr, std::string> buffer_deallocator;
};

class ContitionalType : public Type {
 public:
  std::string attach_to(std::string_view name, std::string_view additional_inner_type_elements = "");

 protected:
  std::string predicate;
  Type then_type;
  Type else_type;
  Type original_type;
};

class StaticArray : public Type {
 public:
  std::string attach_to(std::string name, std::string_view additional_inner_type_elements = "");
  std::string ascribe_type(std::string_view v, std::string_view additional_inner_type_elements = "");
};

class FunctionPointer : public Type {
 public:
  std::string attach_to(std::string name, std::string_view additional_inner_type_elements = "");
  std::string ascribe_type(std::string_view v, std::string_view additional_inner_type_elements = "");
};

class Argument {
 public:
  absl::flat_hash_set<Type> contained_types();
  friend bool operator<(const Argument &l, const Argument &r);

  Type type;
  static absl::flat_hash_set<std::string> hidden_annotations = {{"ret"}, {"function"}, {"location"}, {"name"}};
  std::string annotations();
  std::string declaration();
  std::string original_decclaration();
};

class Function {
 public:
  std::string name_;
  Argument return_value_;
  std::vector<Argument> _arguments_;
  bool supported_{true};
  bool callback_decl_{false};
  bool ignore_{false};
  bool generate_timing_code_{false};
  bool disable_native_{false};

  static absl::flat_hash_map<std::string, std::string> synchrony_spellings = {
      {"NW_SYNC", "sync"},
      {"NW_ASYNC", "async"},
      {"NW_FLUSH", "flush"},
  };
  static absl::flat_hash_map<std::string> hidden_annotations = {{"api"},          {"location"}, {"name"},
                                                                {"return_value"}, {"epilogue"}, {"prologue"},
                                                                {"arguments"},    {"type"}} protected
      : std::string prologue{""};
  std::string epilogue{""};
  std::string_view name;
  std::string synchrony{"NW_SYNC"};
};

class API {
 public:
  std::string include_lines();
  std::string directory_spelling();

 protected:
};

}  // namespace cava
}  // namespace ava
