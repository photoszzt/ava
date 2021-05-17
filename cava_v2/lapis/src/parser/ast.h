#pragma once
#include <absl/container/flat_hash_map.h>

#include <any>
#include <iostream>

#include "common/common.h"

namespace ava {
namespace cava {
class Declaration {
 public:
  virtual uint32_t descriptor_count() = 0;
};

class DescriptorSequence {
 public:
  operator bool();
  friend std::ostream &operator<<(std::ostream &os, const DescriptorSequence &ds);
  uint32_t descriptor_count();

 private:
  absl::flat_hash_map<std::any> children;
};

class ConditionalDescriptor {
 public:
  friend std::ostream &operator<<(std::ostream &os, const ConditionalDescriptor &cd);
};

class Descriptor : public Declaration {
 public:
  std::vector<Descriptor> subdescriptors();
  uint32_t descriptor_count() override;
  friend std::ostream &operator<<(std::ostream &os, const Descriptor &d);
};

class Value {
 public:
  virtual void eval() = 0;
};

class Id : public Value {
 public:
  void eval() override;
  friend std::ostream &operator<<(std::ostream &os, const Descriptor &d);
};

class String : public Value {
 public:
  void eval() override;
  friend std::ostream &operator<<(std::ostream &os, const Descriptor &d);
};

class Number : public Value {
 public:
  void eval() override;
  friend std::ostream &operator<<(std::ostream &os, const Descriptor &d);
};

class Bool : public Value {
 public:
  void eval() override;
  friend std::ostream &operator<<(std::ostream &os, const Descriptor &d);
};

class QuotedCodeSegmentInterpolate : public Value {
 public:
  void eval() override;
  friend std::ostream &operator<<(std::ostream &os, const Descriptor &d);
};

class Code : public Value {
 public:
  void eval() override;
  friend std::ostream &operator<<(std::ostream &os, const Descriptor &d);
};

class Matcher {};

class MatchDescriptor : public Matcher {
 public:
  std::vector<MatchDescriptor> subdescriptors();
  uint32_t descriptor_count();
  friend std::ostream &operator<<(std::ostream &os, const MatchDescriptor &d);
};

class MatcherBlock : public Matcher {
 public:
  uint32_t descriptor_count();
  friend std::ostream &operator<<(std::ostream &os, const MatchBlock &d);
};

class MatcherBind : public Matcher {
 public:
  friend std::ostream &operator<<(std::ostream &os, const MatcherBind &d);
};

class MatcherString : public Matcher {
 public:
  friend std::ostream &operator<<(std::ostream &os, const MatcherString &d);
};

class MatcherValue : public Matcher {
 public:
  friend std::ostream &operator<<(std::ostream &os, const MatcherValue &d);
};

class MatcherPredicate : public Matcher {
 public:
  friend std::ostream &operator<<(std::ostream &os, const MatcherPredicate &d);
};

class MatcherAny : public Matcher {
 public:
  friend std::ostream &operator<<(std::ostream &os, const MatcherAny &d);
};

class Rule : public Declaration {
 public:
  uint32_t descriptor_count() override;
  std::vector result_descriptors();
  friend std::ostream &operator<<(std::ostream &os, const Rule &d);
};

class Specification {
  friend std::ostream &operator<<(std::ostream &os, const Specification &d);
};

}  // namespace cava
}  // namespace ava
