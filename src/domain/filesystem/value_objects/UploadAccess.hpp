/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadAccess.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 20:04:07 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 20:15:33 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOAD_ACCESS_HPP
#define UPLOAD_ACCESS_HPP

#include <string>

namespace domain {
namespace filesystem {
namespace value_objects {

class UploadAccess {
 public:
  static const unsigned int USER_READ_BIT = 1 << 0;
  static const unsigned int USER_WRITE_BIT = 1 << 1;
  static const unsigned int GROUP_READ_BIT = 1 << 2;
  static const unsigned int GROUP_WRITE_BIT = 1 << 3;
  static const unsigned int OTHER_READ_BIT = 1 << 4;
  static const unsigned int OTHER_WRITE_BIT = 1 << 5;
  
  static const unsigned int USER_ALL = USER_READ_BIT | USER_WRITE_BIT;
  static const unsigned int GROUP_ALL = GROUP_READ_BIT | GROUP_WRITE_BIT;
  static const unsigned int OTHER_ALL = OTHER_READ_BIT | OTHER_WRITE_BIT;
  static const unsigned int ALL_PERMISSIONS = USER_ALL | GROUP_ALL | OTHER_ALL;

  static const unsigned int NO_PERMISSION = 0;
  static const unsigned int USER_READ_ONLY = USER_READ_BIT;
  static const unsigned int USER_WRITE_ONLY = USER_WRITE_BIT;
  static const unsigned int GROUP_READ_ONLY = GROUP_READ_BIT;
  static const unsigned int GROUP_WRITE_ONLY = GROUP_WRITE_BIT;
  static const unsigned int OTHER_READ_ONLY = OTHER_READ_BIT;
  static const unsigned int OTHER_WRITE_ONLY = OTHER_WRITE_BIT;

  static const unsigned int READ_ONLY = USER_READ_BIT | GROUP_READ_BIT | OTHER_READ_BIT;
  static const unsigned int READ_WRITE = USER_ALL | GROUP_ALL | OTHER_ALL;
  static const unsigned int USER_READ_WRITE_GROUP_READ = USER_ALL | GROUP_READ_BIT;
  static const unsigned int USER_READ_WRITE_GROUP_READ_OTHER_READ = USER_ALL | GROUP_READ_BIT | OTHER_READ_BIT;

  UploadAccess();
  explicit UploadAccess(unsigned int permissionBits);
  explicit UploadAccess(const std::string& accessString);
  UploadAccess(const UploadAccess& other);
  ~UploadAccess();

  UploadAccess& operator=(const UploadAccess& other);

  unsigned int getPermissionBits() const;
  std::string toString() const;

  static bool isValidPermission(unsigned int permissionBits);
  static bool isValidAccessString(const std::string& accessString);

  bool canUserRead() const;
  bool canUserWrite() const;
  bool canGroupRead() const;
  bool canGroupWrite() const;
  bool canOtherRead() const;
  bool canOtherWrite() const;

  void setUserRead(bool enable);
  void setUserWrite(bool enable);
  void setGroupRead(bool enable);
  void setGroupWrite(bool enable);
  void setOtherRead(bool enable);
  void setOtherWrite(bool enable);

  bool operator==(const UploadAccess& other) const;
  bool operator!=(const UploadAccess& other) const;
  bool operator<(const UploadAccess& other) const;
  bool operator<=(const UploadAccess& other) const;
  bool operator>(const UploadAccess& other) const;
  bool operator>=(const UploadAccess& other) const;

  static UploadAccess fromString(const std::string& accessString);
  static UploadAccess fromBits(unsigned int permissionBits);

  static UploadAccess readOnly();
  static UploadAccess readWrite();
  static UploadAccess userReadWriteGroupReadOtherRead();
  static UploadAccess userReadWriteGroupReadOtherNone();

 private:
  unsigned int m_permissionBits;

  void validate() const;
  static unsigned int parseAccessString(const std::string& accessString);
  
  static bool isValidAccessCharacter(char character);
  static bool isValidWhoString(const std::string& who);
  static bool isValidPermissionString(const std::string& perms);
  
  static unsigned int getBitsForWho(const std::string& who, bool read, bool write);
  static std::string whoToString(unsigned int bits);
  static std::string permsToString(unsigned int bits);
  
  static char readToSymbol(bool hasPermission);
  static char writeToSymbol(bool hasPermission);
};

}  // namespace value_objects
}  // namespace filesystem
}  // namespace domain

#endif  // UPLOAD_ACCESS_HPP
