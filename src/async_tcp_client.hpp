
#ifndef BOOST_ASIO_NPCBOOK_ASYNC_TCP_CLIENT_HPP
#define BOOST_ASIO_NPCBOOK_ASYNC_TCP_CLIENT_HPP

#include <boost/predef.h>

// We need this to enable cancelling of I/O operations on Windows XP, Windows Server 2003 and earlier. Refer to
// "http://www.boost.org/doc/libs/1_58_0/doc/html/boost_asio/reference/basic_stream_socket/cancel/overload1.html"
// for details.
#ifdef BOOST_OS_WINDOWS
#if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
#define BOOST_ASIO_DISABLE_IOCP
#define BOOST_ASIO_ENABLE_CANCELIO
#endif
#endif

#include <boost/asio.hpp>

#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include "session.hpp"

using namespace boost;

class async_tcp_client : public boost::noncopyable {
 private:
  asio::io_service m_ios;
  std::map<int, std::shared_ptr<s_session>> m_active_sessions;
  std::mutex m_active_sessions_guard;
  // Class to inform the io_service when it has work to do. Keeps threads running event loop from exiting this loop when
  // there are no pending asynchronous operations.
  std::unique_ptr<asio::io_service::work> m_work;
  std::unique_ptr<std::thread> m_thread;

  // Method is called whenever the request completes with any result. Performs a cleanup and then to call a callback
  // provided by the caller.
  void onRequestComplete(const std::shared_ptr<s_session> &session) {

    boost::system::error_code ignored_ec;

    // Shutdown both send and receive on the socket. This method may fail in case socket is not connected. We don't care
    // about the error code if this function fails. Note that here we use the overload of the socket's shutdown()
    // method, which doesn't throw exceptions.
    session->m_sock.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);

    // Remove session from the map of active sessions.
    std::unique_lock<std::mutex> active_sessions_lock(m_active_sessions_guard);
    auto it = m_active_sessions.find(session->m_id);
    if (it != m_active_sessions.end()) {
      m_active_sessions.erase(it);
    }
    active_sessions_lock.unlock();

    boost::system::error_code ec;

    if (session->m_ec == 0 && session->m_was_cancelled) {
      ec = asio::error::operation_aborted;
    } else {
      ec = session->m_ec;
    }

    // Call the callback provided by the user.
    session->m_callback(session->m_id, session->m_response, ec);
  }

 public:
  // Default constructor.
  async_tcp_client() {
    m_work = std::make_unique<asio::io_service::work>(m_ios);
    m_thread = std::make_unique<std::thread>([this]() {
      // The spawned thread plays the role of I/O thread in our application; in the context of this thread, the
      // callbacks assigned asynchronous operations will be invoked.
      m_ios.run();  // Run the io_service object's event processing loop.
    });
  }

  // This method initiates a request to the server.
  void emulate_long_computation_op(unsigned int duration_sec, const std::string &raw_ip_address,
                                   unsigned short port_num, s_callback callback, unsigned int request_id) {
    // Preparing the request string.
    std::string request = "EMULATE_LONG_CALC_OP " + std::to_string(duration_sec) + "\n";

    auto session = std::make_shared<s_session>(m_ios, raw_ip_address, port_num, request, request_id, callback);

    session->m_sock.open(session->m_ep.protocol());

    // Add new session to the list of active sessions so that we can access it if the user decides to cancel the
    // corresponding request before it completes. Because active sessions list can be accessed from multiple threads, we
    // guard it with a mutex to avoid data corruption.
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
    m_active_sessions[request_id] = session;
    lock.unlock();

    // Session object is cached, we need to connect the socket to the server.
    session->m_sock.async_connect(session->m_ep, [this, session](const system::error_code &ec) {
      if (ec != 0) {
        session->m_ec = ec;
        onRequestComplete(session);
        return;
      }

      std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);
      if (session->m_was_cancelled) {
        onRequestComplete(session);
        return;
      }

      // Send the request data to the server.
      asio::async_write(session->m_sock, asio::buffer(session->m_request),
                        [this, session](const boost::system::error_code &ec, std::size_t bytes_transferred) {
                          if (ec != 0) {
                            session->m_ec = ec;
                            onRequestComplete(session);
                            return;
                          }

                          std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);  // Lock in another thread.
                          if (session->m_was_cancelled) {
                            onRequestComplete(session);
                            return;
                          }

                          // Receive a response from the server.
                          asio::async_read_until(session->m_sock, session->m_response_buf, '\n',
                                                 [this, session](const boost::system::error_code &ec,
                                                                 std::size_t bytes_transferred) {
                                                   if (ec != 0) {
                                                     session->m_ec = ec;
                                                   } else {
                                                     std::istream strm(&session->m_response_buf);
                                                     std::getline(strm, session->m_response);
                                                   }
                                                   onRequestComplete(session);
                                                 });
                        });
    });
  }

  // This method cancels the previously initiated request designated by the request_id argument.
  void cancelRequest(unsigned int request_id) {
    std::unique_lock<std::mutex> lock(m_active_sessions_guard);

    auto it = m_active_sessions.find(request_id);
    if (it != m_active_sessions.end()) {
      std::unique_lock<std::mutex> cancel_lock(it->second->m_cancel_guard);
      it->second->m_was_cancelled = true;
      it->second->m_sock.cancel(); // Immediately cancel all asynchronous operations associated with the socket.
    }
  }

  // This method blocks the calling thread until all the currently running requests complete and deinitializes the
  // client. When this method returns, the corresponding instance of the AsyncTCPClient class can't be used anymore.
  void close() {
    // Destroy work object. This allows the I/O thread to exit the event loop when there are no more pending async
    // operations.
    m_work.reset(NULL);

    // Wait for the I/O thread to exit.
    m_thread->join();
  }
};

#endif //BOOST_ASIO_NPCBOOK_ASYNC_TCP_CLIENT_HPP
