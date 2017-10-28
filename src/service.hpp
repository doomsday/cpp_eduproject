#ifndef BOOST_ASIO_NPCBOOK_SERVICE_HPP
#define BOOST_ASIO_NPCBOOK_SERVICE_HPP

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <bits/ios_base.h>

using namespace boost;

// Key functional component in the whole application. Implements the actual function (or service) provided by the server
// to the clients.
class service {

 private:
  // Table containing HTTP status codes and status messages.
  static const std::map<unsigned int, std::string> http_status_table;
  // TCP socket object connected to the client.
  std::shared_ptr<boost::asio::ip::tcp::socket> m_sock;
  // Buffer into which the request message is read.
  boost::asio::streambuf m_request;
  // The map where request headers are put when the HTTP request headers block is parsed.
  std::map<std::string, std::string> m_request_headers;
  // URI of the resource (file) requested by the client.
  std::string m_requested_resource;
  // The buffer where the contents of a requested resource is stored before being sent to the client.
  std::unique_ptr<char[]> m_resource_buffer;
  // HTTP response status code.
  unsigned int m_response_status_code;
  // The size of the contents of the requested resource.
  std::size_t m_resource_size_bytes;
  // Properly formatted response headers block.
  std::string m_response_headers;
  // Response status line.
  std::string m_response_status_line;

 public:

  explicit service(std::shared_ptr<asio::ip::tcp::socket> sock) :
      m_sock(std::move(sock)),
      m_request(4096),  // Maximum size of the buffer in bytes (security measure).
      m_response_status_code(200),  // Assume success.
      m_resource_size_bytes(0) {}

  // Initiates an asynchronous communication session with the client connected to the socket.
  void start_handling() {
    asio::async_read_until(*m_sock.get(), m_request, "\r\n",
                           [this](const boost::system::error_code &ec, std::size_t bytes_transferred) {
                             on_request_line_received(ec, bytes_transferred);
                           });
  }

 private:

  // Method that processes the HTTP request line.
  void on_request_line_received(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec != 0) {
      std::cout << "Error occurred! Error code = "
                << ec.value()
                << ". Message: " << ec.message();

      if (ec == asio::error::not_found) {
        // No delimiter has been found in the request message, including more bytes have been received
        // from the client than the size of the buffer.
        m_response_status_code = 413; // "Request Entity Too Large"
        send_response();
        return;
      } else {
        // In case of any other error - close the socket and clean up.
        on_finish();
        return;
      }
    }

    // Parse the request line.
    // Get the whole request line.
    std::string request_line;
    std::istream request_stream(&m_request);
    std::getline(request_stream, request_line, '\r');
    // Remove symbol '\n' from the buffer.
    request_stream.get();

    // Parse the request method.
    std::string request_method;
    std::istringstream request_line_stream(request_line);
    request_line_stream >> request_method;

    // We only support GET method.
    if (request_method != "GET") {
      // Unsupported method.
      m_response_status_code = 501;
      send_response();
      return;
    }

    // Get the requested resource (file).
    request_line_stream >> m_requested_resource;

    // Get the HTTP protocol version.
    std::string request_http_version;
    request_line_stream >> request_http_version;

    if (request_http_version != "HTTP/1.1" && request_http_version != "HTTP/1.0") {
      // Unsupported HTTP version or bad request.
      m_response_status_code = 505;
      send_response();
      return;
    }

    // At this point the request line is successfully received and parsed. Now read the request headers. Request headers
    // block ends with the \r\n\r\n symbol sequence.
    asio::async_read_until(*m_sock.get(), m_request, "\r\n\r\n", [this](const boost::system::error_code &ec,
                                                                        std::size_t bytes_transferred) {
      on_headers_received(ec, bytes_transferred);
    });
  }

  // Process and store the request headers block.
  void on_headers_received(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec != 0) {
      std::cout << "Error occurred! Error code = "
                << ec.value()
                << ". Message: " << ec.message();

      if (ec == asio::error::not_found) {
        // No delimiter has been found in the request message.
        m_response_status_code = 413;
        send_response();
        return;
      } else {
        // In case of any other error - close the socket and clean up.
        on_finish();
        return;
      }
    }

    // Parse and store headers.
    std::istream request_stream(&m_request);
    std::string header_name, header_value;

    while (!request_stream.eof()) {
      std::getline(request_stream, header_name, ':');
      if (!request_stream.eof()) {
        std::getline(request_stream, header_value, '\r');
        // Remove symbol \n from the stream.
        request_stream.get();
        m_request_headers[header_name] = header_value;
      }
    }

    // Now we have all we need to process the request.
    process_request();
    send_response();
  }

  // Read the contents of the requested resource from the file system and store it in the buffer, ready to be sent back
  // to the client.
  void process_request() {
    // Read file.
    std::string resource_file_path = std::string("D:\\http_root") + m_requested_resource;
    if (!boost::filesystem::exists(resource_file_path)) {
      // Resource not found.
      m_response_status_code = 404;
      return;
    }

    std::ifstream resource_fstream(resource_file_path, std::ifstream::binary);
    if (!resource_fstream.is_open()) {
      // Could not open file. Something bad has happened.
      m_response_status_code = 500;
      return;
    }

    // Find out file size.
    resource_fstream.seekg(0, std::ios_base::end);
    m_resource_size_bytes = static_cast<std::size_t>(resource_fstream.tellg());

    m_resource_buffer = std::make_unique<char[]>(m_resource_size_bytes);
    resource_fstream.seekg(std::ios_base::beg);
    resource_fstream.read(m_resource_buffer.get(), m_resource_size_bytes);

    m_response_headers += std::string("content-length: ") + std::to_string(m_resource_size_bytes) + "\r\n";
  }

  // Composes a response message and send it to the client.
  void send_response() {
    // Let the client know that the server will not read any data from it anymore.
    m_sock->shutdown(asio::ip::tcp::socket::shutdown_receive);

    const auto &status_line = http_status_table.at(m_response_status_code);

    m_response_status_line = std::string("HTTP/1.1 ") + status_line + "\r\n";

    m_response_headers += "\r\n";

    std::vector<asio::const_buffer> response_buffers;
    response_buffers.push_back(asio::buffer(m_response_status_line));

    if (m_response_headers.length() > 0) {
      response_buffers.push_back(asio::buffer(m_response_headers));
    }

    if (m_resource_size_bytes > 0) {
      response_buffers.emplace_back(asio::buffer(m_resource_buffer.get(), m_resource_size_bytes));
    }

    // Initiate asynchronous write operation.
    asio::async_write(*m_sock.get(), response_buffers, [this](const boost::system::error_code &ec,
                                                              std::size_t bytes_transferred) {
      on_response_sent(ec, bytes_transferred);
    });
  }

  // Shut down the socket to let the client know that a full response has been sent and no more
  // data will be sent by the server.
  void on_response_sent(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec != 0) {
      std::cout << "Error occurred! Error code = "
                << ec.value()
                << ". Message: " << ec.message();
    }

    m_sock->shutdown(asio::ip::tcp::socket::shutdown_both);

    on_finish();
  }

  // Performs cleanup and deletes an instance of the Service object.
  void on_finish() {
    delete this;
  }
};

const std::map<unsigned int, std::string> service::http_status_table =
    {
        {200, "200 OK"},
        {404, "404 Not Found"},
        {413, "413 Request Entity Too Large"},
        {500, "500 Server Error"},
        {501, "501 Not Implemented"},
        {505, "505 HTTP Version Not Supported"}
    };

#endif //BOOST_ASIO_NPCBOOK_SERVICE_HPP