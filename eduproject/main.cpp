#include <boost/asio.hpp>
#include <iostream>
#include <memory>

using namespace boost;

int main() {

  /* Using fixed length I/O buffers */

  // OUTPUT BUFFER

  // 'out_data' is the raw buffer.
  std::string out_data;

  // Step 1, 2. Allocate a buffer with the data that is to be used as the output.
  out_data = "Hello";

  // Step 3. Represent the buffer as an object that satisfies ConstBufferSequence concept's requirements.
  asio::const_buffers_1 output_buf = asio::buffer(out_data);

  // Step 4. 'output_buf' is the representation of the
  // buffer 'buf' that can be used in Boost.Asio output
  // operations.

  // INPUT BUFFER

  // We expect to receive a block of data no more than 20 bytes long.
  const size_t BUF_SIZE_BYTES = 20;

  // Step 1. Allocating the buffer.
  auto inp_data(std::make_unique<char[]>(BUF_SIZE_BYTES));

  // Step 2. Creating buffer representation that satisfies MutableBufferSequence concept requirements.
  asio::mutable_buffers_1 input_buf = asio::buffer(static_cast<void *>(inp_data.get()), BUF_SIZE_BYTES);

  // Step 3. 'input_buf' is the representation of the buffer 'buf' tat can be used in Boost.Asio input operations.

  return EXIT_SUCCESS;
}