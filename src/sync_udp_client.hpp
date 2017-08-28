//
// Created by drpsy on 27-Aug-17.
//

#ifndef EDUPROJECT_SYNCTCPCLIENT_HPP
#define EDUPROJECT_SYNCTCPCLIENT_HPP

#include <iostream>
#include <boost/asio.hpp>

using namespace boost;

/**
 * Implements and provides access to the communication functionality. Uses only those overloads of Boost.Asio
 * functions and objects' methods that throw exceptions in case of failure.
 */
class sync_udp_client {
 private:
  // This is the object providing access to the operating system's communication services,
  // which are used by the socket object.
  asio::io_service m_ios;
  // This is the UDP socket used for communication.
  asio::ip::udp::socket m_sock;

 public:
  sync_udp_client() :
      m_sock(m_ios) {
    m_sock.open(asio::ip::udp::v4());
  }

  std::string emulate_long_computation_op(unsigned int duration_sec,
                                          const std::string &raw_ip_address,
                                          unsigned short port_num) {
    std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + "\n";
    asio::ip::udp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
    send_request(ep, request);
    return receive_response(ep);
  }

 private:
  void send_request(const asio::ip::udp::endpoint& ep, const std::string& request) {
    // This is UDP, so if the method returns without an error, it only means that the request has been sent and does not
    // mean that the request has been received by the server. UDP protocol doesn't guarantee message delivery and it
    // provides no means to check whether the datagram has been successfully received on the server-side or got lost
    // somewhere on its way to the server.
    m_sock.send_to(asio::buffer(request), ep);
  }

  std::string receive_response(asio::ip::udp::endpoint& ep) {
    // Buffer that will hold the response message. The largest message that the server may send according to the
    // application layer protocol is an ERROR\n string that consists of six ASCII symbols.
    char response[6];
    // If the datagram never arrives the method will never unblock! If the size of the datagram that arrives from the
    // server is larger than the size of the supplied buffer, the method will fail.
    std::size_t bytes_received = m_sock.receive_from(asio::buffer(response, sizeof(response)), ep);
    m_sock.shutdown(asio::ip::udp::socket::shutdown_both);
    return std::string(response, bytes_received);
  }
};

#endif //EDUPROJECT_SYNCTCPCLIENT_HPP
