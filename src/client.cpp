//
// Implementing an asynchronous TCP client
//

#include <iostream>
#include <boost/asio.hpp>
#include "async_tcp_client.hpp"

using namespace boost;

// Outputs the result of the request execution and the response message to the standard output stream if the request is
// completed successfully.
void handler(unsigned int request_id, const std::string &response, const system::error_code &ec);

// Invoked in the context of the UI thread. This function emulates the behavior of the user who initiates and cancels
// requests.
int main() {

  try {

    async_tcp_client client;

    // Here we emulate the user's behavior.

    // User initiated a request with id 1.
    client.emulate_long_computation_op(10, "127.0.0.1", 3333, handler, 1);

    // Then does nothing for 5 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Then initiates another request with id 2.
    client.emulate_long_computation_op(11, "127.0.0.1", 3334, handler, 2);

    // Then decides to cancel the request with id 1.
    client.cancelRequest(1);

    // Does nothing for another 6 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(6));

    // Initiates one more request assigning ID3 to it.
    client.emulate_long_computation_op(12, "127.0.0.1", 3335, handler, 3);

    // Does nothing for another 15 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(15));

    // Decites to exit the application.
    client.close();

  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = "
              << e.code() << ". Message: "
              << e.what();
    return e.code().value();
  }

  return EXIT_SUCCESS;
}

void handler(unsigned int request_id, const std::string &response, const system::error_code &ec) {
  if (ec == 0) {
    std::cout << "Request #" << request_id << " has completed. Response: " << response << std::endl;
  } else if (ec == asio::error::operation_aborted) {
    std::cout << "Request #" << request_id << " has been cancelled by user" << std::endl;
  } else {
    std::cout << "Request #" << request_id << " failed. Error code = " << ec.value() << ". Error message = "
              << ec.message() << std::endl;
  }
}