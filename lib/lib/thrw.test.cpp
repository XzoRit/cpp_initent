#include "lib/source_location.hpp"
#include <lib/exception.hpp>
#include <lib/thrw.hpp>

#include <boost/test/unit_test.hpp>

#include <stdexcept>

namespace
{
struct test_thrw
{
    using E = ::xzr::error::exception<int>;
    using Thrw = ::xzr::error::thrw<E>;

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

    try
    {
        Thrw{"msg", 1} << " - ext msg";
    }
    catch (const E& e)
    {
        BOOST_TEST(e.data() == 1);
    }
}

BOOST_AUTO_TEST_SUITE_END()
}
