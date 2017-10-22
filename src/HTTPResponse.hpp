//
// Created by drpsy on 21-Oct-17.
//

#ifndef BOOST_ASIO_NPCBOOK_HTTPRESPONSE_HPP
#define BOOST_ASIO_NPCBOOK_HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <boost/asio.hpp>

using namespace boost;

class HTTPResponse {
 private:
  unsigned int m_status_code; // HTTP status code.
  std::string m_status_message; // HTTP status message.

  // Response headers.
  std::map<std::string, std::string> m_headers;
  asio::streambuf m_response_buf;
  std::istream m_response_stream;

 private:
  friend class HTTPRequest;

  HTTPResponse() :
      m_response_stream(&m_response_buf)
  {}

  asio::streambuf& get_response_buf() {
    return m_response_buf;
  }

  void set_status_code(unsigned int status_code) {
    m_status_code = status_code;
  }

  void set_status_message(const std::string& status_message) {
    m_status_message = status_message;
  }

  void add_header(const std::string& name, const std::string& value) {
    m_headers[name] = value;
  }

 public:
  unsigned int get_status_code() const {
    return m_status_code;
  }

  const std::string& get_status_message() const {
    return m_status_message;
  }

  const std::map<std::string, std::string>& get_headers() {
    return m_headers;
  };

  const std::istream& get_response() const {
    return m_response_stream;
  }
};

#endif //BOOST_ASIO_NPCBOOK_HTTPRESPONSE_HPP
