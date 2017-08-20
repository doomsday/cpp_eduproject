#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

void writeToSocket(const std::shared_ptr<asio::ip::tcp::socket> &sock);

int main() {

  /* Writing to a TCP socket asynchronously (simplified using asio::async_write) */

  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
    asio::io_service ios;

    // Step 3. Allocating, opening and connecting a socket.
    auto sock(std::make_shared<asio::ip::tcp::socket>(ios, ep.protocol()));
    sock->connect(ep);
    // This function initiates an asynchronous write operation and returns.
    writeToSocket(sock);

    // Step 6. Run the io_service object's event processing loop. The run() function blocks until all work has finished
    // and there are no more handlers to be dispatched, or until the io_service has been stopped.
    ios.run();
  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = "
              << e.code() << ". Message: "
              << e.what();
    return e.code().value();
  }

  return EXIT_SUCCESS;
}

void writeToSocket(const std::shared_ptr<asio::ip::tcp::socket> &sock) {

  // Step 4. Allocating and filling the buffer.
  std::string buf = std::string("Hello everybody\n");

  // Step 5. Initiating asynchronous write operation.
  asio::async_write(*sock, asio::buffer(buf),
      [](const boost::system::error_code &ec, std::size_t bytes_transferred) {
        // Step 2. Function used as a callback for asynchronous writing operation. Checks if all data from the buffer
        // has been written to the socket and initiates new asynchronous writing operation if needed.
        if (ec != 0) {
          std::cout << "Error occurred! Error code = "
                    << ec.value() << ". Message: "
                    << ec.message();
          return;
        }

        // Here we know that all the data has been written to the socket.
      }
  );
}