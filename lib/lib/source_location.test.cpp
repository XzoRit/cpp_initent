#include <boost/test/unit_test_log.hpp>
#include <lib/source_location.hpp>

#include <boost/test/unit_test.hpp>

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

        src_loc_ctor(int, source_location sl = source_location::current())
            : m_sl{sl}
        {
        }

        source_location m_sl{};
    };

    struct src_loc_member
    {
        src_loc_member() = default;

        src_loc_member(int)
        {
        }

        source_location m_sl{source_location::current()};
    };
};

BOOST_FIXTURE_TEST_SUITE(source_location_tests, test_source_location)

BOOST_AUTO_TEST_CASE(current)
{
    const auto sl{source_location::current()};

    BOOST_TEST(sl.file_name() == __FILE__);
    BOOST_TEST(sl.line() == __LINE__ - 3);
    BOOST_TEST(sl.function_name() == "test_method");
}

BOOST_AUTO_TEST_CASE(default_func_param)
{
    const auto sl{src_loc()};

    BOOST_TEST(sl.file_name() == __FILE__);
    BOOST_TEST(sl.line() == __LINE__ - 3);
    BOOST_TEST(sl.function_name() == "test_method");
}

BOOST_AUTO_TEST_CASE(default_ctor_param)
{
    {
        const auto sl{src_loc_ctor{}.m_sl};

        BOOST_TEST(sl.file_name() == __FILE__);
        BOOST_TEST(sl.line() == __LINE__ - 3);
        BOOST_TEST(sl.function_name() == "test_method");
    }
    {
        const auto sl{src_loc_ctor{int{}}.m_sl};

        BOOST_TEST(sl.file_name() == __FILE__);
        BOOST_TEST(sl.line() == __LINE__ - 3);
        BOOST_TEST(sl.function_name() == "test_method");
    }
}

BOOST_AUTO_TEST_CASE(default_member)
{
    {
        const auto sl{src_loc_member{}.m_sl};

        BOOST_TEST(sl.file_name() == __FILE__);
        BOOST_TEST(sl.function_name() == "src_loc_member");
    }
    {
        const auto sl{src_loc_member{int{}}.m_sl};

        BOOST_TEST(sl.file_name() == __FILE__);
        BOOST_TEST(sl.function_name() == "src_loc_member");
    }
}

BOOST_AUTO_TEST_CASE(std_format)
{
    const auto sl{source_location::current()};

    BOOST_TEST(std::format("{:F}", sl) == sl.file_name());
    BOOST_TEST(std::format("{:L}", sl) == std::to_string(sl.line()));
    BOOST_TEST(std::format("{:C}", sl) == std::to_string(sl.column()));
    BOOST_TEST(std::format("{:f}", sl) == sl.function_name());

    BOOST_TEST(std::format("{0:F}{0:L}{0:C}{0:f}", sl) ==
               std::string{sl.file_name()} + std::to_string(sl.line()) +
                   std::to_string(sl.column()) + sl.function_name());
}

BOOST_AUTO_TEST_SUITE_END()
}
