#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {

  /* Accepting connections. Describes how to switch an acceptor socket into listening mode and accept incoming
   * connection requests in a TCP server application */

  // The size of the queue containing the pending connection requests. When the queue becomes full, the new connection
  // requests are rejected by the operating system.
  const int BACKLOG_SIZE = 30;

  // Step 1. Here we assume that the server application has already obtained the protocol port number.
  unsigned short port_num = 3333;

  // Step 2. Creating a server endpoint.
  asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

  asio::io_service ios;

  try {
    // Step 3. Instantiating and opening an acceptor socket.
    asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

    // Step 4. Binding the acceptor socket to server endpoint.
    acceptor.bind(ep);

    // Step 5. Starting to listen for incoming connection requests. Unless we call the listen() method on the acceptor
    // object, all connection requests arriving at corresponding endpoint will be rejected by the operating system
    // network software.
    acceptor.listen(BACKLOG_SIZE);

    // Step 6. Creating an active socket.
    asio::ip::tcp::socket sock(ios);

    // Step 7. Processing the next connection request and connecting the active socket to the client. Blocks execution
    // until a new connection request arrives.
    acceptor.accept(sock);

    // At this point 'sock' socket is connected to the client application and can be used to send data to or receive
    // data from it.

  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = " << e.code()
              << ". Message: " << e.what();
    return e.code().value();
  }

  return EXIT_SUCCESS;
}