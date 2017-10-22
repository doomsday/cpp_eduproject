//
// Created by drpsy on 21-Oct-17.
//

#ifndef BOOST_ASIO_NPCBOOK_HTTPREQUEST_HPP
#define BOOST_ASIO_NPCBOOK_HTTPREQUEST_HPP

#include <boost/asio.hpp>
#include <mutex>
#include <iostream>

#include "HTTPResponse.hpp"
#include "http_errors_category.hpp"

using namespace boost;

class HTTPRequest;

typedef void(*Callback) (const HTTPRequest& request, const HTTPResponse& response, const system::error_code& ec);

class HTTPRequest {
 private:
  friend class HTTPClient;

  // Request parameters.
  std::string m_host;
  unsigned int m_port;
  std::string m_uri;

  // Object unique identifier.
  unsigned int m_id;

  // Callback to be called when request completes.
  Callback m_callback;

  // Buffer containing the request line.
  std::string m_request_buf;

  asio::ip::tcp::socket m_sock;
  asio::ip::tcp::resolver m_resolver;

  HTTPResponse m_response;

  bool m_was_cancelled;
  std::mutex m_cancel_mux;

  asio::io_service &m_ios;

  static const unsigned int DEFAULT_PORT = 80;

  HTTPRequest(asio::io_service &ios, unsigned int id) :
      m_port(DEFAULT_PORT),
      m_id(id),
      m_callback(nullptr),
      m_sock(ios),
      m_resolver(ios),
      m_was_cancelled(false),
      m_ios(ios) {}

 public:
  void set_host(const std::string &host) {
    m_host = host;
  }

  void set_port(unsigned int port) {
    m_port = port;
  }

  void set_uri(const std::string &uri) {
    m_uri = uri;
  }

  // Allows setting a pointer to a callback function that will be called when the request completes.
  void set_callback(Callback callback) {
    m_callback = callback;
  }

  std::string get_host() const {
    return m_host;
  }

  unsigned int get_port() const {
    return m_port;
  }

  const std::string &get_uri() const {
    return m_uri;
  }

  unsigned int get_id() const {
    return m_id;
  }

  // Initiates the execution of the request.
  void execute() {
    // Ensure that preconditions hold.
    assert(m_port > 0);
    assert(m_host.length() > 0);
    assert(m_uri.length() > 0);
    assert(m_callback != nullptr);

    // Prepare the resolving query.
    asio::ip::tcp::resolver::query resolver_query(m_host, std::to_string(m_port),
                                                  asio::ip::tcp::resolver::query::numeric_service);

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
    if (m_was_cancelled) {
      cancel_lock.unlock();
      on_finish(boost::system::error_code(asio::error::operation_aborted));
      return;
    }

    // Resolve the host name.
    m_resolver.async_resolve(resolver_query, [this](const boost::system::error_code &ec,
                                                    asio::ip::tcp::resolver::iterator iterator) {
      on_host_name_resolved(ec, iterator);
    });
  }

  // Canceling the initiated request before it completes.
  void cancel() {
    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

    m_was_cancelled = true;

    if (m_sock.is_open()) {
      // Finish immediately, and the handlers for cancelled operations will be passed the
      // boost::asio::error::operation_aborted error.
      m_sock.cancel();
    }
  }

 private:
  // Method that is used as a callback for an asynchronous DNS name resolution operation.
  void on_host_name_resolved(const boost::system::error_code &ec, asio::ip::tcp::resolver::iterator iterator) {
    if (ec != 0) {
      on_finish(ec);
      return;
    }

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
    if (m_was_cancelled) {
      cancel_lock.unlock();
      on_finish(boost::system::error_code(asio::error::operation_aborted));
      return;
    }

    // Connect to the host.
    asio::async_connect(m_sock, iterator, [this](const boost::system::error_code &ec,
                                                 asio::ip::tcp::resolver::iterator iterator) {
      on_connection_established(ec, iterator);
    });
  }

  // Method used as a callback for an asynchronous connection operation, which is initiated in the
  // on_host_name_resolved() method.
  void on_connection_established(const boost::system::error_code &ec, asio::ip::tcp::resolver::iterator iterator) {
    if (ec != 0) {
      on_finish(ec);
      return;
    }

    // Compose the request message.
    m_request_buf += "GET " + m_uri + " HTTP/1.1\r\n";

    // Add mandatory header.
    m_request_buf += "Host: " + m_host + "\r\n";
    m_request_buf += "\r\n";

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

    if (m_was_cancelled) {
      cancel_lock.unlock();
      on_finish(boost::system::error_code(asio::error::operation_aborted));
      return;
    }

    // Send the request message:
    // "GET /assets/images/392dffac024b9632664e6f2c0cac6fe5-logo.png HTTP/1.1\r\nHost: mvnrepository.com\r\n\r\n"
    asio::async_write(m_sock, asio::buffer(m_request_buf), [this](const boost::system::error_code &ec,
                                                                  std::size_t bytes_transferred) {
      on_request_sent(ec, bytes_transferred);
    });
  }

  // Callback, which is called after the request message is sent to the server.
  void on_request_sent(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec != 0) {
      on_finish(ec);
      return;
    }

    m_sock.shutdown(asio::ip::tcp::socket::shutdown_send);

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
    if (m_was_cancelled) {
      cancel_lock.unlock();
      on_finish(boost::system::error_code(asio::error::operation_aborted));
      return;
    }

    // Read the status line.
    asio::async_read_until(m_sock, m_response.get_response_buf(), "\r\n", [this](const boost::system::error_code &ec,
                                                                                 std::size_t bytes_transferred) {
      on_status_line_received(ec, bytes_transferred);
    });
  }

  // Callback method, which is called when the frst portion of the response message, namely, status line, is received
  // from the server.
  void on_status_line_received(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec != 0) {
      on_finish(ec);
      return;
    }

    // Parse the status line.
    std::string http_version;
    std::string str_status_code;
    std::string status_message;
    unsigned int status_code;

    std::istream response_stream(&m_response.get_response_buf());
    response_stream >> http_version;

    if (http_version != "HTTP/1.1" && http_version != "HTTP/1.0") {
      // Response is incorrect.
      on_finish(http_errors::invalid_response);
      return;
    }

    response_stream >> str_status_code;

    // Convert status code to integer.
    try {
      status_code = std::stoul(str_status_code);  // Interprets an unsigned integer value in the string.
    } catch (std::logic_error &) {
      // Response is incorrect.
      on_finish(http_errors::invalid_response);
      return;
    }

    // Remove 'space' from the buffer.
    response_stream.get();
    std::getline(response_stream, status_message, '\r');
    // Remove symbol '\n' from the buffer.
    response_stream.get();

    m_response.set_status_code(status_code);
    m_response.set_status_message(status_message);

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
    if (m_was_cancelled) {
      cancel_lock.unlock();
      on_finish(boost::system::error_code(asio::error::operation_aborted));
      return;
    }

    // At this point the status line is successfully received and parsed. Now read the response headers.
    asio::async_read_until(m_sock, m_response.get_response_buf(), "\r\n\r\n",
                           [this](const boost::system::error_code &ec, std::size_t bytes_transferred) {
                             on_headers_received(ec, bytes_transferred);
                           });
  }

  // Reads response headers.
  void on_headers_received(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec != 0) {
      on_finish(ec);
      return;
    }

    // Parse and store the headers.
    std::string header, header_name, header_value;
    std::istream response_stream(&m_response.get_response_buf());

    std::cout << "HEADERS OF RESPONSE #" << m_id << ":" << std::endl;
    while (true) {
      std::getline(response_stream, header, '\r');

      // Remove \n symbol from the stream.
      response_stream.get();

      if (header.empty()) {
        break;
      }

      size_t separator_pos = header.find(':');
      if (separator_pos != std::string::npos) { // npos - Value returned by various member functions when they fail.
        header_name = header.substr(0, separator_pos);

        if (separator_pos < header.length() - 1) {
          header_value = header.substr(separator_pos + 1);
        } else {
          header_value = "";
        }
      }
      std::cout << header_name << ":";
      std::cout << header_value << std::endl;
    }
    std::cout << std::endl;

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
    if (m_was_cancelled) {
      cancel_lock.unlock();
      on_finish(boost::system::error_code(asio::error::operation_aborted));
      return;
    }
    // Now we want to read the response body.
    asio::async_read(m_sock, m_response.get_response_buf(), [this](const boost::system::error_code &ec,
                                                                   std::size_t bytes_transferred) {
      on_response_body_received(ec, bytes_transferred);
    });
  }

  // Method that will handle the last part of the response—the response body.
  void on_response_body_received(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (ec == asio::error::eof) {
      on_finish(boost::system::error_code());
    } else {
      on_finish(ec);
    }
  }

  // Method that serves as a fnal point of all execution paths (including erroneous) that start in the execute() method.
  void on_finish(const boost::system::error_code& ec) {
    if (ec != 0) {
      std::cout << "Error occurred! Error code = "
                << ec.value()
                << ". Message: " << ec.message() << std::endl;
    }

    m_callback(*this, m_response, ec);
  }
};

#endif //BOOST_ASIO_NPCBOOK_HTTPREQUEST_HPP
