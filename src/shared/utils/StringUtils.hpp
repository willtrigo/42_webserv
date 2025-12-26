/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 03:32:26 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/26 03:37:31 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>

namespace shared {
namespace utils {

class StringUtils {
 public:
  static const int BASE_DECIMAL = 10;
  static const int BASE_HEXADECIMAL = 16;
  static const int BASE_OCTAL = 8;

  static bool isAllDigits(const std::string& str);
  static bool isAllHexDigits(const std::string& str);
  static bool isAllOctalDigits(const std::string& str);
  static bool isAllAlphanumeric(const std::string& str);
  static bool containsOnly(const std::string& str,
                           const std::string& validChars);

  static unsigned long toUnsignedLong(const std::string& str,
                                      int base = BASE_DECIMAL);
  static long toLong(const std::string& str, int base = BASE_DECIMAL);
  static int toInt(const std::string& str, int base = BASE_DECIMAL);

  static std::string trim(const std::string& str);
  static std::string trimLeft(const std::string& str);
  static std::string trimRight(const std::string& str);

  static bool startsWith(const std::string& str, const std::string& prefix);
  static bool endsWith(const std::string& str, const std::string& suffix);

  static std::string toUpperCase(const std::string& str);
  static std::string toLowerCase(const std::string& str);

 private:
  StringUtils(const StringUtils&);
  ~StringUtils();

  StringUtils& operator=(const StringUtils&);

  static bool isValidBase(int base);
};

}  // namespace utils
}  // namespace shared

#endif  // STRING_UTILS_HPP
