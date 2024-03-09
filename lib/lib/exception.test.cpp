#include <lib/exception.hpp>
#include <lib/source_location.hpp>

#include <boost/test/unit_test.hpp>

#include <exception>
#include <type_traits>

namespace
{
struct test_exception
{
    using E = ::xzr::error::basic_exception<int>;

    test_exception()
    {
    }

    ~test_exception()
    {
    }
};

BOOST_FIXTURE_TEST_SUITE(exception_tests, test_exception)

BOOST_AUTO_TEST_CASE(ctor)
{
    const auto e{E{"msg", 1}};

    BOOST_TEST(e.where().line() == __LINE__ - 2);
    BOOST_TEST(e.where().file_name() == __FILE__);
    BOOST_TEST(e.where().function_name() == __func__);
    BOOST_TEST(e.what() == "msg");
    BOOST_TEST(e.str() == "msg");
}

BOOST_AUTO_TEST_CASE(inherits_from_std_exception)
{
    static_assert(std::is_base_of_v<std::exception, E>);
}

BOOST_AUTO_TEST_SUITE_END()
}
