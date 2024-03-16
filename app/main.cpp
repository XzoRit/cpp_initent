#include <lib/exception.hpp>
#include <lib/intent.hpp>
#include <lib/raise.hpp>
#include <lib/source_location.hpp>

#include <boost/program_options.hpp>

#include <format>
#include <iostream>
#include <iterator>
#include <random>

namespace po = boost::program_options;

void flush_intention_stack()
{
    for (const auto& m : ::xzr::error::msgs())
    {
        std::format_to(std::ostreambuf_iterator<char>{std::cout},
                       "{0:F}:{0:L}:{0:C}:{0:f}:{1}\n",
                       m.location(),
                       m.msg());
    }
    ::xzr::error::msgs().clear();
}

int rand()
{
    static auto rd{std::random_device{}};
    static auto mt{std::mt19937{rd()}};
    static auto dis{std::uniform_int_distribution<>{-2, 10}};

    return dis(mt);
}

void odd(int a)
{
    const auto _{::xzr::error::intent().capture("check if ", a, " is odd")};
    if (a % 2)
        xzr::error::raise<std::runtime_error>()
            << "odd (" << a << ") not allowed";
}

void negativ(int a)
{
    const auto _{::xzr::error::intent().capture("check if ", a, " is negativ")};
    if (a < 0)
        xzr::error::raise<std::runtime_error>()
            << "negativ (" << a << ") not allowed";
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
            std::cout << e.what() << '\n';
            flush_intention_stack();
        }
    }
}

int main(int ac, char* av[])
{
    std::cout << "Hello app\n";

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()("help", "produce help message");

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return 0;
        }
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
