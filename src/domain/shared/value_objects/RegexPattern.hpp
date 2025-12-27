/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RegexPattern.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 10:51:25 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/27 20:03:11 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REGEX_PATTERN_HPP
#define REGEX_PATTERN_HPP

#include <string>

namespace domain {
namespace shared {
namespace exceptions {

class RegexPattern {
 public:
  static const std::size_t MAX_PATTERN_LENGTH = 4096;

  enum Flag {
    FLAG_NONE = 0,
    FLAG_CASE_INSENSITIVE = 1 << 0,
    FLAG_MULTILINE = 1 << 1,
    FLAG_DOTALL = 1 << 2,
    FLAG_EXTENDED = 1 << 3
  };

  static const std::string SPECIAL_CHARACTERS;
  static const std::string CHARACTER_CLASS_SPECIAL;

  static const std::string PHP_EXTENSION;
  static const std::string PYTHON_EXTENSION;
  static const std::string IMAGE_EXTENSIONS;
  static const std::string NUMERIC_PATTERN;
  static const std::string ALPHANUMERIC_PATTERN;
  static const std::string EMAIL_PATTERN;
  static const std::string URL_PATTERN;

  RegexPattern();
  explicit RegexPattern(const std::string& pattern, int flags = FLAG_NONE);
  RegexPattern(const RegexPattern& other);
  ~RegexPattern();

  RegexPattern& operator=(const RegexPattern& other);

  std::string getPattern() const;
  std::string toString() const;
  int getFlags() const;
  std::string getFlagsString() const;

  static bool isValidPattern(const std::string& pattern);
  static bool isSimplePattern(const std::string& pattern);
  static bool needsEscaping(const std::string& text);

  bool matches(const std::string& text) const;
  static std::string escape(const std::string& text);
  std::string makeCaseInsensitive() const;
  std::string makeLiteral() const;

  bool hasFlag(Flag flag) const;
  void setFlag(Flag flag, bool enable);

  bool operator==(const RegexPattern& other) const;
  bool operator!=(const RegexPattern& other) const;
  bool operator<(const RegexPattern& other) const;

  static RegexPattern fromString(const std::string& pattern,
                                 int flags = FLAG_NONE);
  static RegexPattern literal(const std::string& text);
  static RegexPattern caseInsensitive(const std::string& pattern);

  static RegexPattern phpExtension();
  static RegexPattern pythonExtension();
  static RegexPattern imageExtensions();
  static RegexPattern numeric();
  static RegexPattern alphanumeric();

  bool isEmpty() const;

  std::size_t length() const;

 private:
  std::string m_pattern;
  int m_flags;

  void validate() const;
  static void validatePattern(const std::string& pattern);

  static bool hasBalancedBrackets(const std::string& pattern);
  static bool hasBalancedParentheses(const std::string& pattern);
  static bool hasBalancedBraces(const std::string& pattern);
  static bool hasValidEscapeSequences(const std::string& pattern);

  static bool hasUnescapedSpecialChars(
      const std::string& regexPatternToCheck,
      const std::string& specialCharactersList);
  static void updateParserState(bool& escapeFlag, char currentChar,
                                bool& insideCharacterClassFlag);
  static bool checkInsideCharacterClass(char currentChar,
                                        const std::string& regexPatternToCheck,
                                        std::size_t currentPosition);
  static bool checkOutsideCharacterClass(
      char currentChar, const std::string& regexPatternToCheck,
      std::size_t currentPosition, const std::string& specialCharactersList);

  static std::string escapeString(const std::string& text, size_t textLength,
                                  const std::string& specialChars);
  static std::string addFlagModifier(const std::string& pattern, Flag flag);
  static std::string flagToString(Flag flag);
  static bool isSpecialCharacter(char character,
                                 const std::string& specialChars);
};

}  // namespace exceptions
}  // namespace shared
}  // namespace domain

#endif  // REGEX_PATTERN_HPP
