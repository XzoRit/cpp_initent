#include <lib/exception.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
struct test_exception
{
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
}

BOOST_AUTO_TEST_SUITE_END()
}
