#include <cstdint>
#include <stdexcept>
#include <string>

struct GUID {
  uint32_t data1;
  uint16_t data2;
  uint16_t data3;
  uint8_t data4[8];

  std::string toClassName() const;
  std::string toString() const;
  inline bool operator==(const GUID &other) const {
    return data1 == other.data1 && data2 == other.data2 &&
           data3 == other.data3 && data4[0] == other.data4[0] &&
           data4[1] == other.data4[1] && data4[2] == other.data4[2] &&
           data4[3] == other.data4[3] && data4[4] == other.data4[4] &&
           data4[5] == other.data4[5] && data4[6] == other.data4[6] &&
           data4[7] == other.data4[7];
  }
};

namespace std {
template <> struct hash<GUID> {
  std::size_t operator()(GUID const &guid) const {
    auto hasher32 = hash<uint32_t>();
    auto hasher16 = hash<uint16_t>();
    auto hasher8 = hash<uint8_t>();
    return hasher32(guid.data1) ^ hasher16(guid.data2) ^ hasher16(guid.data3) ^
           hasher8(guid.data4[0]) ^ hasher8(guid.data4[1]) ^
           hasher8(guid.data4[2]) ^ hasher8(guid.data4[3]) ^
           hasher8(guid.data4[4]) ^ hasher8(guid.data4[5]) ^
           hasher8(guid.data4[6]) ^ hasher8(guid.data4[7]);
  }
};
} // namespace std

namespace guid_parse {
namespace details {
constexpr const size_t short_guid_form_length =
    36; // XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
constexpr const size_t long_guid_form_length =
    38; // {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}

constexpr int parse_hex_digit(const char c) {
  using namespace std::string_literals;
  if ('0' <= c && c <= '9')
    return c - '0';
  else if ('a' <= c && c <= 'f')
    return 10 + c - 'a';
  else if ('A' <= c && c <= 'F')
    return 10 + c - 'A';
  else
    throw std::domain_error{"invalid character in GUID"s};
}

template <class T> constexpr T parse_hex(const char *ptr) {
  constexpr size_t digits = sizeof(T) * 2;
  T result{};
  for (size_t i = 0; i < digits; ++i)
    result |= parse_hex_digit(ptr[i]) << (4 * (digits - i - 1));
  return result;
}

constexpr GUID make_guid_helper(const char *begin) {
  GUID result{};
  result.data1 = parse_hex<uint32_t>(begin);
  begin += 8 + 1;
  result.data2 = parse_hex<uint16_t>(begin);
  begin += 4 + 1;
  result.data3 = parse_hex<uint16_t>(begin);
  begin += 4 + 1;
  result.data4[0] = parse_hex<uint8_t>(begin);
  begin += 2;
  result.data4[1] = parse_hex<uint8_t>(begin);
  begin += 2 + 1;
  for (size_t i = 0; i < 6; ++i)
    result.data4[i + 2] = parse_hex<uint8_t>(begin + i * 2);
  return result;
}

template <size_t N> constexpr GUID make_guid(const char (&str)[N]) {
  using namespace std::string_literals;
  static_assert(
      N == (long_guid_form_length + 1) || N == (short_guid_form_length + 1),
      "String GUID of the form {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} or "
      "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX is expected");

  if constexpr (N == (long_guid_form_length + 1)) {
    if (str[0] != '{' || str[long_guid_form_length - 1] != '}')
      throw std::domain_error{"Missing opening or closing brace"s};
  }

  return make_guid_helper(str + (N == (long_guid_form_length + 1) ? 1 : 0));
}
} // namespace details
using details::make_guid;

namespace literals {
constexpr GUID operator"" _guid(const char *str, size_t N) {
  using namespace std::string_literals;
  using namespace details;

  if (!(N == long_guid_form_length || N == short_guid_form_length))
    throw std::domain_error{
        "String GUID of the form {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} or XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX is expected"s};
  if (N == long_guid_form_length &&
      (str[0] != '{' || str[long_guid_form_length - 1] != '}'))
    throw std::domain_error{"Missing opening or closing brace"s};

  return make_guid_helper(str + (N == long_guid_form_length ? 1 : 0));
}
} // namespace literals
} // namespace guid_parse
