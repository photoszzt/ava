#include <clang/AST/ASTConsumer.h>
#include <clang/AST/DeclVisitor.h>
#include <clang/AST/RecordLayout.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/StmtVisitor.h>
#include <clang/AST/TypeVisitor.h>
#include <clang/Basic/Builtins.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/Path.h>

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/Version.h"

#ifndef LLVM_DEBUG
#define LLVM_DEBUG DEBUG
#endif

class TranslateASTVisitor final : public clang::RecursiveASTVisitor<TranslateASTVisitor> {
 public:
  bool TraverseDecl(clang::Decl *D) { return true; }

  bool TraverseStmt(clang::Stmt *x) { return true; }

  bool TraverseType(clang::QualType x) { return true; }
};

class TranslateConsumer : public clang::ASTConsumer {
 public:
  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

 private:
  TranslateASTVisitor Visitor;
};

class TranslateAction : public clang::ASTFrontendAction {
 public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler,
                                                                llvm::StringRef InFile) {
    return std::make_unique<TranslateConsumer>();
  }
};

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// Added in C++ 17
template <class _Tp, size_t _Sz>
constexpr size_t size(const _Tp (&)[_Sz]) noexcept {
  return _Sz;
}

// We augment the command line arguments to ensure that comments are always
// parsed and string literals are always treated as constant.
static std::vector<const char *> augment_argv(int argc, const char *argv[]) {
  const char *const extras[] = {
      "-extra-arg=-fparse-all-comments",  // always parse comments
      "-extra-arg=-Wwrite-strings",       // string literals are constant
      "-extra-arg=-D_FORTIFY_SOURCE=0",   // we don't want to use checked
                                          // versions of libc. without this we
                                          // get calls to __builtin__memcpy_chk,
                                          // etc.
  };

  // // Build a -resource-dir argument based on the path to the linked clang
  // // installation. Without this, ClangTool builds the resource directory from
  // // the path to the tool (in this case, the binary running the AST Exporter).
  // llvm::SmallString<128> P("-extra-arg=-resource-dir=" CLANG_BIN_PATH);
  // llvm::sys::path::append(P, "..", llvm::Twine("lib") + CLANG_LIBDIR_SUFFIX,
  //                         "clang", CLANG_VERSION_STRING);
  // std::string resource_dir = P.str().str();
  // char *resource_dir_cstr = new char[resource_dir.length() + 1];
  // strncpy(resource_dir_cstr, resource_dir.c_str(), resource_dir.length() + 1);

  auto argv_ = std::vector<const char *>();
  // argv_.reserve(argc + size(extras) + 2);
  argv_.reserve(argc + size(extras) + 1);

  auto pusher = std::back_inserter(argv_);
  std::copy_n(argv, argc, pusher);
  std::copy_n(extras, size(extras), pusher);
  // *pusher++ = resource_dir_cstr;
  *pusher++ = nullptr;  // The value of argv[argc] is guaranteed to be a null pointer.

  return argv_;
}

namespace ava {
namespace cava {
void process(int argc, const char *argv[], int *result) {
  static uint64_t source_path_count = 0;
  auto argv_ = augment_argv(argc, argv);
  int argc_ = argv_.size() - 1;  // ignore the extra nullptr
  clang::tooling::CommonOptionsParser OptionsParser(argc_, argv_.data(), MyToolCategory);

  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

  *result = Tool.run(clang::tooling::newFrontendActionFactory<TranslateAction>().get());
}
}  // namespace cava
}  // namespace ava
