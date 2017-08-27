#include <boost/asio.hpp>
#include <iostream>
#include <thread>

#include "server.hpp"

using namespace boost;

void communicate(asio::ip::tcp::socket& sock);

std::thread start_server();

void stop_server(std::thread& server);

int main() {

  /* Shutting down and closing a socket */

  // Starting server.
  std::thread server = start_server();

  // Starting client.
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
    asio::io_service ios;
    asio::ip::tcp::socket sock(ios, ep.protocol());
    sock.connect(ep);
    // Communication is established.
    communicate(sock);
  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = "
              << e.code() << ". Message: "
              << e.what();
    return e.code().value();
  }

  // Stopping server.
  stop_server(server);

  return EXIT_SUCCESS;
}

void communicate(asio::ip::tcp::socket& sock) {
  // Allocating and filling the buffer with binary data.
  const char request_buf[] = {0x48, 0x65, 0x0, 0x6c, 0x6c, 0x6f};

  // Sending the request data.
  asio::write(sock, asio::buffer(request_buf, sizeof(request_buf)));

  // Shutting down the socket to let the server know that we've sent the whole request.
  sock.shutdown(asio::socket_base::shutdown_send);

  // We use extensible buffer for response because we don't know the size of the response message.
  asio::streambuf response_buf;

  system::error_code ec;
  asio::read(sock, response_buf, ec);

  if (ec == asio::error::eof) {
    // Whole response message has been received. Here we can handle it. After the client has shut down its socket's send
    // part, it can still read data from the socket because the receive part of the socket stays open independently from
    // the send part
  } else {
    throw system::system_error(ec);
  }
}

std::thread start_server() {
  return std::thread([]{
    auto * srv = new server();
    srv->start();
  });
}

void stop_server(std::thread& server) {
  if (server.joinable()) {
    server.join();
  }
}