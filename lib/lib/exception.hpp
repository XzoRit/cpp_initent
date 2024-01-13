#pragma once

#include <lib/source_location.hpp>

#include <string>
#include <utility>

namespace xzr::error
{
template <class Data>
class exception
{
  public:
    explicit exception(
        std::string msg,
        Data d,
        ext::source_location sl = ext::source_location::current())
        : m_msg{std::move(msg)}
        , m_data{std::move(d)}
        , m_sl{sl}
    {
    }

    const std::string& what() const
    {
        return m_msg;
    }

    std::string& what()
    {
        return m_msg;
    }

    const Data& data() const
    {
        return m_data;
    }

    ext::source_location where() const
    {
        return m_sl;
    }

  private:
    std::string m_msg;
    Data m_data;
    ext::source_location m_sl;
};
}
