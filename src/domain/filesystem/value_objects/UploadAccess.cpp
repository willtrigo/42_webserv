/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadAccess.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 20:12:58 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 20:19:28 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/value_objects/UploadAccess.hpp"
#include "domain/filesystem/exceptions/UploadAccessException.hpp"

#include <cctype>
#include <sstream>

namespace domain {
namespace filesystem {
namespace value_objects {

UploadAccess::UploadAccess()
    : m_permissionBits(USER_READ_WRITE_GROUP_READ_OTHER_READ) {}

UploadAccess::UploadAccess(unsigned int permissionBits)
    : m_permissionBits(permissionBits) {
  validate();
}

UploadAccess::UploadAccess(const std::string& accessString)
    : m_permissionBits(USER_READ_WRITE_GROUP_READ_OTHER_READ) {
  m_permissionBits = parseAccessString(accessString);
  validate();
}

UploadAccess::UploadAccess(const UploadAccess& other)
    : m_permissionBits(other.m_permissionBits) {}

UploadAccess::~UploadAccess() {}

UploadAccess& UploadAccess::operator=(const UploadAccess& other) {
  if (this != &other) {
    m_permissionBits = other.m_permissionBits;
  }
  return *this;
}

unsigned int UploadAccess::getPermissionBits() const {
  return m_permissionBits;
}

std::string UploadAccess::toString() const {
  std::ostringstream oss;
  bool first = true;
  
  if (canUserRead() || canUserWrite()) {
    if (!first) oss << " ";
    oss << "user:";
    if (canUserRead()) oss << "r";
    if (canUserWrite()) oss << "w";
    first = false;
  }
  
  if (canGroupRead() || canGroupWrite()) {
    if (!first) oss << " ";
    oss << "group:";
    if (canGroupRead()) oss << "r";
    if (canGroupWrite()) oss << "w";
    first = false;
  }
  
  if (canOtherRead() || canOtherWrite()) {
    if (!first) oss << " ";
    oss << "all:";
    if (canOtherRead()) oss << "r";
    if (canOtherWrite()) oss << "w";
  }
  
  std::string result = oss.str();
  if (result.empty()) {
    return "none";
  }
  return result;
}

bool UploadAccess::isValidPermission(unsigned int permissionBits) {
  return (permissionBits & ~ALL_PERMISSIONS) == 0;
}

bool UploadAccess::isValidAccessString(const std::string& accessString) {
  try {
    parseAccessString(accessString);
    return true;
  } catch (const exceptions::UploadAccessException&) {
    return false;
  }
}

void UploadAccess::validate() const {
  if (!isValidPermission(m_permissionBits)) {
    std::ostringstream oss;
    oss << "Upload access bits out of range: " << m_permissionBits 
        << " (valid bits: 0x0-0x" << std::hex << ALL_PERMISSIONS << ")";
    throw exceptions::UploadAccessException(
        oss.str(), exceptions::UploadAccessException::OUT_OF_RANGE);
  }
}

bool UploadAccess::canUserRead() const {
  return (m_permissionBits & USER_READ_BIT) != 0;
}

bool UploadAccess::canUserWrite() const {
  return (m_permissionBits & USER_WRITE_BIT) != 0;
}

bool UploadAccess::canGroupRead() const {
  return (m_permissionBits & GROUP_READ_BIT) != 0;
}

bool UploadAccess::canGroupWrite() const {
  return (m_permissionBits & GROUP_WRITE_BIT) != 0;
}

bool UploadAccess::canOtherRead() const {
  return (m_permissionBits & OTHER_READ_BIT) != 0;
}

bool UploadAccess::canOtherWrite() const {
  return (m_permissionBits & OTHER_WRITE_BIT) != 0;
}

void UploadAccess::setUserRead(bool enable) {
  if (enable) {
    m_permissionBits |= USER_READ_BIT;
  } else {
    m_permissionBits &= ~USER_READ_BIT;
  }
}

void UploadAccess::setUserWrite(bool enable) {
  if (enable) {
    m_permissionBits |= USER_WRITE_BIT;
  } else {
    m_permissionBits &= ~USER_WRITE_BIT;
  }
}

void UploadAccess::setGroupRead(bool enable) {
  if (enable) {
    m_permissionBits |= GROUP_READ_BIT;
  } else {
    m_permissionBits &= ~GROUP_READ_BIT;
  }
}

void UploadAccess::setGroupWrite(bool enable) {
  if (enable) {
    m_permissionBits |= GROUP_WRITE_BIT;
  } else {
    m_permissionBits &= ~GROUP_WRITE_BIT;
  }
}

void UploadAccess::setOtherRead(bool enable) {
  if (enable) {
    m_permissionBits |= OTHER_READ_BIT;
  } else {
    m_permissionBits &= ~OTHER_READ_BIT;
  }
}

void UploadAccess::setOtherWrite(bool enable) {
  if (enable) {
    m_permissionBits |= OTHER_WRITE_BIT;
  } else {
    m_permissionBits &= ~OTHER_WRITE_BIT;
  }
}

bool UploadAccess::operator==(const UploadAccess& other) const {
  return m_permissionBits == other.m_permissionBits;
}

bool UploadAccess::operator!=(const UploadAccess& other) const {
  return m_permissionBits != other.m_permissionBits;
}

bool UploadAccess::operator<(const UploadAccess& other) const {
  return m_permissionBits < other.m_permissionBits;
}

bool UploadAccess::operator<=(const UploadAccess& other) const {
  return m_permissionBits <= other.m_permissionBits;
}

bool UploadAccess::operator>(const UploadAccess& other) const {
  return m_permissionBits > other.m_permissionBits;
}

bool UploadAccess::operator>=(const UploadAccess& other) const {
  return m_permissionBits >= other.m_permissionBits;
}

UploadAccess UploadAccess::fromString(const std::string& accessString) {
  return UploadAccess(accessString);
}

UploadAccess UploadAccess::fromBits(unsigned int permissionBits) {
  return UploadAccess(permissionBits);
}

UploadAccess UploadAccess::readOnly() {
  return UploadAccess(READ_ONLY);
}

UploadAccess UploadAccess::readWrite() {
  return UploadAccess(READ_WRITE);
}

UploadAccess UploadAccess::userReadWriteGroupReadOtherRead() {
  return UploadAccess(USER_READ_WRITE_GROUP_READ_OTHER_READ);
}

UploadAccess UploadAccess::userReadWriteGroupReadOtherNone() {
  return UploadAccess(USER_READ_WRITE_GROUP_READ);
}

unsigned int UploadAccess::parseAccessString(const std::string& accessString) {
  if (accessString.empty()) {
    throw exceptions::UploadAccessException(
        "Upload access string cannot be empty",
        exceptions::UploadAccessException::EMPTY_STRING);
  }
  
  if (accessString == "none") {
    return NO_PERMISSION;
  }
  
  unsigned int permissionBits = 0;
  std::istringstream iss(accessString);
  std::string token;
  
  while ((iss >> token) != 0) {
    size_t colonPos = token.find(':');
    if (colonPos == std::string::npos) {
      throw exceptions::UploadAccessException(
          "Invalid upload access format: missing colon in token '" + token + "'",
          exceptions::UploadAccessException::INVALID_FORMAT);
    }
    
    std::string who = token.substr(0, colonPos);
    std::string perms = token.substr(colonPos + 1);
    
    if (!isValidWhoString(who)) {
      throw exceptions::UploadAccessException(
          "Invalid who specifier: '" + who + "'",
          exceptions::UploadAccessException::INVALID_WHO);
    }
    
    if (!isValidPermissionString(perms)) {
      throw exceptions::UploadAccessException(
          "Invalid permission string: '" + perms + "'",
          exceptions::UploadAccessException::INVALID_PERMISSION);
    }
    
    bool read = perms.find('r') != std::string::npos;
    bool write = perms.find('w') != std::string::npos;
    
    permissionBits |= getBitsForWho(who, read, write);
  }
  
  return permissionBits;
}

bool UploadAccess::isValidAccessCharacter(char character) {
  unsigned char uchr = static_cast<unsigned char>(character);
  char lowerChar = static_cast<char>(std::tolower(uchr));
  return lowerChar == 'r' || lowerChar == 'w' || lowerChar == 'u' || 
         lowerChar == 'g' || lowerChar == 'a' || lowerChar == ':';
}

bool UploadAccess::isValidWhoString(const std::string& who) {
  return who == "user" || who == "group" || who == "all";
}

// TODO: verify if can not use StringUtils
bool UploadAccess::isValidPermissionString(const std::string& perms) {
  if (perms.empty()) {
    return false;
  }
  
  for (size_t i = 0; i < perms.length(); ++i) {
    unsigned char uchr = static_cast<unsigned char>(perms[i]);
    char chr = static_cast<char>(std::tolower(uchr));
    if (chr != 'r' && chr != 'w') {
      return false;
    }
  }
  
  bool hasR = false;
  bool hasW = false;
  for (size_t i = 0; i < perms.length(); ++i) {
    unsigned char uchr = static_cast<unsigned char>(perms[i]);
    char chr = static_cast<char>(std::tolower(uchr));
    if (chr == 'r') {
      if (hasR) return false;
      hasR = true;
    } else if (chr == 'w') {
      if (hasW) return false;
      hasW = true;
    }
  }
  
  return true;
}

unsigned int UploadAccess::getBitsForWho(const std::string& who, bool read, bool write) {
  unsigned int bits = 0;
  
  if (who == "user") {
    if (read) bits |= USER_READ_BIT;
    if (write) bits |= USER_WRITE_BIT;
  } else if (who == "group") {
    if (read) bits |= GROUP_READ_BIT;
    if (write) bits |= GROUP_WRITE_BIT;
  } else if (who == "all") {
    if (read) bits |= OTHER_READ_BIT;
    if (write) bits |= OTHER_WRITE_BIT;
  }
  
  return bits;
}

std::string UploadAccess::whoToString(unsigned int bits) {
  if ((bits & (USER_READ_BIT | USER_WRITE_BIT)) != 0U) return "user";
  if ((bits & (GROUP_READ_BIT | GROUP_WRITE_BIT)) != 0U) return "group";
  if ((bits & (OTHER_READ_BIT | OTHER_WRITE_BIT)) != 0U) return "all";
  return "";
}

std::string UploadAccess::permsToString(unsigned int bits) {
  std::string perms;
  if ((bits & (USER_READ_BIT | GROUP_READ_BIT | OTHER_READ_BIT)) != 0U) perms += "r";
  if ((bits & (USER_WRITE_BIT | GROUP_WRITE_BIT | OTHER_WRITE_BIT)) != 0U) perms += "w";
  return perms;
}

char UploadAccess::readToSymbol(bool hasPermission) {
  return hasPermission ? 'r' : '-';
}

char UploadAccess::writeToSymbol(bool hasPermission) {
  return hasPermission ? 'w' : '-';
}

}  // namespace value_objects
}  // namespace filesystem
}  // namespace domain
