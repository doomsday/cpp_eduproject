#ifndef BOOST_ASIO_NPCBOOK_ACCEPTOR_HPP
#define BOOST_ASIO_NPCBOOK_ACCEPTOR_HPP

#include <boost/asio.hpp>
#include <utility>
#include "service.hpp"

using namespace boost;

// The 'acceptor' class is a part of the server application infrastructure.
class acceptor {
 private:
  asio::io_service &m_ios;
  asio::ip::tcp::acceptor m_acceptor;
  std::atomic<bool> m_is_stopped{};

 public:
  acceptor(asio::io_service &ios, unsigned short port_num) :
      m_ios(ios),
      m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)) {
    m_is_stopped.store(false);
  }

  // Start accepting incoming connection request.
  void start() {
    m_acceptor.listen();
    init_accept();
  }

  // Stop accepting incoming connection request.
  void stop() {
    m_is_stopped.store(true);
  }

 private:
  // Performs the full handling cycle of one client.
  void init_accept() {

    // Will be accepted into this socket.
    auto sock(std::make_shared<asio::ip::tcp::socket>(m_ios));

    m_acceptor.async_accept(*sock.get(), [this, sock](const boost::system::error_code &error) {
      on_accept(error, sock);
    });
  }

  void on_accept(const boost::system::error_code &ec, std::shared_ptr<asio::ip::tcp::socket> sock) {
    if (ec == 0) {
      (new service(std::move(sock)))->start_handling();
    } else {
      std::cout << "Error occurred! Error code = "
                << ec.value()
                << ". Message: " << ec.message();
    }

    // Init next async accept operation if acceptor has not been stopped yet.
    if (!m_is_stopped.load()) {
      init_accept();
    } else {
      // Stop accepting incoming connections and free allocated resources.
      m_acceptor.close();
    }
  }
};

#endif //BOOST_ASIO_NPCBOOK_ACCEPTOR_HPP
