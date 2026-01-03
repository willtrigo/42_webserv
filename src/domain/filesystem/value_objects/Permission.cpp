/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Permission.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 00:12:32 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/03 16:28:38 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/filesystem/value_objects/Permission.hpp"
#include "domain/filesystem/exceptions/PermissionException.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>

namespace domain {
namespace filesystem {
namespace value_objects {

Permission::Permission()
    : m_permission(Permission::OWNER_READ_WRITE_GROUP_READ_OTHER_READ) {}

Permission::Permission(unsigned int octalPermission)
    : m_permission(octalPermission) {
  validate();
}

Permission::Permission(const std::string& permissionString)
    : m_permission(Permission::OWNER_READ_WRITE_GROUP_READ_OTHER_READ) {
  m_permission = parsePermissionString(permissionString);
  validate();
}

Permission::Permission(const Permission& other)
    : m_permission(other.m_permission) {}

Permission::~Permission() {}

Permission& Permission::operator=(const Permission& other) {
  if (this != &other) {
    m_permission = other.m_permission;
  }
  return *this;
}

unsigned int Permission::getOctalValue() const { return m_permission; }

std::string Permission::toString() const {
  std::ostringstream oss;
  oss << std::oct << m_permission;
  std::string result = oss.str();

  const std::size_t MIN_OCTAL_DIGITS = 4;
  while (result.length() < MIN_OCTAL_DIGITS) {
    result = "0" + result;
  }

  return result;
}

std::string Permission::toSymbolicString() const {
  std::string symbolic;

  symbolic += ((m_permission & SETUID_BIT) != 0U) ? 'S' : '-';
  symbolic += ((m_permission & SETGID_BIT) != 0U) ? 'S' : '-';
  symbolic += ((m_permission & STICKY_BIT) != 0U) ? 'T' : '-';
  symbolic += ' ';

  symbolic += canRead(CLASS_OWNER) ? 'r' : '-';
  symbolic += canWrite(CLASS_OWNER) ? 'w' : '-';
  symbolic += canExecute(CLASS_OWNER) ? 'x' : '-';

  symbolic += canRead(CLASS_GROUP) ? 'r' : '-';
  symbolic += canWrite(CLASS_GROUP) ? 'w' : '-';
  symbolic += canExecute(CLASS_GROUP) ? 'x' : '-';

  symbolic += canRead(CLASS_OTHER) ? 'r' : '-';
  symbolic += canWrite(CLASS_OTHER) ? 'w' : '-';
  symbolic += canExecute(CLASS_OTHER) ? 'x' : '-';

  return symbolic;
}

bool Permission::isValidPermission(unsigned int permission) {
  return permission <= MAX_PERMISSION;
}

bool Permission::isValidPermissionString(const std::string& permissionString) {
  try {
    parsePermissionString(permissionString);
    return true;
  } catch (const exceptions::PermissionException&) {
    return false;
  }
}

bool Permission::isValidSymbolicString(const std::string& symbolicString) {
  try {
    parseSymbolicString(symbolicString);
    return true;
  } catch (const exceptions::PermissionException&) {
    return false;
  }
}

void Permission::validate() const {
  if (!isValidPermission(m_permission)) {
    std::ostringstream oss;
    oss << "Permission value out of range: 0" << std::oct << m_permission
        << " (valid range: 0-07777)";
    throw exceptions::PermissionException(
        oss.str(), exceptions::PermissionException::OUT_OF_RANGE);
  }
}

bool Permission::canRead(Class permissionClass) const {
  if (permissionClass == CLASS_ALL) {
    return canRead(CLASS_OWNER) && canRead(CLASS_GROUP) && canRead(CLASS_OTHER);
  }

  unsigned int shift = getClassShift(permissionClass);
  return ((m_permission >> shift) & READ_BIT) != 0U;
}

bool Permission::canWrite(Class permissionClass) const {
  if (permissionClass == CLASS_ALL) {
    return canWrite(CLASS_OWNER) && canWrite(CLASS_GROUP) &&
           canWrite(CLASS_OTHER);
  }

  unsigned int shift = getClassShift(permissionClass);
  return ((m_permission >> shift) & WRITE_BIT) != 0U;
}

bool Permission::canExecute(Class permissionClass) const {
  if (permissionClass == CLASS_ALL) {
    return canExecute(CLASS_OWNER) && canExecute(CLASS_GROUP) &&
           canExecute(CLASS_OTHER);
  }

  unsigned int shift = getClassShift(permissionClass);
  return ((m_permission >> shift) & EXECUTE_BIT) != 0U;
}

bool Permission::hasSetUid() const { return (m_permission & SETUID_BIT) != 0U; }

bool Permission::hasSetGid() const { return (m_permission & SETGID_BIT) != 0U; }

bool Permission::hasStickyBit() const {
  return (m_permission & STICKY_BIT) != 0U;
}

void Permission::setRead(Class permissionClass, bool enable) {
  unsigned int mask = getBitMask(permissionClass, MODE_READ);
  if (enable) {
    m_permission |= mask;
  } else {
    m_permission &= ~mask;
  }
}

void Permission::setWrite(Class permissionClass, bool enable) {
  unsigned int mask = getBitMask(permissionClass, MODE_WRITE);
  if (enable) {
    m_permission |= mask;
  } else {
    m_permission &= ~mask;
  }
}

void Permission::setExecute(Class permissionClass, bool enable) {
  unsigned int mask = getBitMask(permissionClass, MODE_EXECUTE);
  if (enable) {
    m_permission |= mask;
  } else {
    m_permission &= ~mask;
  }
}

void Permission::setSetUid(bool enable) {
  if (enable) {
    m_permission |= SETUID_BIT;
  } else {
    m_permission &= ~SETUID_BIT;
  }
}

void Permission::setSetGid(bool enable) {
  if (enable) {
    m_permission |= SETGID_BIT;
  } else {
    m_permission &= ~SETGID_BIT;
  }
}

void Permission::setStickyBit(bool enable) {
  if (enable) {
    m_permission |= STICKY_BIT;
  } else {
    m_permission &= ~STICKY_BIT;
  }
}

bool Permission::operator==(const Permission& other) const {
  return m_permission == other.m_permission;
}

bool Permission::operator!=(const Permission& other) const {
  return m_permission != other.m_permission;
}

bool Permission::operator<(const Permission& other) const {
  return m_permission < other.m_permission;
}

bool Permission::operator<=(const Permission& other) const {
  return m_permission <= other.m_permission;
}

bool Permission::operator>(const Permission& other) const {
  return m_permission > other.m_permission;
}

bool Permission::operator>=(const Permission& other) const {
  return m_permission >= other.m_permission;
}

Permission Permission::fromString(const std::string& permissionString) {
  return Permission(permissionString);
}

Permission Permission::fromSymbolicString(const std::string& symbolicString) {
  unsigned int permission = parseSymbolicString(symbolicString);
  return Permission(permission);
}

Permission Permission::fromOctal(unsigned int octalValue) {
  return Permission(octalValue);
}

Permission Permission::readOnly() { return Permission(READ_ONLY); }

Permission Permission::readWrite() { return Permission(READ_WRITE); }

Permission Permission::readWriteExecute() {
  return Permission(ALL_PERMISSIONS);
}

Permission Permission::ownerReadWriteGroupReadOtherRead() {
  return Permission(OWNER_READ_WRITE_GROUP_READ_OTHER_READ);
}

Permission Permission::ownerReadWriteGroupReadOtherNone() {
  return Permission(OWNER_READ_WRITE_GROUP_READ);
}

Permission Permission::ownerAllGroupReadExecuteOtherReadExecute() {
  return Permission(OWNER_ALL_GROUP_READ_EXECUTE_OTHER_READ_EXECUTE);
}

unsigned int Permission::parsePermissionString(
    const std::string& permissionString) {
  if (permissionString.empty()) {
    throw exceptions::PermissionException(
        "Permission string cannot be empty",
        exceptions::PermissionException::EMPTY_STRING);
  }

  if (std::isalpha(static_cast<unsigned char>(permissionString[0])) != 0) {
    return parseSymbolicString(permissionString);
  }

  if (!isValidOctalString(permissionString)) {
    throw exceptions::PermissionException(
        "Invalid permission format: '" + permissionString + "'",
        exceptions::PermissionException::INVALID_FORMAT);
  }

  char* endPointer = NULL;
  unsigned long result =
      std::strtoul(permissionString.c_str(), &endPointer, BASE_OCTAL);

  if (endPointer == permissionString.c_str() || *endPointer != '\0') {
    throw exceptions::PermissionException(
        "Failed to convert permission string: '" + permissionString + "'",
        exceptions::PermissionException::CONVERSION_FAILED);
  }

  if (result > static_cast<unsigned long>(MAX_PERMISSION)) {
    std::ostringstream oss;
    oss << "Permission value out of range: '" << permissionString
        << "' (max: 07777)";
    throw exceptions::PermissionException(
        oss.str(), exceptions::PermissionException::OUT_OF_RANGE);
  }

  return static_cast<unsigned int>(result);
}

unsigned int Permission::parseSymbolicString(
    const std::string& symbolicString) {
  if (symbolicString.empty()) {
    throw exceptions::PermissionException(
        "Symbolic permission string cannot be empty",
        exceptions::PermissionException::EMPTY_STRING);
  }

  const bool hasSpecial = hasSpecialBitsFormat(symbolicString);
  const std::size_t expectedLength =
      hasSpecial ? SYMBOLIC_STRING_WITH_SPECIAL_LENGTH : SYMBOLIC_STRING_LENGTH;

  if (symbolicString.length() != expectedLength) {
    std::ostringstream oss;
    oss << "Symbolic permission must be " << expectedLength << " characters: '"
        << symbolicString << "'";
    throw exceptions::PermissionException(
        oss.str(), exceptions::PermissionException::INVALID_FORMAT);
  }

  unsigned int permission = 0;
  std::size_t startIndex = 0;

  if (hasSpecial) {
    permission |= parseSpecialBits(symbolicString);
    startIndex = SPECIAL_BITS_START_INDEX;
  }

  permission |= parseRegularPermissions(symbolicString, startIndex);

  return permission;
}

bool Permission::hasSpecialBitsFormat(const std::string& symbolicString) {
  return symbolicString.length() ==
         Permission::SYMBOLIC_STRING_WITH_SPECIAL_LENGTH;
}

unsigned int Permission::parseSpecialBits(const std::string& symbolicString) {
  // Validate special bits format: "XYZ " where X,Y,Z are S/s/T/t/-
  char setuidChar = symbolicString[0];
  char setgidChar = symbolicString[1];
  char stickyChar = symbolicString[2];
  char spaceChar = symbolicString[3];
  
  // Validate each position
  if (setuidChar != 'S' && setuidChar != 's' && setuidChar != '-') {
    throw exceptions::PermissionException(
        "Invalid setuid character: expected 'S', 's', or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (setgidChar != 'S' && setgidChar != 's' && setgidChar != '-') {
    throw exceptions::PermissionException(
        "Invalid setgid character: expected 'S', 's', or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (stickyChar != 'T' && stickyChar != 't' && stickyChar != '-') {
    throw exceptions::PermissionException(
        "Invalid sticky bit character: expected 'T', 't', or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (spaceChar != ' ') {
    throw exceptions::PermissionException(
        "Invalid special bits format: expected space after special bits",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  
  // Build special bits
  unsigned int specialBits = 0;

  if (setuidChar == 'S' || setuidChar == 's') {
    specialBits |= Permission::SETUID_BIT;
  }

  if (setgidChar == 'S' || setgidChar == 's') {
    specialBits |= Permission::SETGID_BIT;
  }

  if (stickyChar == 'T' || stickyChar == 't') {
    specialBits |= Permission::STICKY_BIT;
  }

  return specialBits;
}

unsigned int Permission::parseRegularPermissions(
    const std::string& symbolicString, std::size_t startIndex) {
  // Extract all characters
  char ownerRead = symbolicString[startIndex + OWNER_READ_POSITION];
  char ownerWrite = symbolicString[startIndex + OWNER_WRITE_POSITION];
  char ownerExecute = symbolicString[startIndex + OWNER_EXECUTE_POSITION];
  char groupRead = symbolicString[startIndex + GROUP_READ_POSITION];
  char groupWrite = symbolicString[startIndex + GROUP_WRITE_POSITION];
  char groupExecute = symbolicString[startIndex + GROUP_EXECUTE_POSITION];
  char otherRead = symbolicString[startIndex + OTHER_READ_POSITION];
  char otherWrite = symbolicString[startIndex + OTHER_WRITE_POSITION];
  char otherExecute = symbolicString[startIndex + OTHER_EXECUTE_POSITION];

  // Validate all positions
  if (ownerRead != 'r' && ownerRead != '-') {
    throw exceptions::PermissionException(
        "Invalid character in owner read position: expected 'r' or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (ownerWrite != 'w' && ownerWrite != '-') {
    throw exceptions::PermissionException(
        "Invalid character in owner write position: expected 'w' or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (ownerExecute != 'x' && ownerExecute != 's' && ownerExecute != 'S' && ownerExecute != '-') {
    throw exceptions::PermissionException(
        "Invalid character in owner execute position: expected 'x', 's', 'S', or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (groupRead != 'r' && groupRead != '-') {
    throw exceptions::PermissionException(
        "Invalid character in group read position: expected 'r' or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (groupWrite != 'w' && groupWrite != '-') {
    throw exceptions::PermissionException(
        "Invalid character in group write position: expected 'w' or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (groupExecute != 'x' && groupExecute != 's' && groupExecute != 'S' && groupExecute != '-') {
    throw exceptions::PermissionException(
        "Invalid character in group execute position: expected 'x', 's', 'S', or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (otherRead != 'r' && otherRead != '-') {
    throw exceptions::PermissionException(
        "Invalid character in other read position: expected 'r' or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (otherWrite != 'w' && otherWrite != '-') {
    throw exceptions::PermissionException(
        "Invalid character in other write position: expected 'w' or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }
  if (otherExecute != 'x' && otherExecute != 't' && otherExecute != 'T' && otherExecute != '-') {
    throw exceptions::PermissionException(
        "Invalid character in other execute position: expected 'x', 't', 'T', or '-'",
        exceptions::PermissionException::INVALID_FORMAT);
  }

  // Build permission bits
  unsigned int permissions = 0;
  
  if (ownerRead == 'r')
    permissions |= (READ_BIT << OWNER_PERMISSION_SHIFT);
  if (ownerWrite == 'w')
    permissions |= (WRITE_BIT << OWNER_PERMISSION_SHIFT);
  if (ownerExecute == 'x' || ownerExecute == 's')
    permissions |= (EXECUTE_BIT << OWNER_PERMISSION_SHIFT);
    
  if (groupRead == 'r')
    permissions |= (READ_BIT << GROUP_PERMISSION_SHIFT);
  if (groupWrite == 'w')
    permissions |= (WRITE_BIT << GROUP_PERMISSION_SHIFT);
  if (groupExecute == 'x' || groupExecute == 's')
    permissions |= (EXECUTE_BIT << GROUP_PERMISSION_SHIFT);
    
  if (otherRead == 'r')
    permissions |= READ_BIT;
  if (otherWrite == 'w')
    permissions |= WRITE_BIT;
  if (otherExecute == 'x' || otherExecute == 't')
    permissions |= EXECUTE_BIT;

  return permissions;
}

bool Permission::isOctalDigit(char character) {
  return character >= '0' && character <= '7';
}

bool Permission::isValidOctalString(const std::string& octalString) {
  for (std::size_t index = 0; index < octalString.length(); ++index) {
    if (!isOctalDigit(octalString[index])) {
      return false;
    }
  }
  return !octalString.empty();
}

bool Permission::isPermissionCharacter(char character) {
  unsigned char uchr = static_cast<unsigned char>(character);
  char lowerChar = static_cast<char>(std::tolower(uchr));
  return lowerChar == 'r' || lowerChar == 'w' || lowerChar == 'x' ||
         lowerChar == 's' || lowerChar == 't' || lowerChar == '-';
}

unsigned int Permission::getBitMask(Class permissionClass, Mode mode) {
  unsigned int bit;
  switch (mode) {
    case MODE_READ:
      bit = READ_BIT;
      break;
    case MODE_WRITE:
      bit = WRITE_BIT;
      break;
    case MODE_EXECUTE:
      bit = EXECUTE_BIT;
      break;
    default:
      return 0;
  }

  if (permissionClass == CLASS_ALL) {
    return (bit << OWNER_PERMISSION_SHIFT) | (bit << GROUP_PERMISSION_SHIFT) |
           bit;
  }

  unsigned int shift = getClassShift(permissionClass);
  return bit << shift;
}

unsigned int Permission::getClassShift(Class permissionClass) {
  switch (permissionClass) {
    case CLASS_OWNER:
      return OWNER_PERMISSION_SHIFT;
    case CLASS_GROUP:
      return GROUP_PERMISSION_SHIFT;
    case CLASS_OTHER:
      return OTHER_PERMISSION_SHIFT;
    default:
      return 0;
  }
}

char Permission::modeToSymbol(Mode mode, bool hasPermission) {
  if (!hasPermission) return '-';

  switch (mode) {
    case MODE_READ:
      return 'r';
    case MODE_WRITE:
      return 'w';
    case MODE_EXECUTE:
      return 'x';
    default:
      return '-';
  }
}

std::string Permission::classToString(Class permissionClass) {
  switch (permissionClass) {
    case CLASS_OWNER:
      return "owner";
    case CLASS_GROUP:
      return "group";
    case CLASS_OTHER:
      return "other";
    case CLASS_ALL:
      return "all";
    default:
      return "unknown";
  }
}

}  // namespace value_objects
}  // namespace domain
}  // namespace domain
