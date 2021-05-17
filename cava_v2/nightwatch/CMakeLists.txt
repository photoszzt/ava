add_executable(nwcc)

set(common_srcs
  ${CMAKE_SOURCE_DIR}/common/status.cpp
  ${CMAKE_SOURCE_DIR}/common/result.cpp
  ${CMAKE_SOURCE_DIR}/common/logging.cpp
)

set(sources
  src/main.cpp
  src/parser/parse.cpp
  src/annotation_set.cpp
  src/c_dsl.cpp
  src/model.cpp
  ${common_srcs}
)
find_package(Clang REQUIRED CONFIG)

include_directories(
  ${CMAKE_SOURCE_DIR}/common
  ${CMAKE_SOURCE_DIR}/third_party/plog/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${LLVM_INCLUDE_DIRS}
)
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

set(absl_flags_libraries
  absl::flags
  absl::flags_commandlineflag
  absl::flags_commandlineflag_internal
  absl::flags_config
  absl::flags_internal
  absl::flags_marshalling
  absl::flags_parse
  absl::flags_private_handle_accessor
  absl::flags_program_name
  absl::flags_reflection
  absl::flags_usage
  absl::flags_usage_internal
)

set(CLANG_LINK_LIBRARIES
  libclang
  clangTooling
  clangARCMigrate
  clangAST
  clangASTMatchers
  clangAnalysis
  clangBasic
  clangDriver
  clangEdit
  clangFrontend
  clangFrontendTool
  clangLex
  clangParse
  clangRewrite
  clangRewriteFrontend
  clangSema
  clangSerialization
)

target_sources(nwcc PUBLIC ${sources})

target_link_libraries(nwcc PRIVATE
  ${absl_flags_libraries}
  fmt
)

function(clang_target_link target type)
if (CLANG_LINK_CLANG_DYLIB)
    target_link_libraries(${target} ${type} clang_shared)
else()
    target_link_libraries(${target} ${type} ${ARGN})
endif()
endfunction()

clang_target_link(nwcc PRIVATE ${CLANG_LINK_LIBRARIES} GSL)
