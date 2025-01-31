#include "lnk.h"
#include "guid.h"
#include <format>

bool ShellLinkHeader::validate() const {
  if (this->size != shell_link_header_size) {
    return false;
  }

  // TODO: Add class_identifier validation

  return true;
}

std::tuple<std::string, std::string> ShellLinkHeader::parseHotKey() const {
  auto hot_key_low = this->hot_key & 0xFF;
  auto hot_key_high = (this->hot_key >> 8) & 0xFF;
  std::string hot_key_1;
  std::string hot_key_2;

  if (hot_key_low == 0) {
    // No key assigned to hot_key_1
  } else if ((0x30 <= hot_key_low && hot_key_low <= 0x39) ||
             (0x41 <= hot_key_low && hot_key_low <= 0x5A)) {
    hot_key_1 = (char)hot_key_low;
  } else if (0x70 <= hot_key_low && hot_key_low <= 0x87) {
    hot_key_1 = std::format("F{}", hot_key_low - 0x6F);
  } else if (hot_key_low == 0x90) {
    hot_key_1 = "NUM LOCK";
  } else if (hot_key_low == 0x91) {
    hot_key_1 = "SCROLL LOCK";
  }

  if (hot_key_high & 0x1) {
    hot_key_2 += "SHIFT ";
  }
  if (hot_key_high & 0x2) {
    hot_key_2 += "CTRL ";
  }
  if (hot_key_high & 0x4) {
    hot_key_2 += "ALT";
  }
  if (hot_key_high == 0) {
    // No modifier used to hot_key_2
  } else if (hot_key_2.empty() == true) {
    hot_key_2 = "ABNORMAL";
  }

  return std::make_tuple(hot_key_1, hot_key_2);
}
