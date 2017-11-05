#ifndef BOOST_ASIO_NPCBOOK_SERVER_HPP
#define BOOST_ASIO_NPCBOOK_SERVER_HPP

#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <atomic>
#include "acceptor.hpp"

using namespace boost;

// Initiates the start-up of the server.
class server {
 private:
  std::unique_ptr<std::thread> m_thread;
  std::atomic<bool> m_stop;
  asio::io_service m_ios;

  void run(unsigned short port_num) {
    acceptor acc(m_ios, port_num);

    while (!m_stop.load()) {
      acc.accept();
    }
  }
 public:
  server() :
      m_stop(false) {}

  void start(unsigned short port_num) {
    m_thread = std::make_unique<std::thread>([this, port_num]() {
      run(port_num);
    });
  }

  // Synchronously stops the server.
  void stop() {
    m_stop.store(true);
    m_thread->join();
  }
};

#endif //BOOST_ASIO_NPCBOOK_SERVER_HPP
