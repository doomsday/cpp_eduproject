//
// Performing a stream-based I/O.
//
// Boost.Asio provides tools that allow us to implement inter-process communication in a stream-based fashion.
// In this recipe, we will see how to use them.
//

#include <boost/asio.hpp>
#include <iostream>
#include <chrono>

using namespace boost;

int main() {

  // Wraps a TCP socket and provides an I/O stream-like interface to it. Automatically tries to resolve the DNS name
  // and then tries to connect to that server.
  asio::ip::tcp::iostream stream("google.com", "http");  // Or ("google.com", "http").

  // Test whether connection has succeeded.
  if (!stream) {
    std::cout << "Error occurred! Error code = "
              << stream.error().value()
              << ". Message = " << stream.error().message()
                                << std::endl;
    return EXIT_FAILURE;
  }

  stream << "Request.";
  stream.flush(); // Make sure all the buffered data is pushed to the server.

  stream.expires_from_now(boost::posix_time::seconds(10));

  std::cout << "Response: "
            << stream.rdbuf();  // Return a pointer to the underlying streambuf.

  return EXIT_SUCCESS;
}