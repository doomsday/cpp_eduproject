//
// Implementing the HTTP client application.
//

#include <boost/predef.h> // Tools to identify the OS.

// We need this to enable cancelling of I/O operations on Windows XP, Windows Server 2003 and earlier. Refer to
// "http://www.boost.org/doc/libs/1_58_0/doc/html/boost_asio/reference/basic_stream_socket/cancel/overload1.html"
// for details.
#ifdef BOOST_OS_WINDOWS
#if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
#define BOOST_ASIO_DISABLE_IOCP
#define BOOST_ASIO_ENABLE_CANCELIO
#endif
#endif

#include <iostream>
#include "HTTPRequest.hpp"
#include "HTTPClient.hpp"

using namespace boost;

void handler(const HTTPRequest& request, const HTTPResponse& response, const system::error_code& ec) {
  if (ec == 0) {
    std::cout << "RESPONSE TO REQUEST #" << request.get_id() << ":\n" << response.get_response().rdbuf();
  } else if (ec == asio::error::operation_aborted) {
    std::cout << "Request #" << request.get_id() << " has been cancelled by the user" << std::endl;
  } else {
    std::cout << "Request #" << request.get_id() << " failed! Error code = " << ec.value() << ". Error message = "
              << ec.message() << std::endl;
  }
}

int main() {

  try {

    HTTPClient client;

    std::shared_ptr<HTTPRequest> request_one = client.create_request(1);
    request_one->set_host("tcpipguide.com");
    request_one->set_uri("/free/diagrams/httpresponse.png");
    request_one->set_port(80);
    request_one->set_callback(handler);

    request_one->execute();

    // Do nothing for 15 seconds, letting the request complete.
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Closing the client and exiting the application.
    client.close();

  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = "
              << e.code() << ". Message: "
              << e.what() << std::endl;
    return e.code().value();
  }

  return EXIT_SUCCESS;
}