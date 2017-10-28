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
  asio::io_service m_ios;
  // Class to inform the io_service when it has work to do. Keeps threads running event loop from exiting this loop when
  // there are no pending asynchronous operations.
  std::unique_ptr<asio::io_service::work> m_work;
  std::unique_ptr<acceptor> acc;
  std::vector<std::unique_ptr<std::thread>> m_thread_pool;

 public:
  server() {
    m_work = std::make_unique<asio::io_service::work>(m_ios);
  }

  // Start the server.
  void start(unsigned short port_num, unsigned int thread_pool_size) {
    assert(thread_pool_size > 0);

    // Create and start acceptor.
    acc = std::make_unique<acceptor>(m_ios, port_num);
    acc->start();

    // Create specified number of threads and add them to the pool.
    for (unsigned int i = 0; i < thread_pool_size; i++) {
      // Multiple threads may call the run() function to set up a pool of threads from which the io_service may
      // execute handlers. All threads that are waiting in the pool are equivalent and the io_service may choose any
      // one of them to invoke a handler.
      std::unique_ptr<std::thread> th(new std::thread([this]() {
        m_ios.run();
      }));
      m_thread_pool.push_back(std::move(th));
    }
  }

  // Stop the server.
  void stop() {
    // Stop acceptor.
    acc->stop();
    //  Exit as soon as possible, discarding all pending asynchronous operations.
    m_ios.stop();

    for (auto &thread : m_thread_pool) {
      thread->join();
    }
  }
};

#endif //BOOST_ASIO_NPCBOOK_SERVER_HPP
