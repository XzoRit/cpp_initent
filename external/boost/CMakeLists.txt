find_package(Boost REQUIRED COMPONENTS program_options unit_test_framework)

if(Boost_FOUND)
  set_target_properties(
    Boost::boost Boost::program_options Boost::unit_test_framework
    PROPERTIES IMPORTED_GLOBAL true
  )
endif()
