//
// Implementing a synchronous iterative TCP server
//

#include <iostream>
#include <boost/asio.hpp>
#include "server.hpp"

using namespace boost;

int main() {

  unsigned short port_num = 3333;

  try {

    server srv;
    srv.start(port_num);

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