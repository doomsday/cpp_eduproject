//
// Using timers.
//

#include <boost/asio/steady_timer.hpp>
#include <iostream>

using namespace boost;

int main() {

  asio::io_service ios;

  asio::steady_timer t1(ios);
  // Switches the timer to a non-expired state and starts it.
  t1.expires_from_now(std::chrono::seconds(2));

  asio::steady_timer t2(ios);
  t2.expires_from_now(std::chrono::seconds(5));

  // Define and set a callback function that is to be called when the t1 expires.
  t1.async_wait([&t2](boost::system::error_code ec) {
    if (ec == 0) {
      std::cout << "Timer #1 has expired!" << std::endl;
    } else if (ec == asio::error::operation_aborted) {
      std::cout << "Timer #1 has been cancelled!" << std::endl;
    } else {
      std::cout << "Error occurred! Error code = "
                << ec.value()
                << ". Message: " << ec.message()
                << std::endl;
    }

    t2.cancel();
  });

  t2.async_wait([](boost::system::error_code ec) {
    if (ec == 0) {
      std::cout << "Timer #2 has expired!" << std::endl;
    } else if (ec == asio::error::operation_aborted) {
      std::cout << "Timer #2 has been cancelled!" << std::endl;
    } else {
      std::cout << "Error occurred! Error code = "
                << ec.value()
                << ". Message: " << ec.message()
                << std::endl;
    }
  });

  // The method blocks until both the timers expire. The expiration callbacks will be called in the context of
  // the thread that called 'run()'.
  ios.run();

  return EXIT_SUCCESS;
}