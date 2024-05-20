#include <lib/exception.hpp>
#include <lib/intent.hpp>
#include <lib/raise.hpp>
#include <lib/source_location.hpp>

#include <iostream>
#include <random>

int rand()
{
    static auto rd{std::random_device{}};
    static auto mt{std::mt19937{rd()}};
    static auto dis{std::uniform_int_distribution<>{-2, 10}};

    return dis(mt);
}

void odd(int a)
{
    const auto _{
        ::xzr::error::eager_fmt::intent().capture("check if parameter is odd",
                                                  a)};
    if (a % 2)
    {
        const auto _{::xzr::error::eager_fmt::intent().capture(
            "throwing since parameter is odd",
            a)};
        ::xzr::error::raise<std::runtime_error>()
            << "odd (" << a << ") not allowed";
    }
}

void negativ(int a)
{
    const auto _{::xzr::error::eager_fmt::intent().capture(
        "check if parameter is negative",
        a)};
    if (a < 0)
    {
        const auto _{::xzr::error::eager_fmt::intent().capture(
            "throwing since parameter is negative",
            a)};
        ::xzr::error::raise<std::runtime_error>()
            << "negativ (" << a << ") not allowed";
    }
    odd(a);
}

void example()
{
    for (auto i{0}; i < 10; ++i)
    {
        try
        {
            const auto num{rand()};
            negativ(num);
        }
        catch (const std::exception& e)
        {
            std::cout << "\n\n";
            std::cout << "exception: " << e.what() << '\n';
            std::cout << "intentions:\n";
            ::xzr::error::eager_fmt::dump_and_clear_intention_stack(std::cout);
            std::cout << "\n\n";
        }
    }
}

int main(int ac, char* av[])
{
    std::cout << "Hello app\n";

    try
    {
        example();
    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Exception of unknown type!\n";
    }

    return 0;
}
