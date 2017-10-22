//
// Created by drpsy on 22-Oct-17.
//

#ifndef BOOST_ASIO_NPCBOOK_HTTPCLIENT_HPP
#define BOOST_ASIO_NPCBOOK_HTTPCLIENT_HPP

#include <boost/asio.hpp>
#include <thread>

#include "HTTPRequest.hpp"

using namespace boost;

/*
 *  Responsible for the following three functions:
 *  * To establish a threading policy
 *  * To spawn and destroy threads in a pool of threads running the Boost.Asio event loop and delivering asynchronous
 *  operations' completion events
 *  * To act as a factory of the HTTPRequest objects
 */
class HTTPClient {
 private:
  asio::io_service m_ios;
  std::unique_ptr<boost::asio::io_service::work> m_work;
  std::unique_ptr<std::thread> m_thread;

 public:
  HTTPClient() {
    // Make sure that threads running the event loop do not exit this loop when there are no pending asynchronous
    // operations.
    m_work = std::make_unique<boost::asio::io_service::work>(m_ios);

    // Create a thread of control
    m_thread = std::make_unique<std::thread>([this](){
      // and add it to the pool.
      m_ios.run();
    });
  }

  std::shared_ptr<HTTPRequest> create_request(unsigned int id) {
    return std::shared_ptr<HTTPRequest>(new HTTPRequest(m_ios, id));
  }

  void close() {
    // Destroy the work object allowing threads to exit the event loop just as soon as all pending operations complete.
    m_work.reset(NULL);

    // Waiting for the I/O thread to exit.
    m_thread->join();
  }
};

#endif //BOOST_ASIO_NPCBOOK_HTTPCLIENT_HPP
