//
// Created by drpsy on 20-Oct-17.
//

#ifndef BOOST_ASIO_NPCBOOK_HTTP_ERRORS_CATEGORY_HPP
#define BOOST_ASIO_NPCBOOK_HTTP_ERRORS_CATEGORY_HPP

#include <boost/system/error_code.hpp>

namespace http_errors {

  enum http_error_codes {
    invalid_response = 1
  };

/**
 * Class representing an error category.
 */
  class http_errors_category : public boost::system::error_category {
   public:
    const char *name() const BOOST_SYSTEM_NOEXCEPT override { // BOOST_NOEXCEPT -> noexcept
      return "http_errors";
    }

    std::string message(int e) const override {
      switch (e) {
        case invalid_response:return "Server response cannot be processed.";
          break;
        default:return "Unknown error.";
          break;
      }
    }
  };

  const boost::system::error_category &get_http_errors_category() {
    static http_errors_category cat;
    return cat;
  }

  boost::system::error_code make_error_code(http_error_codes e) {
    return {static_cast<int>(e), get_http_errors_category()};
  }

} // namespace http_errors

namespace boost {
  namespace system {
    template<>
    struct is_error_code_enum<http_errors::http_error_codes> {
      BOOST_STATIC_CONSTANT(bool, value = true);
    };
  } // namespace system
} // namespace boost

#endif //BOOST_ASIO_NPCBOOK_HTTP_ERRORS_CATEGORY_HPP
