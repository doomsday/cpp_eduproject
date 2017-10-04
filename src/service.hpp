#ifndef BOOST_ASIO_NPCBOOK_SERVICE_HPP
#define BOOST_ASIO_NPCBOOK_SERVICE_HPP

#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

// Key functional component in the whole application. Implements the actual function (or service) provided by the server
// to the clients.
class service {
 public:
  service() = default;

  void start_handling_client(const std::shared_ptr<asio::ip::tcp::socket> &sock) {
    std::thread th([this, sock] () {
      handle_client(sock);
    });

    // Separate the thread of execution from the thread object.
    th.detach();
  }

 private:
  void handle_client(const std::shared_ptr<asio::ip::tcp::socket> &sock) {
    try {

      asio::streambuf request;
      asio::read_until(*sock.get(), request, '\n');

      // Emulate request processing: intensively consume CPU.
      int i = 0;
      while (i != 1000000) {
        i++;
      }
      // Sleep for some time to emulate such operations as reading a fle or communicating with a peripheral device
      // synchronously.
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      // Sending response.
      std::string response = "Response\n";
      asio::write(*sock.get(), asio::buffer(response));

    } catch (system::system_error &e) {
      std::cout << "Error occurred! Error code = "
                << e.code() << ". Message: "
                << e.what();
    }

    // Clean-up: delete the associated object of the Service class.
    delete this;
  }
};

#endif //BOOST_ASIO_NPCBOOK_SERVICE_HPP