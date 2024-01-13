#include <lib/exception.hpp>
#include <lib/source_location.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
struct test_exception
{
    using E = ::xzr::error::exception<int>;

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

    BOOST_TEST(e.what() == "msg");
    BOOST_TEST(e.where().line() == __LINE__ - 3);
    BOOST_TEST(e.data() == 1);
}

BOOST_AUTO_TEST_CASE(mut_msg)
{
    auto e{E{"msg", 1}};
    e.what() += " mut";

    BOOST_TEST(e.what() == "msg mut");
}

BOOST_AUTO_TEST_SUITE_END()
}
