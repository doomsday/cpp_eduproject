#include <boost/asio.hpp>
#include <iostream>
#include <memory>

using namespace boost;

int main() {

  /* Using extensible stream-oriented I/O buffers */

  asio::streambuf buf;

  std::ostream output(&buf);

  // Writing the message to the stream-based buffer. String is written to the output stream object, which in turn
  // redirects the data to the buf stream buffer.
  output << "Message1\nMessage2";

  // Now we want to read all data from a streambuf until '\n' delimiter. Instantiate an input stream which uses our
  // stream buffer.
  std::istream input(&buf);

  // We'll read data into this string.
  std::string message1;

  std::getline(input, message1);

  // Now message1 string contains 'Message1'.

  std::cout << "First part: " << message1 << "\n";
  std::getline(input, message1);
  std::cout << "Second part: " << message1 << std::endl;

  return EXIT_SUCCESS;
}