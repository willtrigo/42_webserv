/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Size.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 09:57:33 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/20 10:24:53 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIZE_HPP
#define SIZE_HPP

#include <string>

namespace domain {
namespace value_objects {

class Size {
 public:
  static const std::size_t BYTES_PER_KB = 1024;
  static const std::size_t BYTES_PER_MB = BYTES_PER_KB * BYTES_PER_KB;
  static const std::size_t BYTES_PER_GB = BYTES_PER_KB * BYTES_PER_MB;
  static const std::size_t MAX_SIZE = static_cast<std::size_t>(-1);

  static const int BASE_DECIMAL = 10;

  enum Unit {
    UNIT_BYTES,
    UNIT_KILOBYTES,
    UNIT_MEGABYTES,
    UNIT_GIGABYTES,
    UNIT_UNKNOWN
  };

  static const char* UNIT_STRINGS[];

  Size();
  explicit Size(std::size_t bytes);
  explicit Size(const std::string& sizeString);
  Size(const Size& other);
  ~Size();

  Size& operator=(const Size& other);

  std::size_t getBytes() const;
  std::size_t getKilobytes() const;
  std::size_t getMegabytes() const;
  std::size_t getGigabytes() const;
  std::string toString() const;
  std::string toString(Unit unit) const;

  static bool isValidSizeString(const std::string& sizeString);
  static bool isValidBytes(std::size_t bytes);

  bool operator==(const Size& other) const;
  bool operator!=(const Size& other) const;
  bool operator<(const Size& other) const;
  bool operator<=(const Size& other) const;
  bool operator>(const Size& other) const;
  bool operator>=(const Size& other) const;

  Size operator+(const Size& other) const;
  Size operator-(const Size& other) const;
  Size& operator+=(const Size& other);
  Size& operator-=(const Size& other);

  static Size fromString(const std::string& sizeString);
  static Size fromKilobytes(std::size_t kilobytes);
  static Size fromMegabytes(std::size_t megabytes);
  static Size fromGigabytes(std::size_t gigabytes);

  static Size zero();
  static Size kilobyte();
  static Size megabyte();
  static Size gigabyte();

  bool isZero() const;

 private:
  std::size_t m_bytes;

  void validate() const;
  static std::size_t parseSizeString(const std::string& sizeString);

  static std::size_t extractNumber(const std::string& sizeString,
                                   std::string::size_type& pos);
  static Unit extractUnit(const std::string& sizeString,
                          std::string::size_type pos);
  static std::size_t convertToBytes(std::size_t number, Unit unit);
  static bool isOverflowMultiplication(std::size_t multiplicand,
                                       std::size_t multiplier);

  static std::string unitToString(Unit unit);
  static char unitToChar(Unit unit);
  static Unit charToUnit(char chr);
};

}  // namespace value_objects
}  // namespace domain

#endif  // SIZE_HPP
