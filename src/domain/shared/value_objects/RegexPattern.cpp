/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RegexPattern.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 10:55:26 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/04 22:04:40 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/shared/exceptions/RegexPatternException.hpp"
#include "domain/shared/value_objects/RegexPattern.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <regex.h>
#include <sstream>

namespace domain {
namespace shared {
namespace value_objects {

namespace {
const char BACKSLASH = '\\';
const char OPEN_BRACKET = '[';
const char CLOSE_BRACKET = ']';
const char OPEN_PAREN = '(';
const char CLOSE_PAREN = ')';
const char OPEN_BRACE = '{';
const char CLOSE_BRACE = '}';
const char CARET = '^';
const char LOWERCASE_I = 'i';
const char QUESTION_MARK = '?';
const char HYPHEN = '-';

const std::size_t MIN_FLAG_MODIFIER_LENGTH = 3;
const std::size_t MIN_EXISTING_FLAG_LENGTH = 4;
const std::size_t FLAG_MODIFIER_PREFIX_LENGTH = 2;
const int BALANCE_ZERO = 0;
const int BALANCE_NEGATIVE_THRESHOLD = 0;
}  // namespace

const std::string RegexPattern::SPECIAL_CHARACTERS = "\\^$.|?*+()[]{}";
const std::string RegexPattern::CHARACTER_CLASS_SPECIAL = "\\^-]";

const std::string RegexPattern::PHP_EXTENSION = "(^php$|\\.php$)";
const std::string RegexPattern::PYTHON_EXTENSION = "(^py$|\\.py$)";
const std::string RegexPattern::IMAGE_EXTENSIONS =
    "(^(jpg|jpeg|png|gif|bmp)$|\\.(jpg|jpeg|png|gif|bmp)$)";
const std::string RegexPattern::NUMERIC_PATTERN = "^[0-9]+$";
const std::string RegexPattern::ALPHANUMERIC_PATTERN = "^[a-zA-Z0-9]+$";
const std::string RegexPattern::EMAIL_PATTERN =
    "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
const std::string RegexPattern::URL_PATTERN =
    "^https?://[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}(/.*)?$";

RegexPattern::RegexPattern() : m_flags(FLAG_NONE) {}

RegexPattern::RegexPattern(const std::string& pattern, int flags)
    : m_pattern(pattern), m_flags(flags) {
  validate();
}

RegexPattern::RegexPattern(const RegexPattern& other)
    : m_pattern(other.m_pattern), m_flags(other.m_flags) {}

RegexPattern::~RegexPattern() {}

RegexPattern& RegexPattern::operator=(const RegexPattern& other) {
  if (this != &other) {
    m_pattern = other.m_pattern;
    m_flags = other.m_flags;
  }
  return *this;
}

std::string RegexPattern::getPattern() const { return m_pattern; }

std::string RegexPattern::toString() const { return m_pattern; }

int RegexPattern::getFlags() const { return m_flags; }

std::string RegexPattern::getFlagsString() const {
  std::string flagsStr;

  if ((m_flags & FLAG_CASE_INSENSITIVE) != 0) {
    if (!flagsStr.empty()) flagsStr += "|";
    flagsStr += "CASE_INSENSITIVE";
  }
  if ((m_flags & FLAG_MULTILINE) != 0) {
    if (!flagsStr.empty()) flagsStr += "|";
    flagsStr += "MULTILINE";
  }
  if ((m_flags & FLAG_DOTALL) != 0) {
    if (!flagsStr.empty()) flagsStr += "|";
    flagsStr += "DOTALL";
  }
  if ((m_flags & FLAG_EXTENDED) != 0) {
    if (!flagsStr.empty()) flagsStr += "|";
    flagsStr += "EXTENDED";
  }

  if (flagsStr.empty()) {
    flagsStr = "NONE";
  }

  return flagsStr;
}

bool RegexPattern::isValidPattern(const std::string& pattern) {
  if (pattern.empty()) {
    return false;
  }
  try {
    validatePattern(pattern);
    return true;
  } catch (const shared::exceptions::RegexPatternException&) {
    return false;
  }
}

bool RegexPattern::isSimplePattern(const std::string& pattern) {
  for (std::size_t index = 0; index < pattern.length(); ++index) {
    if (pattern[index] == BACKSLASH) {
      ++index;
      continue;
    }

    if (isSpecialCharacter(pattern[index], SPECIAL_CHARACTERS)) {
      return false;
    }
  }
  return true;
}

bool RegexPattern::needsEscaping(const std::string& text) {
  for (std::size_t index = 0; index < text.length(); ++index) {
    if (isSpecialCharacter(text[index], SPECIAL_CHARACTERS)) {
      return true;
    }
  }
  return false;
}

void RegexPattern::validate() const {
  validatePattern(m_pattern);

  if (m_pattern.length() > MAX_PATTERN_LENGTH) {
    std::ostringstream oss;
    oss << "Pattern too long: " << m_pattern.length()
        << " characters (max: " << MAX_PATTERN_LENGTH << ")";
    throw shared::exceptions::RegexPatternException(
        oss.str(), shared::exceptions::RegexPatternException::PATTERN_TOO_LONG);
  }
}

bool RegexPattern::matches(const std::string& text) const {
  if (m_pattern.empty()) {
    return text.empty();
  }

  if (isSimplePattern(m_pattern)) {
    if (hasFlag(FLAG_CASE_INSENSITIVE)) {
      std::string patternLower = m_pattern;
      std::string textLower = text;
      std::transform(patternLower.begin(), patternLower.end(),
                     patternLower.begin(), ::tolower);
      std::transform(textLower.begin(), textLower.end(), textLower.begin(),
                     ::tolower);
      return patternLower == textLower;
    }
    return m_pattern == text;
  }

  regex_t regex;
  int cflags = REG_EXTENDED | REG_NOSUB;

  if (hasFlag(FLAG_CASE_INSENSITIVE)) {
    cflags |= REG_ICASE;
  }
  if (hasFlag(FLAG_MULTILINE)) {
    cflags |= REG_NEWLINE;
  }

  int compileResult = regcomp(&regex, m_pattern.c_str(), cflags);
  if (compileResult != 0) {
    return false;
  }

  int matchResult = regexec(&regex, text.c_str(), 0, NULL, 0);
  regfree(&regex);

  return matchResult == 0;
}

std::string RegexPattern::escape(const std::string& text) {
  return escapeString(text, text.length(), SPECIAL_CHARACTERS);
}

std::string RegexPattern::makeCaseInsensitive() const {
  return addFlagModifier(m_pattern, FLAG_CASE_INSENSITIVE);
}

std::string RegexPattern::makeLiteral() const {
  return escapeString(m_pattern, m_pattern.length(), SPECIAL_CHARACTERS);
}

bool RegexPattern::hasFlag(Flag flag) const { return (m_flags & flag) != 0; }

void RegexPattern::setFlag(Flag flag, bool enable) {
  if (enable) {
    m_flags |= flag;
  } else {
    m_flags &= ~flag;
  }
}

bool RegexPattern::operator==(const RegexPattern& other) const {
  return m_pattern == other.m_pattern && m_flags == other.m_flags;
}

bool RegexPattern::operator!=(const RegexPattern& other) const {
  return !(*this == other);
}

bool RegexPattern::operator<(const RegexPattern& other) const {
  if (m_pattern != other.m_pattern) {
    return m_pattern < other.m_pattern;
  }
  return m_flags < other.m_flags;
}

RegexPattern RegexPattern::fromString(const std::string& pattern, int flags) {
  return RegexPattern(pattern, flags);
}

RegexPattern RegexPattern::literal(const std::string& text) {
  std::string escaped = escapeString(text, text.length(), SPECIAL_CHARACTERS);
  return RegexPattern(escaped);
}

RegexPattern RegexPattern::caseInsensitive(const std::string& pattern) {
  return RegexPattern(pattern, FLAG_CASE_INSENSITIVE);
}

RegexPattern RegexPattern::phpExtension() {
  return RegexPattern(PHP_EXTENSION);
}

RegexPattern RegexPattern::pythonExtension() {
  return RegexPattern(PYTHON_EXTENSION);
}

RegexPattern RegexPattern::imageExtensions() {
  return RegexPattern(IMAGE_EXTENSIONS);
}

RegexPattern RegexPattern::numeric() { return RegexPattern(NUMERIC_PATTERN); }

RegexPattern RegexPattern::alphanumeric() {
  return RegexPattern(ALPHANUMERIC_PATTERN);
}

bool RegexPattern::isEmpty() const { return m_pattern.empty(); }

std::size_t RegexPattern::length() const { return m_pattern.length(); }

void RegexPattern::validatePattern(const std::string& pattern) {
  if (pattern.empty()) {
    throw shared::exceptions::RegexPatternException(
        "Pattern cannot be empty",
        shared::exceptions::RegexPatternException::EMPTY_PATTERN);
  }
  if (pattern.length() > MAX_PATTERN_LENGTH) {
    throw shared::exceptions::RegexPatternException(
        "Pattern too long: '" + pattern + "'",
        shared::exceptions::RegexPatternException::PATTERN_TOO_LONG);
  }

  if (!hasBalancedBrackets(pattern)) {
    throw shared::exceptions::RegexPatternException(
        "Unbalanced brackets in pattern: '" + pattern + "'",
        shared::exceptions::RegexPatternException::UNBALANCED_BRACKETS);
  }

  if (!hasBalancedParentheses(pattern)) {
    throw shared::exceptions::RegexPatternException(
        "Unbalanced parentheses in pattern: '" + pattern + "'",
        shared::exceptions::RegexPatternException::UNBALANCED_PARENTHESES);
  }

  if (!hasBalancedBraces(pattern)) {
    throw shared::exceptions::RegexPatternException(
        "Unbalanced braces in pattern: '" + pattern + "'",
        shared::exceptions::RegexPatternException::UNBALANCED_BRACES);
  }

  if (!hasValidEscapeSequences(pattern)) {
    throw shared::exceptions::RegexPatternException(
        "Invalid escape sequence in pattern: '" + pattern + "'",
        shared::exceptions::RegexPatternException::INVALID_ESCAPE);
  }
}

bool RegexPattern::hasBalancedBrackets(const std::string& pattern) {
  int balance = BALANCE_ZERO;
  bool inEscape = false;

  for (std::size_t index = 0; index < pattern.length(); ++index) {
    char currentChar = pattern[index];

    if (inEscape) {
      inEscape = false;
      continue;
    }

    if (currentChar == BACKSLASH) {
      inEscape = true;
      continue;
    }

    if (currentChar == OPEN_BRACKET) {
      ++balance;
    } else if (currentChar == CLOSE_BRACKET) {
      --balance;
      if (balance < BALANCE_NEGATIVE_THRESHOLD) {
        return false;
      }
    }
  }

  return balance == BALANCE_ZERO;
}

bool RegexPattern::hasBalancedParentheses(const std::string& pattern) {
  int balance = BALANCE_ZERO;
  bool inEscape = false;
  bool inCharacterClass = false;

  for (std::size_t index = 0; index < pattern.length(); ++index) {
    char currentChar = pattern[index];

    if (inEscape) {
      inEscape = false;
      continue;
    }

    if (currentChar == BACKSLASH) {
      inEscape = true;
      continue;
    }

    if (currentChar == OPEN_BRACKET) {
      inCharacterClass = true;
    } else if (currentChar == CLOSE_BRACKET) {
      inCharacterClass = false;
    }

    if (!inCharacterClass) {
      if (currentChar == OPEN_PAREN) {
        ++balance;
      } else if (currentChar == CLOSE_PAREN) {
        --balance;
        if (balance < BALANCE_NEGATIVE_THRESHOLD) {
          return false;
        }
      }
    }
  }

  return balance == BALANCE_ZERO;
}

bool RegexPattern::hasBalancedBraces(const std::string& pattern) {
  int balance = BALANCE_ZERO;
  bool inEscape = false;
  bool inCharacterClass = false;

  for (std::size_t index = 0; index < pattern.length(); ++index) {
    char currentChar = pattern[index];

    if (inEscape) {
      inEscape = false;
      continue;
    }

    if (currentChar == BACKSLASH) {
      inEscape = true;
      continue;
    }

    if (currentChar == OPEN_BRACKET) {
      inCharacterClass = true;
    } else if (currentChar == CLOSE_BRACKET) {
      inCharacterClass = false;
    }

    if (!inCharacterClass) {
      if (currentChar == OPEN_BRACE) {
        ++balance;
      } else if (currentChar == CLOSE_BRACE) {
        --balance;
        if (balance < BALANCE_NEGATIVE_THRESHOLD) {
          return false;
        }
      }
    }
  }

  return balance == BALANCE_ZERO;
}

bool RegexPattern::hasValidEscapeSequences(const std::string& pattern) {
  for (std::size_t index = 0; index < pattern.length(); ++index) {
    if (pattern[index] == BACKSLASH) {
      if (index + 1 >= pattern.length()) {
        return false;
      }

      ++index;
    }
  }
  return true;
}

bool RegexPattern::hasUnescapedSpecialChars(
    const std::string& regexPatternToCheck,
    const std::string& specialCharactersList) {
  bool escapeFlag = false;
  bool insideCharacterClassFlag = false;

  for (std::size_t position = 0; position < regexPatternToCheck.length();
       ++position) {
    char currentChar = regexPatternToCheck[position];

    if (escapeFlag) {
      escapeFlag = false;
      continue;
    }

    if (currentChar == BACKSLASH) {
      escapeFlag = true;
      continue;
    }

    if (currentChar == OPEN_BRACKET) {
      insideCharacterClassFlag = true;
      continue;
    }

    if (currentChar == CLOSE_BRACKET && insideCharacterClassFlag) {
      insideCharacterClassFlag = false;
      continue;
    }

    if (insideCharacterClassFlag) {
      if (checkInsideCharacterClass(currentChar, regexPatternToCheck,
                                    position)) {
        return true;
      }
    } else {
      if (checkOutsideCharacterClass(currentChar, regexPatternToCheck, position,
                                     specialCharactersList)) {
        return true;
      }
    }
  }

  return false;
}

void RegexPattern::updateParserState(bool& escapeFlag, char currentChar,
                                     bool& insideCharacterClassFlag) {
  if (escapeFlag) {
    escapeFlag = false;
    return;
  }

  if (currentChar == BACKSLASH) {
    escapeFlag = true;
    return;
  }

  if (currentChar == OPEN_BRACKET) {
    insideCharacterClassFlag = true;
    return;
  }

  if (currentChar == CLOSE_BRACKET && insideCharacterClassFlag) {
    insideCharacterClassFlag = false;
    return;
  }
}

bool RegexPattern::checkInsideCharacterClass(
    char currentChar, const std::string& regexPatternToCheck,
    std::size_t currentPosition) {
  if (!isSpecialCharacter(currentChar, CHARACTER_CLASS_SPECIAL)) {
    return false;
  }

  if (currentChar == CARET && currentPosition > 0 &&
      regexPatternToCheck[currentPosition - 1] == OPEN_BRACKET) {
    return false;
  }

  return (currentPosition == 0 ||
          regexPatternToCheck[currentPosition - 1] != BACKSLASH);
}

bool RegexPattern::checkOutsideCharacterClass(
    char currentChar, const std::string& regexPatternToCheck,
    std::size_t currentPosition, const std::string& specialCharactersList) {
  if (!isSpecialCharacter(currentChar, specialCharactersList)) {
    return false;
  }

  return (currentPosition == 0 ||
          regexPatternToCheck[currentPosition - 1] != BACKSLASH);
}

std::string RegexPattern::escapeString(const std::string& text,
                                       std::size_t textLength,
                                       const std::string& specialChars) {
  std::string escaped;

  for (std::size_t index = 0; index < textLength; ++index) {
    char currentChar = text[index];

    if (isSpecialCharacter(currentChar, specialChars)) {
      escaped += BACKSLASH;
    }
    escaped += currentChar;
  }

  return escaped;
}

std::string RegexPattern::addFlagModifier(const std::string& pattern,
                                          Flag flag) {
  if (pattern.empty()) {
    return pattern;
  }

  if (flag == FLAG_CASE_INSENSITIVE) {
    if (pattern.length() >= MIN_EXISTING_FLAG_LENGTH &&
        pattern[0] == OPEN_PAREN && pattern[1] == QUESTION_MARK &&
        pattern[2] == LOWERCASE_I && pattern[3] == CLOSE_PAREN) {
      return pattern;
    }

    if (pattern.length() >= MIN_FLAG_MODIFIER_LENGTH &&
        pattern[0] == OPEN_PAREN && pattern[1] == QUESTION_MARK) {
      std::size_t pos = FLAG_MODIFIER_PREFIX_LENGTH;
      while (pos < pattern.length() && pattern[pos] != CLOSE_PAREN &&
             ((pattern[pos] >= 'a' && pattern[pos] <= 'z') ||
              pattern[pos] == HYPHEN)) {
        if (pattern[pos] == LOWERCASE_I) {
          return pattern;
        }
        ++pos;
      }

      if (pos < pattern.length() && pattern[pos] == CLOSE_PAREN) {
        std::string newPattern = "(?i";
        newPattern += pattern.substr(FLAG_MODIFIER_PREFIX_LENGTH,
                                     pos - FLAG_MODIFIER_PREFIX_LENGTH);
        newPattern += CLOSE_PAREN;
        newPattern += pattern.substr(pos + 1);
        return newPattern;
      }
    }

    return "(?i)" + pattern;
  }

  return pattern;
}

std::string RegexPattern::flagToString(Flag flag) {
  switch (flag) {
    case FLAG_NONE:
      return "NONE";
    case FLAG_CASE_INSENSITIVE:
      return "CASE_INSENSITIVE";
    case FLAG_MULTILINE:
      return "MULTILINE";
    case FLAG_DOTALL:
      return "DOTALL";
    case FLAG_EXTENDED:
      return "EXTENDED";
    default:
      return "UNKNOWN";
  }
}

bool RegexPattern::isSpecialCharacter(char character,
                                      const std::string& specialChars) {
  return specialChars.find(character) != std::string::npos;
}

}  // namespace value_objects
}  // namespace shared
}  // namespace domain
