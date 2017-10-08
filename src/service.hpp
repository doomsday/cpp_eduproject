#ifndef BOOST_ASIO_NPCBOOK_SERVICE_HPP
#define BOOST_ASIO_NPCBOOK_SERVICE_HPP

#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>
#include <utility>

using namespace boost;

// Key functional component in the whole application. Implements the actual function (or service) provided by the server
// to the clients.
class service {
 public:
  explicit service(std::shared_ptr<asio::ip::tcp::socket> sock) :
      m_sock(std::move(sock)) {}

  void start_handling() {
    asio::async_read_until(*m_sock.get(), m_request, '\n', [this](const boost::system::error_code &ec,
                                                                  std::size_t bytes_transferred) {
      on_request_received(ec, bytes_transferred);
    });
  }

 private:
  std::shared_ptr<asio::ip::tcp::socket> m_sock;
  std::string m_response;
  asio::streambuf m_request;

  void on_request_received(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec != 0) {
      std::cout << "Error occurred! Error code = "
                << ec.value()
                << ". Message: " << ec.message();
      on_finish();
      return;
    }

    // Process the request.
    m_response = process_request(m_request);

    // Initialize asynchronous write operation.
    asio::async_write(*m_sock.get(), asio::buffer(m_response), [this](const boost::system::error_code &ec,
                                                                      std::size_t bytes_transferred) {
      on_response_set(ec, bytes_transferred);
    });
  }

  void on_response_set(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec != 0) {
      std::cout << "Error occurred! Error code = "
                << ec.value()
                << ". Message: " << ec.message();
    }

    on_finish();
  }

  // Here we perform the cleanup.
  void on_finish() {
    delete this;
  }

  std::string process_request(asio::streambuf &request) {
    // In this method we parse the request, process it and prepare the request.

    // Emulate CPU-consuming operations.
    int i = 0;
    while (i != 1000000)
      i++;

    // Emulate operations that block the thread (e.g. sync I/O operations).
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Prepare and return the response message.
    std::string response = "Response\n";
    return response;
  }
};

#endif //BOOST_ASIO_NPCBOOK_SERVICE_HPP