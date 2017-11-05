//
// Getting and setting socket options.
//

#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {

  // Let's consider a hypothetical situation where we want to make the size of the socket's receive buffer two times
  // bigger than whatever its size now.

  // Each socket option, whose value can be set or obtained by means of a functionality provided by Boost.Asio, is
  // represented by a separate class.

  try {

    asio::io_service ios;

    // Create and open a TCP socket. Before we can get or set options on a particular socket object, the corresponding
    // socket must be opened.
    asio::ip::tcp::socket sock(ios, asio::ip::tcp::v4());

    // Create an object representing receive buffer size option.
    asio::socket_base::receive_buffer_size cur_buf_size;

    // Get the currently set value of the option. Method deduces the option that is requested by the type of the
    // argument passed to it.
    sock.get_option(cur_buf_size);

    std::cout << "Current receive buffer size is " << cur_buf_size.value() << " bytes." << std::endl;

    // Create an object representing receive buffer size option with new value.
    asio::socket_base::receive_buffer_size new_buf_size(cur_buf_size.value() * 2);

    // Set new value of the option.
    sock.set_option(new_buf_size);

    std::cout << "New receive buffer size is "
              << new_buf_size.value() << " bytes." << std::endl;

  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = "
              << e.code() << ". Message: " << e.what();

    return e.code().value();
  }

  return EXIT_SUCCESS;
}