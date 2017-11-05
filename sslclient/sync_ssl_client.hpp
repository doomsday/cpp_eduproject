//
// Created by drpsy on 27-Aug-17.
//

#ifndef EDUPROJECT_SYNCSSLCLIENT_HPP
#define EDUPROJECT_SYNCSSLCLIENT_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

using namespace boost;

/**
 * Class that plays the role of the synchronous SSL/TLS-enabled TCP client.
 */
class sync_ssl_client {
 private:
  // Provides access to the operating system's communication services, which are used by the socket object.
  asio::io_service m_ios;

  // Endpoint designating the server application.
  asio::ip::tcp::endpoint m_ep;

  // An object representing SSL context; basically, this is wrapper around the SSL_CTX data structure defined by OpenSSL
  // library. This object contains global settings and parameters used by other objects and functions involved in the
  // process of communication using SSL/TLS protocol.
  asio::ssl::context m_ssl_context;

  // Represents a stream that wraps a TCP socket object and implements all SSL/TLS communication operations.
  asio::ssl::stream<asio::ip::tcp::socket> m_ssl_stream;

 public:
  sync_ssl_client(const std::string &raw_ip_address, unsigned short port_num) :
      m_ep(asio::ip::address::from_string(raw_ip_address), port_num),
      m_ssl_context(asio::ssl::context::sslv3_client),
      m_ssl_stream(m_ios, m_ssl_context) {
    // Set verification mode and designate that we want to perform verification (during a handshake).
    m_ssl_stream.set_verify_mode(asio::ssl::verify_peer);

    // Set verification callback. Will be called when certificates arrive from the server. The callback is invoked once
    // for each certificate in the certificates chain sent by the server.
    m_ssl_stream.set_verify_callback([this](bool preverified, asio::ssl::verify_context &context) -> bool {
      return on_peer_verify(preverified, context);
    });
  }

  void connect() {
    // Connect the TCP socket. lowest_layer() returns the socket underlying the SSL stream.
    m_ssl_stream.lowest_layer().connect(m_ep);

    // Perform SSL handshake.
    m_ssl_stream.handshake(asio::ssl::stream_base::client);

    // Connections are established and the effective communication can be performed.
  }

  // Shuts the connection down and closes the socket.
  void close() {
    // We ignore any errors that might occur during shutdown as we anyway can't do anything about them.
    boost::system::error_code ec;

    m_ssl_stream.shutdown(ec);  // Shutdown SSL.

    // Shut down the socket.
    m_ssl_stream.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

    m_ssl_stream.lowest_layer().close(ec);

    // Stream object cannot be used to transmit the data anymore.
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
  // Dummy: verification succeeded without performing the actual verification.
  bool on_peer_verify(bool preverified, asio::ssl::verify_context &context) {
    // Here the certificate should be verified and the verification result should be returned.
    return true;
  }

  // Send a string, passed to it as an argument, to the server.
  void send_request(const std::string &request) {
    // All it does is, it fully delegates its job to the asio::write() free function.
    asio::write(m_ssl_stream, asio::buffer(request));
  }

  // Receive response from the server.
  std::string receive_response() {
    asio::streambuf buf;
    // Message sent by the server may vary in length, but must end with the '\n' symbol. Blocks the thread of execution
    // until it encounters '\n'.
    asio::read_until(m_ssl_stream, buf, '\n');
    // Now stream buffer 'buf' contains the response. The 'streambuf' buffer is passed as parameter to the constructor
    // of the istream object to associate it with the stream.
    std::istream input(&buf);
    std::string response;
    std::getline(input, response);

    return response;
  }
};

#endif //EDUPROJECT_SYNCSSLCLIENT_HPP
