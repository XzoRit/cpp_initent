#include <lib/thrw.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
struct test_thrw
{
    test_thrw()
    {
    }

    ~test_thrw()
    {
    }
};

BOOST_FIXTURE_TEST_SUITE(thrw_tests, test_thrw)

BOOST_AUTO_TEST_CASE(throw_catch)
{
    BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()
}
