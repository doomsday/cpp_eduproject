//
// Implementing an asynchronous TCP server
//

#include <iostream>
#include <boost/asio.hpp>
#include "server.hpp"

using namespace boost;

const unsigned int DEFAULT_THREAD_POOL_SIZE = 2;

int main() {

  unsigned short port_num = 3333;

  try {

    server srv;

    unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;

    if (thread_pool_size == 0) {
      thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
    }

    // Method does not block.
    srv.start(port_num, thread_pool_size);

    // Allow the server to run for some time.
    std::this_thread::sleep_for(std::chrono::seconds(60));

    srv.stop();

  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = "
              << e.code() << ". Message: "
              << e.what();
    return e.code().value();
  }

  return EXIT_SUCCESS;
}