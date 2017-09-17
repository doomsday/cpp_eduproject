//
// Data structure whose purpose is to keep the data related to a particular request while it is being executed.
//

#ifndef BOOST_ASIO_NPCBOOK_SESSION_HPP
#define BOOST_ASIO_NPCBOOK_SESSION_HPP

#include <boost/asio.hpp>
#include <mutex>
#include <utility>

using namespace boost;

using s_callback = void (*)(unsigned int request_id, const std::string &response, const system::error_code &ec);

struct s_session {
  asio::ip::tcp::socket m_sock; // Socket used for communication.
  asio::ip::tcp::endpoint m_ep; // Remote endpoint.
  std::string m_request;  // Request string.
  asio::streambuf m_response_buf; // Automatically resizable buffer where the response will be stored.
  std::string m_response; // Response represented as string.

  system::error_code m_ec; // Contains the description of an error if one occurs during the life cycle.
  unsigned int m_id; // Unique ID assigned to the request.
  s_callback m_callback; // Pointer to the function to be called when the request completes.
  bool m_was_cancelled;
  std::mutex m_cancel_guard;

  s_session(asio::io_service &ios, const std::string &raw_ip_address, unsigned short port_num, std::string request,
            unsigned int id, s_callback callback) :
      m_sock(ios), m_ep(asio::ip::address::from_string(raw_ip_address), port_num), m_request(std::move(request)),
      m_id(id), m_callback(callback), m_was_cancelled(false) {}
};

#endif //BOOST_ASIO_NPCBOOK_SESSION_HPP
