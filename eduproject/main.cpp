#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

std::string readFromSocket(asio::ip::tcp::socket &sock);

int main() {

  /* Reading from a TCP socket synchronously  (simplified using asio::read) */

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
  const unsigned char MESSAGE_SIZE = 7;
  char buf[MESSAGE_SIZE];

  asio::read(sock, asio::buffer(buf, MESSAGE_SIZE));

  return std::string(buf, MESSAGE_SIZE);
}