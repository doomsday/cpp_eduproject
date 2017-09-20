#ifndef BOOST_ASIO_NPCBOOK_SERVICE_HPP
#define BOOST_ASIO_NPCBOOK_SERVICE_HPP

#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

// Key functional component in the whole application.
class service {
 public:
  service() = default;

  void handle_client(asio::ip::tcp::socket &sock) {
    try {

      asio::streambuf request;
      asio::read_until(sock, request, '\n');

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
      asio::write(sock, asio::buffer(response));

    } catch (system::system_error &e) {
      std::cout << "Error occurred! Error code = "
                << e.code() << ". Message: "
                << e.what();
    }
  }
};

#endif //BOOST_ASIO_NPCBOOK_SERVICE_HPP