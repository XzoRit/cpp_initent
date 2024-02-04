// #include <lib/source_location.hpp>
// #include <lib/thrw.hpp>

// #include <boost/test/unit_test.hpp>

// #include <future>
// #include <numeric>
// #include <string>
// #include <vector>

// using namespace std::string_literals;

// namespace
// {
// using ::xzr::ext::source_location;
// template <class E>
// using thrw = ::xzr::error::thrw<E>;

// template <class BaseException>
// class Exception : public BaseException
// {
//   public:
//     using BaseException::BaseException;

//     explicit Exception(const char* msg,
//                        source_location srcLoc = source_location::current())
//         : BaseException{msg}
//         , m_srcLoc{srcLoc}
//     {
//     }

//     source_location sourceLocation() const noexcept
//     {
//         return m_srcLoc;
//     }

//   private:
//     source_location m_srcLoc{source_location::current()};
// };

// struct message
// {
//     message(std::string s, source_location sl = source_location::current())
//         : loc{sl}
//         , m{std::move(s)}
//     {
//     }

//     [[nodiscard]] const std::string& msg() const
//     {
//         return m;
//     }

//     [[nodiscard]] source_location location() const
//     {
//         return loc;
//     }

//     std::string m{};
//     source_location loc{source_location::current()};
// };

// using messages = std::vector<message>;

// messages& msgs()
// {
//     thread_local static messages ms{};
//     return ms;
// }

// template <class Stream, class... A>
// Stream& stream_args_into(Stream& str, const A&... args)
// {
//     (str << ... << args);
//     return str;
// }

// template <class... A>
// [[nodiscard]] std::string msg_from_ostringstream(const A&... args)
// {
//     std::ostringstream str{};
//     return stream_args_into(str, args...).str();
// }

// template <class... Args>
// struct [[nodiscard]] intent
// {
//     explicit intent(source_location sl, Args... args)
//         : tup{std::move(args)...}
//         , loc{sl}
//     {
//     }

//     explicit intent(Args... args)
//         : tup{std::move(args)...}
//     {
//     }

//     ~intent()
//     {
//         if (std::uncaught_exceptions() <= except_count)
//             return;
//         try
//         {
//             msgs().push_back(std::apply(
//                 [this](const auto&... args) {
//                     return message{msg_from_ostringstream(args...), loc};
//                 },
//                 tup));
//         }
//         catch (...)
//         {
//         }
//     }

//     std::tuple<Args...> tup;
//     int except_count{std::uncaught_exceptions()};
//     source_location loc{source_location::current()};
// };

// using Error = std::runtime_error;

// int a(int i)
// {
//     const auto& in{intent{source_location::current(), "a(", i, ')'}};
//     if (i % 2 != 0)
//         throw Error{"odd number not allowed"};
//     return i;
// }

// std::pair<int, std::string> a_range(int min, int max)
// {
//     auto accu{int{}};
//     auto msg{""s};
//     try
//     {
//         const auto& i{intent{source_location::current(),
//                              "a_range(",
//                              min,
//                              ", ",
//                              max,
//                              ")"}};
//         for (int idx{min}; idx < max; ++idx)
//         {
//             const auto& ii{intent{source_location::current(),
//                                   "accu:",
//                                   accu,
//                                   " idx:",
//                                   idx}};
//             accu += a(idx);
//         }
//     }
//     catch (...)
//     {
//         msg += std::accumulate(msgs().cbegin(),
//                                msgs().cend(),
//                                ""s,
//                                [](auto accu, const auto& m) {
//                                    accu += m.msg() + '\n';
//                                    return accu;
//                                });
//         msgs().clear();
//     }
//     return {accu, msg};
// }

// struct cm_spy
// {
//     explicit cm_spy(std::string* s)
//         : calls{s}
//     {
//     }

//     cm_spy(const cm_spy& a)
//         : calls{a.calls}
//     {
//         *calls += "cc";
//     }

//     cm_spy(cm_spy&& a)
//         : calls{a.calls}
//     {
//         *calls += "mc";
//     }

//     cm_spy& operator=(const cm_spy& a)
//     {
//         calls = a.calls;
//         *calls += "ca";
//         return *this;
//     }

//     cm_spy& operator=(cm_spy&& a)
//     {
//         calls = a.calls;
//         *calls += "ma";
//         return *this;
//     }

//     std::string* calls;
// };

// std::ostream& operator<<(std::ostream& s, const cm_spy&)
// {
//     s << "cm_spy";
//     return s;
// }

// struct test_intent
// {
//     test_intent()
//     {
//         msgs().clear();
//     }

//     ~test_intent()
//     {
//         msgs().clear();
//     }
// };

// BOOST_FIXTURE_TEST_SUITE(intent_tests, test_intent)

// BOOST_AUTO_TEST_CASE(no_intent_on_success)
// {
//     const auto& i{intent{source_location::current(), "start", 0}};
//     a(0);
//     BOOST_TEST(msgs().empty());
// }

// BOOST_AUTO_TEST_CASE(intent_on_failure)
// {
//     const auto loc{source_location::current()};
//     try
//     {
//         const auto& i{intent{loc, "test a(", -1, ')'}};
//         a(-1);
//     }
//     catch (...)
//     {
//     }
//     BOOST_REQUIRE(!msgs().empty());
//     BOOST_REQUIRE(msgs().size() == 2);
//     BOOST_TEST(msgs()[0].msg() == "a(-1)");
//     BOOST_TEST(msgs()[1].msg() == "test a(-1)");
//     BOOST_TEST(msgs()[1].location().file_name() == loc.file_name());
//     BOOST_TEST(msgs()[1].location().line() == loc.line());
//     BOOST_TEST(msgs()[1].location().column() == loc.column());
//     BOOST_TEST(msgs()[1].location().function_name() == loc.function_name());
// }

// BOOST_AUTO_TEST_CASE(intent_copies_lvalue)
// {
//     auto calls{""s};
//     auto spy{cm_spy{&calls}};

//     try
//     {
//         const auto i{intent{source_location::current(), spy}};
//         throw 42;
//     }
//     catch (...)
//     {
//     }

//     BOOST_TEST(calls == "ccmc");
//     BOOST_TEST(!msgs().empty());
// }

// BOOST_AUTO_TEST_CASE(intent_moves_rvalue)
// {
//     auto calls{""s};

//     try
//     {
//         const auto i{intent{source_location::current(), cm_spy{&calls}}};
//         throw 42;
//     }
//     catch (...)
//     {
//     }

//     BOOST_TEST(calls == "mc");
//     BOOST_TEST(!msgs().empty());
// }

// BOOST_AUTO_TEST_CASE(intent_is_snapshot_by_default)
// {
//     try
//     {
//         auto a{0};
//         const auto i{intent{source_location::current(), a}};
//         ++a;
//         throw 42;
//     }
//     catch (...)
//     {
//     }

//     BOOST_REQUIRE(msgs().size() == 1);
//     BOOST_TEST(msgs()[0].msg() == "0");
// }

// BOOST_AUTO_TEST_CASE(intent_can_take_ref)
// {
//     try
//     {
//         auto a{0};
//         const auto i{intent{source_location::current(), std::cref(a)}};
//         ++a;
//         throw 42;
//     }
//     catch (...)
//     {
//     }

//     BOOST_REQUIRE(msgs().size() == 1);
//     BOOST_TEST(msgs()[0].msg() == "1");
// }

// BOOST_AUTO_TEST_CASE(intent_from_a_range)
// {
//     {
//         const auto& a{a_range(0, 1)};
//         BOOST_TEST(a.second == "");
//     }
//     {
//         const auto& a{a_range(-1, 0)};
//         BOOST_TEST(a.second == "a(-1)\n"
//                                "accu:0 idx:-1\n"
//                                "a_range(-1, 0)\n");
//     }
// }

// BOOST_AUTO_TEST_CASE(intent_from_multiple_threads)
// {
//     auto results{std::vector<std::future<std::pair<int, std::string>>>{}};

//     for (auto i{int{-50}}; i < 50; i += 10)
//     {
//         results.push_back(std::async(a_range, i, i + 10));
//     }

//     for (auto& f : results)
//     {
//         f.wait();
//     }

//     auto msg{""s};
//     for (auto& f : results)
//     {
//         msg += f.get().second;
//     }

//     BOOST_TEST(!msg.empty());
// }

// BOOST_AUTO_TEST_SUITE_END()
// }
