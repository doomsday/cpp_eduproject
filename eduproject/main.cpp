#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {

  /* Connect a socket to a server application represented by a DNS name and a protocol port number */

  // Step1. Assume that the client application has already obtained the DNS name and protocol port number.
  std::string host = "google.com";
  std::string port_num = "80";

  // Used by a 'resolver' and a 'socket'.
  asio::io_service ios;

  // Creating a resolver's query.
  asio::ip::tcp::resolver::query resolver_query(host, port_num, asio::ip::tcp::resolver::query::numeric_service);

  // Creating a resolver.
  asio::ip::tcp::resolver resolver(ios);

  try {
    // Step 2. Resolving a DNS name.
    asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query);

    // Step 3. Creating a socket. We don't open the socket yet (no endpoints passed as second argument) because we don't
    // know the version of IP addresses to which the provided DNS name will resolve.
    asio::ip::tcp::socket sock(ios);

    // Step 4. asio::connect() method iterates over each endpoint until successfully connects to one of them. It will
    // throw an exception if it fails to connect to all the endpoints or if other error occurs.
    asio::connect(sock, it);

    // At this point socket 'sock' is connected to the server application and can be used to send data to or receive
    // data from it.
  }
    // Overloads of asio::ip::tcp::resolver::resolve and asio::connect() used here throw exceptions in case of error
    // condition.
    catch (system::system_error &e) {
      std::cout << "Error occurred! Error code = " << e.code()
                << ". Message: " << e.what();
    return e.code().value();
  }

  return EXIT_SUCCESS;
}