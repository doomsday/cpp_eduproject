#ifndef BOOST_ASIO_NPCBOOK_SERVICE_HPP
#define BOOST_ASIO_NPCBOOK_SERVICE_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <thread>
#include <atomic>
#include <iostream>

using namespace boost;

// Key functional component in the whole application. Responsible for handling a single client.
class service {
 public:
  service() = default;

  void handle_client(asio::ssl::stream<asio::ip::tcp::socket> &ssl_stream) {
    try {
      // Blocks until the handshake (as server) completes.
      ssl_stream.handshake(asio::ssl::stream_base::server);

      asio::streambuf request;
      asio::read_until(ssl_stream, request, '\n');

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
      asio::write(ssl_stream, asio::buffer(response));

    } catch (system::system_error &e) {
      std::cout << "Error occurred! Error code = "
                << e.code() << ". Message: "
                << e.what();
    }
  }
};

#endif //BOOST_ASIO_NPCBOOK_SERVICE_HPP