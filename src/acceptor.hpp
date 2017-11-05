#ifndef BOOST_ASIO_NPCBOOK_ACCEPTOR_HPP
#define BOOST_ASIO_NPCBOOK_ACCEPTOR_HPP

#include <boost/asio.hpp>
#include "service.hpp"

using namespace boost;

// Responsible for accepting connection requests arriving from clients and instantiations objects
// of the 'server' class.
class acceptor {
 private:
  asio::io_service &m_ios;
  asio::ip::tcp::acceptor m_acceptor;

  // Contains global settings and parameters used by other object and functions involced in the process of
  // communication using SSL/TLS protocol.
  asio::ssl::context m_ssl_context;
 public:
  acceptor(asio::io_service &ios, unsigned short port_num) :
      m_ios(ios),
      m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)),
      m_ssl_context(asio::ssl::context::sslv23_server) {  // server only, SSL+TLS
    // Setting up the context.
    m_ssl_context.set_options(
        boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use);  // This option creates a new key when using ephemeral
                                                          // (temporary) Diffie-Hellman parameters.

    m_ssl_context.set_password_callback(
        [this](std::size_t max_length, asio::ssl::context::password_purpose purpose) -> std::string {
          return get_password(max_length, purpose);
        });

    m_ssl_context.use_certificate_chain_file("server.crt");
    m_ssl_context.use_private_key_file("server.key", boost::asio::ssl::context::pem);
    m_ssl_context.use_tmp_dh_file("dhparams.pem");

    // Start listening for incoming connection requests.
    m_acceptor.listen();
  }

  // Performs the full handling cycle of one client.
  void accept() {
    // Represents an SSL/TLS communication channel with the underlying TCP socket.
    asio::ssl::stream<asio::ip::tcp::socket> ssl_stream(m_ios, m_ssl_context);

    // If there are pending connection requests available, the connection request is processed and the ssl_stream
    // underlying socket (received from 'lowest_layer()') is connected to the new client. Otherwise, this method
    // blocks until a new connection request arrives.
    m_acceptor.accept(ssl_stream.lowest_layer());
    service svc;
    svc.handle_client(ssl_stream);  // Socket is connected to the client.
  }

 private:
  std::string get_password(std::size_t max_length, asio::ssl::context::password_purpose purpose) const {
    return "pass";
  }
};

#endif //BOOST_ASIO_NPCBOOK_ACCEPTOR_HPP
