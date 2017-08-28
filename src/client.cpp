
#include <iostream>

#include "sync_udp_client.hpp"

/*
 * Acts as a user of the 'sync_udp_client' class.
 */
int main() {

  /* Implementing a synchronous UDP client */

  const std::string server1_raw_ip_address = "127.0.0.1";
  const unsigned short server1_port_num = 3333;

  const std::string server2_raw_ip_address = "192.168.1.10";
  const unsigned short server2_port_num = 3334;

  try {
    sync_udp_client client;

    // Consume the same service from two different servers.
    std::cout << "Sending request to the server #1 ... " << std::endl;
    std::string response = client.emulate_long_computation_op(10, server1_raw_ip_address, server1_port_num);
    std::cout << "Response from the server #1 received: " << response << std::endl;

    std::cout << "Sending request to the server #2 ... " << std::endl;
    response = client.emulate_long_computation_op(10, server2_raw_ip_address, server2_port_num);
    std::cout << "Response from the server #2 received: " << response << std::endl;

  } catch (system::system_error &e) {
    std::cout << "Error occurred! Error code = "
              << e.code() << ". Message: "
              << e.what();
  }

  return EXIT_SUCCESS;
}