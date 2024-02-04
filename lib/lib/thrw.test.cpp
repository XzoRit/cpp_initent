#include <lib/source_location.hpp>
#include <lib/thrw.hpp>

#include <boost/test/unit_test.hpp>

#include <stdexcept>

namespace
{
using ::xzr::error::throw_msg;

struct test_thrw
{
    using source_location = ::xzr::ext::source_location;
    struct E : public std::runtime_error
    {
        E(const char* msg, source_location sl = source_location::current())
            : std::runtime_error{msg}
            , m_sl{sl}
        {
        }
        source_location where() const
        {
            return m_sl;
        }
        const char* m_msg;
        source_location m_sl;
    };

    test_thrw()
    {
    }

    ~test_thrw()
    {
    }
};

BOOST_FIXTURE_TEST_SUITE(thrw_tests, test_thrw)

BOOST_AUTO_TEST_CASE(throw_simple_msg)
{
    try
    {
        throw_msg<std::runtime_error>("msg");
    }
    catch (const std::runtime_error& e)
    {
        BOOST_TEST(e.what() == "msg");
    }
}

BOOST_AUTO_TEST_CASE(throw_msg_with_streaming)
{
    try
    {
        throw_msg<std::runtime_error>("msg: ") << 1 << " msg: " << 2;
    }
    catch (const std::runtime_error& e)
    {
        BOOST_TEST(e.what() == "msg: 1 msg: 2");
    }
}

BOOST_AUTO_TEST_CASE(throw_msg_with_location)
{
    try
    {
        throw_msg<E>("msg: ") << 1 << " msg: " << 2;
    }
    catch (const E& e)
    {
        BOOST_TEST(e.what() == "msg: 1 msg: 2");
        BOOST_TEST(e.where().line() == __LINE__ - 5);
        BOOST_TEST(e.where().file_name() == __FILE__);
        BOOST_TEST(e.where().function_name() == __func__);
    }
}

// BOOST_AUTO_TEST_CASE(throw_msg_with_location_and_data)
// {
//     try
//     {
//         throw_it<E>()("msg", 1) << " extra msg " << 2;
//     }
//     catch (const E& e)
//     {
//         BOOST_TEST(e.where().line() == __LINE__ - 4);
//         BOOST_TEST(e.where().file_name() == __FILE__);
//         BOOST_TEST(e.where().function_name() == __func__);
//         BOOST_TEST(e.what() == "msg extra msg 2");
//         BOOST_TEST(e.str() == "msg extra msg 2");
//         BOOST_TEST(e.data() == 1);
//     }
// }

BOOST_AUTO_TEST_SUITE_END()
}
