#include <lib/source_location.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
struct test_source_location
{
    test_source_location()
    {
    }

    ~test_source_location()
    {
    }
};

BOOST_FIXTURE_TEST_SUITE(source_location_tests, test_source_location)

BOOST_AUTO_TEST_CASE(current)
{
    const auto sl{xzr::ext::source_location::current()};

    BOOST_TEST(sl.file_name() == __FILE__);
    BOOST_TEST(sl.line() == __LINE__ - 3);
    BOOST_TEST(sl.column() != 0);
    BOOST_TEST(sl.function_name() = "");
}

BOOST_AUTO_TEST_SUITE_END()
}
