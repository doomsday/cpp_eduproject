#ifndef BOOST_ASIO_NPCBOOK_ACCEPTOR_HPP
#define BOOST_ASIO_NPCBOOK_ACCEPTOR_HPP

#include <boost/asio.hpp>
#include "service.hpp"

using namespace boost;

class acceptor {
 private:
  asio::io_service& m_ios;
  asio::ip::tcp::acceptor m_acceptor;
 public:
  acceptor(asio::io_service& ios, unsigned short port_num):
      m_ios(ios),
      m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)) {
    m_acceptor.listen();
  }

  // Performs the full handling cycle of one client.
  void accept() {
    asio::ip::tcp::socket sock(m_ios);  // Active socket.

    // If there are pending connection requests available, the connection request is processed and the active socket
    // sock is connected to the new client. Otherwise, this method blocks until a new connection request arrives.
    m_acceptor.accept(sock);
    service svc;
    svc.handle_client(sock);  // Socket is connected to the client.
  }
};

#endif //BOOST_ASIO_NPCBOOK_ACCEPTOR_HPP
