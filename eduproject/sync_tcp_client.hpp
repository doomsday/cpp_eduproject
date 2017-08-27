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
class sync_tcp_client {
 private:
  // Provides access to the operating system's communication services, which are used by the socket object.
  asio::io_service m_ios;
  // Endpoint designating the server application.
  asio::ip::tcp::endpoint m_ep;
  // Socket used for communication.
  asio::ip::tcp::socket m_sock;

 public:
  sync_tcp_client(const std::string& raw_ip_address, unsigned short port_num) :
      m_ep(asio::ip::address::from_string(raw_ip_address), port_num), m_sock(m_ios) {
    m_sock.open(m_ep.protocol());
  }

  //  Performs the connection of the socket to the server.
  void connect() {
    m_sock.connect(m_ep);
  }

  // Shuts the connection down and closes the socket.
  void close() {
    m_sock.shutdown(asio::ip::tcp::socket::shutdown_both);
    m_sock.close();
  }

  // Where the I/O operations are performed.
  std::string emulate_long_computation_op(unsigned int duration_sec) {
    // Preparing the request string according to the protocol.
    std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + "\n";
    // Send to server.
    send_request(request);
    // Receive the response from the server.
    return receive_response();
  }

 private:
  // Send a string, passed to it as an argument, to the server.
  void send_request(const std::string& request) {
    // All it does is, it fully delegates its job to the asio::write() free function.
    asio::write(m_sock, asio::buffer(request));
  }

  // Receive response from the server.
  std::string receive_response() {
    asio::streambuf buf;
    // Message sent by the server may vary in length, but must end with the '\n' symbol. Blocks the thread of execution
    // until it encounters '\n'.
    asio::read_until(m_sock, buf, '\n');
    // Now stream buffer 'buf' contains the response. The 'streambuf' buffer is passed as parameter to the constructor
    // of the istream object to associate it with the stream.
    std::istream input(&buf);
    std::string response;
    std::getline(input, response);

    return response;
  }
};

#endif //EDUPROJECT_SYNCTCPCLIENT_HPP
