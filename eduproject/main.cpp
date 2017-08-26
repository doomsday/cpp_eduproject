#include <boost/predef.h>

// Step 1. If the application is intended to run on Windows XP or Windows Server 2003, define flags that enable
// asynchronous operation canceling on these versions of Windows.
#ifdef BOOST_OS_WINDOWS
#if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
#define BOOST_ASIO_DISABLE_IOCP
#define BOOST_ASIO_ENABLE_CANCELIO
#endif
#endif

#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using namespace boost;

int main() {

  /* Canceling asynchronous operations */

  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    // Step 2. Allocate and open a TCP or UDP socket. It may be an active or passive (acceptor) socket in the client or
    // server application
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
    asio::io_service ios;
    auto sock(std::make_shared<asio::ip::tcp::socket>(ios, ep.protocol()));


    // Step 3. Define a callback function or functor for an asynchronous operation. If needed, in this callback,
    // implement a branch of code that handles the situation when the operation has been canceled.
    // Step 4. Initiate one or more asynchronous operations and specify a function or an object defined in step 4 as a
    // callback.

    // Asynchronously connect a socket to the specified remote endpoint.
    sock->async_connect(ep, [sock](const boost::system::error_code &ec) {
      // If async operation has been cancelled or an error occurred during execution, ec contains corresponding error
      // code.
      if (ec != 0) {
        if (ec == asio::error::operation_aborted) {
          // If operation cancelled.
          std::cout << "Operation cancelled";
        } else {
          std::cout << "Error occurred! Error code = "
                    << ec.value()
                    << ". Message: "
                    << ec.message();
        }
        return;
      }
      // At this point the socket is connected and can be used for communication with remote application.
    });

    // Step 5. Spawn an additional thread and use it to run the Boost.Asio event loop.
    std::thread worker_thread([&ios]() {
      try {
        // Run the io_service object's event processing loop. The run() function blocks until all work has finished and
        // there are no more handlers to be dispatched, or until the io_service has been stopped.
        ios.run();
      } catch (system::system_error &e) {
        std::cout << "Error occurred! Error code = "
                  << e.code() << ". Message: "
                  << e.what();
      }
    });

    // Emulating delay.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Step 6. Call the cancel() method on the socket object to cancel all the outstanding asynchronous operations
    // associated with this socket. If the operation has already finished, calling the cancel() method has no
    // effect.
    sock->cancel();

    // Waiting for worker thread to complete.
    worker_thread.join();

  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = "
              << e.code() << ". Message: "
              << e.what();
    return e.code().value();
  }

  return EXIT_SUCCESS;
}