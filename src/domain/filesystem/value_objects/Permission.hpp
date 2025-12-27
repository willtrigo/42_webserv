/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Permission.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 00:07:43 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 03:53:36 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PERMISSION_HPP
#define PERMISSION_HPP

#include <string>

namespace domain {
namespace filesystem {
namespace value_objects {

class Permission {
 public:
  enum Class { CLASS_OWNER, CLASS_GROUP, CLASS_OTHER, CLASS_ALL, CLASS_COUNT };

  enum Mode { MODE_READ, MODE_WRITE, MODE_EXECUTE, MODE_COUNT };

  static const unsigned int MIN_PERMISSION = 0;
  static const unsigned int MAX_PERMISSION = 07777;

  static const unsigned int READ_BIT = 4;
  static const unsigned int WRITE_BIT = 2;
  static const unsigned int EXECUTE_BIT = 1;

  static const unsigned int NO_PERMISSION = 0;
  static const unsigned int READ_ONLY = 0444;
  static const unsigned int WRITE_ONLY = 0222;
  static const unsigned int EXECUTE_ONLY = 0111;
  static const unsigned int READ_WRITE = 0666;
  static const unsigned int READ_EXECUTE = 0555;
  static const unsigned int WRITE_EXECUTE = 0333;
  static const unsigned int ALL_PERMISSIONS = 0777;

  static const unsigned int SETUID_BIT = 04000;
  static const unsigned int SETGID_BIT = 02000;
  static const unsigned int STICKY_BIT = 01000;

  static const unsigned int OWNER_READ_WRITE_EXECUTE = 0700;
  static const unsigned int OWNER_READ_WRITE_GROUP_READ = 0640;
  static const unsigned int OWNER_READ_WRITE_GROUP_READ_OTHER_READ = 0644;
  static const unsigned int OWNER_ALL_GROUP_READ_EXECUTE_OTHER_READ_EXECUTE =
      0755;

  static const std::size_t SYMBOLIC_STRING_LENGTH = 9;
  static const std::size_t SYMBOLIC_STRING_WITH_SPECIAL_LENGTH = 10;
  static const std::size_t SPECIAL_BITS_START_INDEX = 3;
  static const std::size_t OWNER_PERMISSION_SHIFT = 6;
  static const std::size_t GROUP_PERMISSION_SHIFT = 3;
  static const std::size_t OTHER_PERMISSION_SHIFT = 0;

  static const std::size_t OWNER_READ_POSITION = 0;
  static const std::size_t OWNER_WRITE_POSITION = 1;
  static const std::size_t OWNER_EXECUTE_POSITION = 2;
  static const std::size_t GROUP_READ_POSITION = 3;
  static const std::size_t GROUP_WRITE_POSITION = 4;
  static const std::size_t GROUP_EXECUTE_POSITION = 5;
  static const std::size_t OTHER_READ_POSITION = 6;
  static const std::size_t OTHER_WRITE_POSITION = 7;
  static const std::size_t OTHER_EXECUTE_POSITION = 8;

  static const int BASE_OCTAL = 8;

  Permission();
  explicit Permission(unsigned int octalPermission);
  explicit Permission(const std::string& permissionString);
  Permission(const Permission& other);
  ~Permission();

  Permission& operator=(const Permission& other);

  unsigned int getOctalValue() const;
  std::string toString() const;
  std::string toSymbolicString() const;

  static bool isValidPermission(unsigned int permission);
  static bool isValidPermissionString(const std::string& permissionString);
  static bool isValidSymbolicString(const std::string& symbolicString);

  bool canRead(Class permissionClass) const;
  bool canWrite(Class permissionClass) const;
  bool canExecute(Class permissionClass) const;
  bool hasSetUid() const;
  bool hasSetGid() const;
  bool hasStickyBit() const;

  void setRead(Class permissionClass, bool enable);
  void setWrite(Class permissionClass, bool enable);
  void setExecute(Class permissionClass, bool enable);
  void setSetUid(bool enable);
  void setSetGid(bool enable);
  void setStickyBit(bool enable);

  bool operator==(const Permission& other) const;
  bool operator!=(const Permission& other) const;
  bool operator<(const Permission& other) const;
  bool operator<=(const Permission& other) const;
  bool operator>(const Permission& other) const;
  bool operator>=(const Permission& other) const;

  static Permission fromString(const std::string& permissionString);
  static Permission fromSymbolicString(const std::string& symbolicString);
  static Permission fromOctal(unsigned int octalValue);

  static Permission readOnly();
  static Permission readWrite();
  static Permission readWriteExecute();
  static Permission ownerReadWriteGroupReadOtherRead();
  static Permission ownerReadWriteGroupReadOtherNone();
  static Permission ownerAllGroupReadExecuteOtherReadExecute();

 private:
  unsigned int m_permission;

  void validate() const;
  static unsigned int parsePermissionString(
      const std::string& permissionString);
  static unsigned int parseSymbolicString(const std::string& symbolicString);
  static bool hasSpecialBitsFormat(const std::string& symbolicString);
  static unsigned int parseSpecialBits(const std::string& symbolicString);
  static unsigned int parseRegularPermissions(const std::string& symbolicString,
                                              std::size_t startIndex);

  static bool isOctalDigit(char character);
  static bool isValidOctalString(const std::string& octalString);
  static bool isPermissionCharacter(char character);
  static unsigned int getBitMask(Class permissionClass, Mode mode);
  static unsigned int getClassShift(Class permissionClass);

  static char modeToSymbol(Mode mode, bool hasPermission);
  static std::string classToString(Class permissionClass);
};

}  // namespace value_objects
}  // namespace filesystem
}  // namespace domain

#endif  // PERMISSION_HPP
