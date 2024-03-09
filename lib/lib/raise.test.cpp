#include <lib/exception.hpp>
#include <lib/raise.hpp>
#include <lib/source_location.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
using ::xzr::error::raise;

struct test_thrw
{
    using StdEx = std::runtime_error;
    using Ex = ::xzr::error::exception;
    using ExInt = ::xzr::error::basic_exception<int>;

    test_thrw()
    {
    }

    ~test_thrw()
    {
    }
};

BOOST_FIXTURE_TEST_SUITE(thrw_tests, test_thrw)

BOOST_AUTO_TEST_CASE(throw_std_e)
{
    try
    {
        raise<StdEx>() << "msg " << 2;
    }
    catch (const StdEx& e)
    {
        BOOST_TEST(e.what() == "msg 2");
    }
}

BOOST_AUTO_TEST_CASE(throw_exception_with_location)
{
    try
    {
        raise<Ex>() << "msg " << 2;
    }
    catch (const Ex& e)
    {
        BOOST_TEST(e.where().line() == __LINE__ - 4);
        BOOST_TEST(e.where().file_name() == __FILE__);
        BOOST_TEST(e.where().function_name() == __func__);
        BOOST_TEST(e.what() == "msg 2");
        BOOST_TEST(e.str() == "msg 2");
    }
}

BOOST_AUTO_TEST_CASE(throw_exception_with_location_and_data)
{
    try
    {
        raise<ExInt>(1) << "msg " << 2;
    }
    catch (const ExInt& e)
    {
        BOOST_TEST(e.where().line() == __LINE__ - 4);
        BOOST_TEST(e.where().file_name() == __FILE__);
        BOOST_TEST(e.where().function_name() == __func__);
        BOOST_TEST(e.what() == "msg 2");
        BOOST_TEST(e.str() == "msg 2");
        BOOST_TEST(e.data() == 1);
    }
}

BOOST_AUTO_TEST_SUITE_END()
}
