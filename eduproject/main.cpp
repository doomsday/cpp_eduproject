#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {

  /* Creating an active UDP socket */

  // Step 1. An instance of 'io_service' class is required by socket constructor.
  asio::io_service ios;

  // Step 2. Creating an object of 'udp' class representing a UDP protocol with IPv6 as underlying protocol.
  asio::ip::udp protocol = asio::ip::udp::v6();

  // Step 3. Instantiating an active UDP socket object.
  asio::ip::udp::socket sock(ios);

  // Used to store information about error that happens while opening the socket.
  boost::system::error_code ec;

  // Step 4. Opening the socket.
  sock.open(protocol, ec);
  if (ec.value() != 0) {
    // Failed to open the socket.
    std::cout
        << "Failed to open the socket! Error code = "
        << ec.value() << ". Message: " << ec.message();
    return ec.value();
  }

  return EXIT_SUCCESS;
}