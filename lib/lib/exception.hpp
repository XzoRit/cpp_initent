#pragma once

#include <lib/source_location.hpp>

#include <exception>
#include <string>
#include <utility>

namespace xzr::error
{
class exception_str : public std::exception
{
  private:
    using source_location = ::xzr::ext::source_location;

  public:
    explicit exception_str(std::string msg,
                           source_location sl = source_location::current())
        : m_msg{std::move(msg)}
        , m_sl{sl}
    {
    }

    const char* what() const noexcept override
    {
        return m_msg.c_str();
    }

    const std::string& str() const
    {
        return m_msg;
    }

    std::string& str()
    {
        return m_msg;
    }

    source_location where() const
    {
        return m_sl;
    }

  private:
    std::string m_msg;
    source_location m_sl{};
};

template <class Data>
class exception : public exception_str
{
  private:
    using source_location = ::xzr::ext::source_location;

  public:
    explicit exception(std::string msg,
                       Data d,
                       source_location sl = source_location::current())
        : exception_str{std::move(msg), sl}
        , m_data{std::move(d)}
    {
    }

    const Data& data() const
    {
        return m_data;
    }

  private:
    Data m_data;
};
}
