#include <lib/exception.hpp>
#include <lib/raise.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
using ::xzr::error::raise;

struct test_thrw
{
    using E = ::xzr::error::exception<int>;

    test_thrw()
    {
    }

    ~test_thrw()
    {
    }
};

BOOST_FIXTURE_TEST_SUITE(thrw_tests, test_thrw)

BOOST_AUTO_TEST_CASE(throw_catch_with_data)
{

    try
    {
        raise<E>(std::make_tuple("msg", 1)) << " extra msg " << 2;
    }
    catch (const E& e)
    {
        BOOST_TEST(e.where().line() == __LINE__ - 4);
        BOOST_TEST(e.where().file_name() == __FILE__);
        BOOST_TEST(e.where().function_name() == __func__);
        BOOST_TEST(e.what() == "msg extra msg 2");
        BOOST_TEST(e.str() == "msg extra msg 2");
        BOOST_TEST(e.data() == 1);
    }
}

BOOST_AUTO_TEST_SUITE_END()
}
