#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
  // Step 1. Assume that the client application has already obtained the IP-address and the protocol port number
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  // Used to store information about error that happens while parsing the raw IP-address
  boost::system::error_code ec;

  // Step 2. Using IP protocol version independent address representation
  asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);
  if (ec.value()!=0) {
    std::cout
        << "Failed to parse the IP address. Error code = "
        << ec.value() << ". Message: " << ec.message();
    return ec.value();
  }

  // Step 3. Instantiate the object of the asio::ip::tcp::endpoint class from the address object created in step 2
  // and a port number
  asio::ip::tcp::endpoint ep(ip_address, port_num);

  // Step 4. The endpoint is ready and can be used to specify a particular server in the network the client wants to
  // communicate with
  return EXIT_SUCCESS;
}