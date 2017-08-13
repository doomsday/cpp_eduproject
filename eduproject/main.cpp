#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {

  /* Creating a passive (acceptor) TCP socket */

  // Step 1. An instance of 'io_service' class is required by socket constructor.
  asio::io_service ios;

  // Step 2. Creating an object of 'tcp' class representing a TCP protocol with IPv6 as underlying protocol.
  asio::ip::tcp protocol = asio::ip::tcp::v6();

  // Step 3. Instantiating an acceptor socket object.
  // Step 4. Opening the acceptor socket.
  try {
    asio::ip::tcp::acceptor acceptor(ios, protocol);
  } catch (boost::system::system_error &error) {
    // Failed to open the socket.
    std::cout << "Error occurred! Error code = " << error.code()
      << ". Message: " << error.what();
  }

  return EXIT_SUCCESS;
}