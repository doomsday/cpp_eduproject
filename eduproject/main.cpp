#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

// Keeps objects we need in a callback to identify whether all data has been read from the socket and to initiate next
// async reading if needed.
struct Session {
  std::shared_ptr<asio::ip::tcp::socket> sock;
  std::unique_ptr<char[]> buf;
  unsigned int buf_size;
};

void readFromSocket(const std::shared_ptr<asio::ip::tcp::socket> &sock);

int main() {

  /* Reading from a TCP socket asynchronously (simplified using asio::async_read) */

  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
    asio::io_service ios;

    // Step 3. Allocating, opening and connecting a socket.
    auto sock(std::make_shared<asio::ip::tcp::socket>(ios, ep.protocol()));
    sock->connect(ep);

    readFromSocket(sock);

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

void readFromSocket(const std::shared_ptr<asio::ip::tcp::socket> &sock) {
  // Step 4. Allocating the buffer.
  const unsigned int MESSAGE_SIZE = 7;

  auto buf(std::make_unique<char[]>(MESSAGE_SIZE));

  // Step 5. Initiating asynchronous reading operation.
  asio::async_read(
      *sock,
      asio::buffer(buf.get(), MESSAGE_SIZE),
      [](const boost::system::error_code &ec, std::size_t bytes_transferred) {
        // Function used as a callback for asynchronous reading operation. Checks if all data has been read from the
        // socket and initiates new reading operation if needed.
        if (ec != 0) {
          std::cout << "Error occurred! Error code = "
                    << ec.value()
                    << ". Message: " << ec.message();
          return;
        }
      }
  );
}