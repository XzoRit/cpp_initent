add_executable(app_exe)

target_sources(app_exe PRIVATE "main.cpp")

target_link_libraries(app_exe PRIVATE lib::lib Boost::program_options)

add_test(NAME app::exe COMMAND app_exe "--help")

add_executable(bench_exe)

target_sources(bench_exe PRIVATE "bench.cpp")

target_link_libraries(bench_exe PRIVATE lib::lib)

add_test(NAME bench::exe COMMAND bench_exe)
