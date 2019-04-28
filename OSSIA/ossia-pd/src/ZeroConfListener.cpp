#include "ZeroConfListener.h"
#include <asio/io_service.hpp>
#include <asio/ip/resolver_service.hpp>
#include <asio/ip/tcp.hpp>

#include <ossia/network/oscquery/oscquery_mirror.hpp>

#include "ossia-pd.hpp"

namespace ossia
{
namespace pd
{
std::vector<std::shared_ptr<ossia::net::generic_device>> ZeroConfListener::m_devices;
std::vector<std::vector<std::shared_ptr<ossia::net::generic_device>>::iterator>  ZeroConfListener::m_zombie_devices;

// TODO add support for Minuit discovery
  ZeroConfListener::ZeroConfListener()
    : service {"_oscjson._tcp"}
  {
    for (const auto& i : service.getInstances())
      instanceAdded(i);

    service.addListener(this);
    service.beginBrowsing(servus::Interface::IF_ALL);
  }

  ZeroConfListener::~ZeroConfListener()
  {
    service.removeListener(this);
    service.endBrowsing();
  }

  void ZeroConfListener::instanceAdded(const std::string& instance)
  {
    for (const auto& dev : m_devices)
    {
      if (dev->get_name() == instance)
        return;
    }

    std::string ip = service.get(instance, "servus_ip");
    std::string port = service.get(instance, "servus_port");
    if (ip.empty())
    {
      ip = service.get(instance, "servus_host");
    }

    try
    {
      asio::io_service io_service;
      asio::ip::tcp::resolver resolver(io_service);
      asio::ip::tcp::resolver::query query(ip, port);
      asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
      if (iter->endpoint().address().is_loopback())
      {
        ip = "localhost";
      }
    }
    catch (...)
    {
    }

    if (ip.empty() || port.empty())
    {
      return;
    }

    try
    {
      auto clt = std::make_unique<ossia::net::generic_device>(
          std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
              "ws://" + ip + ":" + port),
          instance);
      clt->get_protocol().update(clt->get_root_node());
      m_devices.push_back(std::move(clt));
    }
    catch (...)
    {
      std::cerr << "Could not connect: "
                << "ws://" + ip + ":" + port << "\n";
    }
  }

  void ZeroConfListener::instanceRemoved(const std::string& instance)
  {
    auto it = ossia::find_if(m_devices, [&](const auto& d) {
      return d->get_name() == instance;
    });

    if (it != m_devices.end())
    {
      m_zombie_devices.push_back(it);
    }
  }

  ossia::net::generic_device* ZeroConfListener::find_device(
      const std::string& instance)
  {
    auto it = ossia::find_if(m_devices, [&](const auto& d) {
      return d->get_name() == instance;
    });

    if (it != m_devices.end())
    {
      return it->get();
    }
    return nullptr;
  }

  void ZeroConfListener::browse()
  {
    for(auto it : m_zombie_devices)
    {
      for (auto client : ossia_pd::instance().clients.reference())
      {
        if(client->is_zeroconf() && client->m_device == it->get())
        {
          ossia::pd::client::client::disconnect(client);
          clock_delay(client->m_clock, 1000); // hardcoded reconnection delay
        }
      }

      m_devices.erase(it);
    }
    m_zombie_devices.clear();

    service.browse(0);
  }

}
} // namespace ossia