#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

std::string readFromSocket(asio::ip::tcp::socket &sock);

int main() {

  /* Reading from a TCP socket synchronously (read until a specific symbol) */

  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);

    asio::io_service ios;

    asio::ip::tcp::socket sock(ios, ep.protocol());

    sock.connect(ep);

    readFromSocket(sock);

  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = " << e.code()
              << ". Message: " << e.what();
  }

  return EXIT_SUCCESS;
}

std::string readFromSocket(asio::ip::tcp::socket &sock) {
  asio::streambuf buf;

  // Synchronously read data from the socket until '\n' symbol is encountered.
  asio::read_until(sock, buf, '\n');

  std::string message;

  // Because buffer 'buf' may contain some other data after '\n' symbol, we have to parse the buffer and extract only
  // symbols before the delimiter.

  std::istream input_stream(&buf);
  std::getline(input_stream, message);

  std::cout << message;

  return message;
}