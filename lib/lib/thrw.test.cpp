#include <lib/source_location.hpp>
#include <lib/thrw.hpp>

#include <boost/test/unit_test.hpp>

#include <stdexcept>

namespace
{
struct test_thrw
{
    using sl = ::xzr::ext::source_location;

    test_thrw()
    {
    }

    ~test_thrw()
    {
    }

    class Exception : public std::runtime_error
    {
      public:
        using Base = std::runtime_error;

        Exception(const char* msg, sl srcLoc)
            : Base{msg}
            , m_srcLoc{srcLoc}
        {
        }

        sl m_srcLoc{};
    };
};

BOOST_FIXTURE_TEST_SUITE(thrw_tests, test_thrw)

BOOST_AUTO_TEST_CASE(throw_catch)
{
    using E = Exception;
    using Thrw = ::xzr::error::thrw<E>;

    try
    {
        Thrw{} << "msg";
    }
    catch (const E& e)
    {
        BOOST_TEST(e.what() == "msg");
        BOOST_TEST(e.m_srcLoc.line() == 46);
        BOOST_TEST(e.m_srcLoc.file_name() == __FILE__);
    }
}

BOOST_AUTO_TEST_SUITE_END()
}
