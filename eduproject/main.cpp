#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

void writeToSocket(asio::ip::tcp::socket &sock);

int main() {

  /* Writing to a TCP socket synchronously */

  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);

    asio::io_service ios;

    // Step 1. Allocating and opening the socket.
    asio::ip::tcp::socket sock(ios, ep.protocol());

    sock.connect(ep);

    writeToSocket(sock);

  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = " << e.code()
              << ". Message: " << e.what();
    return e.code().value();
  }

  return EXIT_SUCCESS;
}

void writeToSocket(asio::ip::tcp::socket &sock) {
  // Step 2. Allocating and filling the buffer.
  std::string buf = "Hello";

  std::size_t total_bytes_written = 0;

  // Step 3. Run the loop until all data is written to the socket.
  while (total_bytes_written != buf.length()) {
    total_bytes_written += sock.write_some(
        asio::buffer(buf.c_str() + total_bytes_written,
                     buf.length() - total_bytes_written)
    );
  }
}