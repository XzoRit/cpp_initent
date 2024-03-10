set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE ${INCLUDE}/lib)

add_library(lib)

target_sources(
  lib
  PRIVATE
  "${SOURCE}/exception.cpp"
  "${SOURCE}/exception.hpp"
  "${SOURCE}/intent.cpp"
  "${SOURCE}/intent.hpp"
  "${SOURCE}/source_location.cpp"
  "${SOURCE}/source_location.hpp"
  "${SOURCE}/raise.cpp"
  "${SOURCE}/raise.hpp"
)

target_include_directories(lib PUBLIC ${INCLUDE})

add_library(lib::lib ALIAS lib)

find_package(Threads REQUIRED)

add_executable(lib_test)

target_sources(
  lib_test
  PRIVATE
  "${SOURCE}/exception.test.cpp"
  "${SOURCE}/intent.test.cpp"
  "${SOURCE}/source_location.test.cpp"
  "${SOURCE}/raise.test.cpp"
  "${SOURCE}/main.test.cpp"
)

target_link_libraries(lib_test PRIVATE lib::lib Boost::unit_test_framework Threads::Threads)

add_test(NAME lib::test COMMAND lib_test "--color_output=false")
