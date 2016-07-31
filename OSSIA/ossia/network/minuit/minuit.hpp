/*!
 * \file JamomaMinuit.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <future>
#include <string>
#include <set>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/domain/domain.hpp>

#include <ossia/network/minuit/detail/minuit_name_table.hpp>

#include <ossia/editor/value/value.hpp>
#include <unordered_map>
#include <mutex>

namespace ossia
{
namespace net
{
class generic_device;
class OSSIA_EXPORT minuit_protocol final : public ossia::net::protocol_base
{
    private:
        std::string    mIp;
        uint16_t       mInPort{};            /// the port that a remote device opens
        uint16_t       mOutPort{};           /// the port where a remote device sends OSC messages to (opened in this library)
        bool           mLearning{};          /// if the device is currently learning from inbound messages.

        std::mutex mListeningMutex;
        std::unordered_map<std::string, ossia::net::address_base*> mListening;

        std::promise<void> mNamespacePromise;
        ossia::net::generic_device* mDevice;

        std::set<std::string, std::less<>> m_namespaceRequests;
    public:
        osc::sender    mSender;
        ossia::minuit::name_table mLocalNameTable;
        ossia::minuit::name_table mRemoteNameTable;
        std::promise<void> mGetPromise;

        minuit_protocol(std::string, uint16_t, uint16_t);
        ~minuit_protocol();

        void setDevice(ossia::net::device_base& dev) override;

        const std::string& getIp() const;
        minuit_protocol& setIp(std::string);

        uint16_t getInPort() const;
        minuit_protocol& setInPort(uint16_t);

        uint16_t getOutPort() const;
        minuit_protocol& setOutPort(uint16_t);

        bool update(ossia::net::node_base& node_base) override;

        bool pull(ossia::net::address_base& address_base) override;

        bool push(const ossia::net::address_base& address_base) override;

        bool observe(ossia::net::address_base& address_base, bool enable) override;

        void refresh(boost::string_ref req, const std::string& addr)
        {
          auto it = m_namespaceRequests.find(addr);
          if(it == m_namespaceRequests.end())
          {
            m_namespaceRequests.insert(addr);
            mSender.send(req, boost::string_ref{addr});
          }
        }

        void refreshed(boost::string_ref addr)
        {
          auto it = m_namespaceRequests.find(addr);
          if(it != m_namespaceRequests.end())
          {
            m_namespaceRequests.erase(it);
          }

          if(m_namespaceRequests.empty())
          {
            mNamespacePromise.set_value();
          }
        }

    private:
        void handleReceivedMessage(
                const oscpack::ReceivedMessage& m,
                const oscpack::IpEndpointName& ip);

        osc::receiver  mReceiver;


};
}
}
