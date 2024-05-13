#include <lib/intent.hpp>

#include <chrono>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

//#define WITH_INTENT

#if defined(WITH_INTENT)
#define INTENT                                                                 \
    const auto i{::xzr::error::intent_f().on_fail_msg(                         \
        [=]() { return std::format("{} {}", a, b); })};
#else
#define INTENT
#endif
std::string a99(int a, int b)
{
    INTENT
    ++a, ++b;
    if (const auto s{std::format("{} {}", __LINE__ + a, __LINE__ + b)};
        s.empty())
    {
        return s;
    }
    else
    {
        throw std::runtime_error{"s"};
    }
}

std::string a98(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a99(a, b));
}

std::string a97(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a98(a, b));
}

std::string a96(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a97(a, b));
}

std::string a95(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a96(a, b));
}

std::string a94(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a95(a, b));
}

std::string a93(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a94(a, b));
}

std::string a92(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a93(a, b));
}

std::string a91(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a92(a, b));
}

std::string a90(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a91(a, b));
}

std::string a89(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a90(a, b));
}

std::string a88(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a89(a, b));
}

std::string a87(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a88(a, b));
}

std::string a86(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a87(a, b));
}

std::string a85(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a86(a, b));
}

std::string a84(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a85(a, b));
}

std::string a83(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a84(a, b));
}

std::string a82(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a83(a, b));
}

std::string a81(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a82(a, b));
}

std::string a80(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a81(a, b));
}

std::string a79(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a80(a, b));
}

std::string a78(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a79(a, b));
}

std::string a77(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a78(a, b));
}

std::string a76(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a77(a, b));
}

std::string a75(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a76(a, b));
}

std::string a74(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a75(a, b));
}

std::string a73(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a74(a, b));
}

std::string a72(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a73(a, b));
}

std::string a71(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a72(a, b));
}

std::string a70(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a71(a, b));
}

std::string a69(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a70(a, b));
}

std::string a68(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a69(a, b));
}

std::string a67(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a68(a, b));
}

std::string a66(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a67(a, b));
}

std::string a65(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a66(a, b));
}

std::string a64(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a65(a, b));
}

std::string a63(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a64(a, b));
}

std::string a62(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a63(a, b));
}

std::string a61(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a62(a, b));
}

std::string a60(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a61(a, b));
}

std::string a59(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a60(a, b));
}

std::string a58(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a59(a, b));
}

std::string a57(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a58(a, b));
}

std::string a56(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a57(a, b));
}

std::string a55(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a56(a, b));
}

std::string a54(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a55(a, b));
}

std::string a53(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a54(a, b));
}

std::string a52(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a53(a, b));
}

std::string a51(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a52(a, b));
}

std::string a50(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a51(a, b));
}

std::string a49(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a50(a, b));
}

std::string a48(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a49(a, b));
}

std::string a47(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a48(a, b));
}

std::string a46(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a47(a, b));
}

std::string a45(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a46(a, b));
}

std::string a44(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a45(a, b));
}

std::string a43(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a44(a, b));
}

std::string a42(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a43(a, b));
}

std::string a41(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a42(a, b));
}

std::string a40(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a41(a, b));
}

std::string a39(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a40(a, b));
}

std::string a38(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a39(a, b));
}

std::string a37(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a38(a, b));
}

std::string a36(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a37(a, b));
}

std::string a35(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a36(a, b));
}

std::string a34(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a35(a, b));
}

std::string a33(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a34(a, b));
}

std::string a32(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a33(a, b));
}

std::string a31(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a32(a, b));
}

std::string a30(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a31(a, b));
}

std::string a29(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a30(a, b));
}

std::string a28(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a29(a, b));
}

std::string a27(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a28(a, b));
}

std::string a26(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a27(a, b));
}

std::string a25(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a26(a, b));
}

std::string a24(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a25(a, b));
}

std::string a23(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a24(a, b));
}

std::string a22(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a23(a, b));
}

std::string a21(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a22(a, b));
}

std::string a20(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a21(a, b));
}

std::string a19(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a20(a, b));
}

std::string a18(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a19(a, b));
}

std::string a17(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a18(a, b));
}

std::string a16(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a17(a, b));
}

std::string a15(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a16(a, b));
}

std::string a14(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a15(a, b));
}

std::string a13(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a14(a, b));
}

std::string a12(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a13(a, b));
}

std::string a11(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a12(a, b));
}

std::string a10(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a11(a, b));
}

std::string a9(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a10(a, b));
}

std::string a8(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a9(a, b));
}

std::string a7(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a8(a, b));
}

std::string a6(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a7(a, b));
}

std::string a5(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a6(a, b));
}

std::string a4(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a5(a, b));
}

std::string a3(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a4(a, b));
}

std::string a2(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a3(a, b));
}

std::string a1(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a2(a, b));
}

std::string a0(int a, int b)
{
    INTENT
    ++a, ++b;
    return std::format("{} {} {}", __LINE__ + a, __LINE__ + b, a1(a, b));
}

void flush_intention_stack()
{
    ::xzr::error::eager_fmt::stream_intention_stack_into(std::cout);
}

int main()
{
    const auto start{std::chrono::steady_clock::now()};
    try
    {
        std::string s{};
        for (auto i{int{}}; i < 100; ++i)
            s += a0(i, i + 1);
    }
    catch (const std::exception& e)
    {
        std::cout << std::format("{}\n", e.what());
        flush_intention_stack();
        const auto end{std::chrono::steady_clock::now()};
        std::cout << std::format("\n{}\n", end - start);
    }
}
