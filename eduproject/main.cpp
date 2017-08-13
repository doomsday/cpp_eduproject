#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {

  /* Resolving a DNS name */

  // Step 1. Assume that the client application has already obtained the DNS name and protocol port number and
  // represented them as strings.
  std::string host = "google.com";
  std::string port_num = "80";

  // Step 2. Used by the resolver to access underlying OS's services during a DNS name resolution process.
  asio::io_service ios;

  // Step 3. Creating a query. Because the service is specified as a protocol port number (in our case, 3333) and not
  // as a service name (for example, HTTP, FTP, and so on), we passed the
  // asio::ip::tcp::resolver::query::numeric_service ﬂag to explicitly inform the query object about that, so that it
  // properly parses the port number value.
  asio::ip::tcp::resolver::query resolver_query(host, port_num, asio::ip::tcp::resolver::query::numeric_service);

  // Step 4. Creating a resolver.
  asio::ip::tcp::resolver resolver(ios);

  // Used to store information about error that happens during the resolution process.
  boost::system::error_code ec;

  // Step 5. The DNS name resolution is performed.
  asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query, ec);

  // Handling errors if any.
  if (ec != 0) {
  // Failed to resolve the DNS name. Breaking execution.
    std::cout << "Failed to resolve a DNS name."
              << "Error code = " << ec.value()
              << ". Message = " << ec.message();
    return ec.value();
  }

  asio::ip::tcp::resolver::iterator it_end;

  for(; it != it_end; ++it) {
    // Here we can access the endpoint like this.
    asio::ip::tcp::endpoint ep = it->endpoint();
    std::cout << ep.address() << std::endl;
  }

  return EXIT_SUCCESS;
}