#include <boost/test/unit_test_log.hpp>
#include <lib/source_location.hpp>

#include <boost/test/unit_test.hpp>

#include <string_view>

namespace
{
struct test_source_location
{
    using source_location = ::xzr::ext::source_location;

    test_source_location()
    {
    }

    ~test_source_location()
    {
    }

    source_location src_loc(source_location sr = source_location::current())
    {
        return sr;
    }

    struct src_loc_ctor
    {
        src_loc_ctor(source_location sl = source_location::current())
            : m_sl{sl}
        {
        }
        source_location m_sl{};
    };

    struct src_loc_member
    {
        source_location m_sl{source_location::current()};
    };
};

BOOST_FIXTURE_TEST_SUITE(source_location_tests, test_source_location)

BOOST_AUTO_TEST_CASE(current)
{
    const auto sl{source_location::current()};

    BOOST_TEST(sl.file_name() == __FILE__);
    BOOST_TEST(sl.line() == __LINE__ - 3);
    BOOST_TEST(sl.column() != 0);
    BOOST_TEST(std::string_view{sl.function_name()}.ends_with("test_method()"));
}

BOOST_AUTO_TEST_CASE(default_func_param)
{
    const auto sl{src_loc()};

    BOOST_TEST(sl.file_name() == __FILE__);
    BOOST_TEST(sl.line() == __LINE__ - 3);
    BOOST_TEST(sl.column() != 0);
    BOOST_TEST(std::string_view{sl.function_name()}.ends_with("test_method()"));
}

BOOST_AUTO_TEST_CASE(default_ctor_param)
{
    const auto sl{src_loc_ctor{}.m_sl};

    BOOST_TEST(sl.file_name() == __FILE__);
    BOOST_TEST(sl.line() == __LINE__ - 3);
    BOOST_TEST(sl.column() != 0);
    BOOST_TEST(std::string_view{sl.function_name()}.ends_with("test_method()"));
}

BOOST_AUTO_TEST_CASE(default_member)
{
    const auto sl{src_loc_member{}.m_sl};

    BOOST_TEST(sl.file_name() == __FILE__);
    BOOST_TEST(sl.line() == __LINE__ - 3);
    BOOST_TEST(sl.column() != 0);
    BOOST_TEST(std::string_view{sl.function_name()}.ends_with("test_method()"));
}

BOOST_AUTO_TEST_SUITE_END()
}
