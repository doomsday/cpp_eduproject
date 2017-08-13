#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {

  /* Connecting a socket */

  // Step 1. Assume that the client application has already // obtained the IP address and protocol port number of the
  // target server.
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 27017;

  try {
    // Step 2. Creating an endpoint designating a target server application.
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);

    asio::io_service ios;

    // Step 3. Creating and opening a socket. This function connects the socket to the server. The connection is
    // performed synchronously. Before performing the connection establishment procedure, the socket's connect() method
    // will bind the socket to the endpoint consisting of an IP address and a protocol port number chosen by the
    // operating system
    asio::ip::tcp::socket sock(ios, ep.protocol());

    // Step 4. Connecting a socket.
    sock.connect(ep);

    // At this point socket 'sock' is connected to the server application and can be used to send data to or receive
    // data from it.
  }
    // Overloads of asio::ip::address::from_string() and asio::ip::tcp::socket::connect() used here throw exceptions in
    // case of error condition. Both methods have overloads that don't throw exceptions and accept an object of
    // the boost::system::error_code class.
    catch (system::system_error &e) {
      std::cout << "Error occurred! Error code = " << e.code()
                << ". Message: " << e.what();
    return e.code().value();
  }

  return EXIT_SUCCESS;
}