add_library(cava_textx STATIC)
add_library(CAvA::textx ALIAS cava_textx)

set(sources
  src/lang.cpp
)

target_sources(cava_textx PRIVATE ${sources})

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/PEGTL
  ${CMAKE_BINARY_DIR}/_deps/PEGTL-build
)

target_include_directories(cava_textx PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/common>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(cava_textx PRIVATE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/fmt/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(cava_textx PRIVATE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/plog/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(cava_textx PRIVATE taocpp::pegtl)
