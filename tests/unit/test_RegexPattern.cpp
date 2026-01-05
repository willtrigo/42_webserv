/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_RegexPattern.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 16:14:41 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/30 16:18:02 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/shared/value_objects/RegexPattern.hpp"
#include "domain/shared/exceptions/RegexPatternException.hpp"

using namespace domain::shared::value_objects;
using namespace domain::shared::exceptions;

class RegexPatternTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(RegexPatternTest, DefaultConstructor) {
  RegexPattern* pattern = NULL;
  EXPECT_NO_THROW(pattern = new RegexPattern());
  if (pattern != NULL) {
    EXPECT_EQ("", pattern->getPattern());
    EXPECT_EQ(RegexPattern::FLAG_NONE, pattern->getFlags());
    delete pattern;
  }
}

TEST_F(RegexPatternTest, ConstructWithPattern) {
  RegexPattern pattern("test");
  EXPECT_EQ("test", pattern.getPattern());
  EXPECT_EQ(RegexPattern::FLAG_NONE, pattern.getFlags());
}

TEST_F(RegexPatternTest, ConstructWithPatternAndFlags) {
  RegexPattern pattern("test", RegexPattern::FLAG_CASE_INSENSITIVE);
  EXPECT_EQ("test", pattern.getPattern());
  EXPECT_EQ(RegexPattern::FLAG_CASE_INSENSITIVE, pattern.getFlags());
}

TEST_F(RegexPatternTest, CopyConstructor) {
  RegexPattern original("test.*", RegexPattern::FLAG_MULTILINE);
  RegexPattern copy(original);
  
  EXPECT_EQ(original.getPattern(), copy.getPattern());
  EXPECT_EQ(original.getFlags(), copy.getFlags());
}

TEST_F(RegexPatternTest, AssignmentOperator) {
  RegexPattern pattern1("pattern1");
  RegexPattern pattern2("pattern2", RegexPattern::FLAG_CASE_INSENSITIVE);
  
  pattern1 = pattern2;
  EXPECT_EQ("pattern2", pattern1.getPattern());
  EXPECT_EQ(RegexPattern::FLAG_CASE_INSENSITIVE, pattern1.getFlags());
}

TEST_F(RegexPatternTest, SelfAssignment) {
  RegexPattern pattern("test", RegexPattern::FLAG_MULTILINE);
  pattern = pattern;
  
  EXPECT_EQ("test", pattern.getPattern());
  EXPECT_EQ(RegexPattern::FLAG_MULTILINE, pattern.getFlags());
}

// ============================================================================
// Getter Tests
// ============================================================================

TEST_F(RegexPatternTest, GetPattern) {
  RegexPattern pattern("hello.*world");
  EXPECT_EQ("hello.*world", pattern.getPattern());
}

TEST_F(RegexPatternTest, GetFlags) {
  RegexPattern pattern("test", RegexPattern::FLAG_CASE_INSENSITIVE | RegexPattern::FLAG_MULTILINE);
  EXPECT_EQ(RegexPattern::FLAG_CASE_INSENSITIVE | RegexPattern::FLAG_MULTILINE, pattern.getFlags());
}

TEST_F(RegexPatternTest, GetFlagsString) {
  RegexPattern pattern("test", RegexPattern::FLAG_NONE);
  EXPECT_NO_THROW(pattern.getFlagsString());
}

TEST_F(RegexPatternTest, ToString) {
  RegexPattern pattern("test");
  EXPECT_NO_THROW(pattern.toString());
}

TEST_F(RegexPatternTest, GetPatternEmpty) {
  RegexPattern pattern("");
  EXPECT_EQ("", pattern.getPattern());
}

// ============================================================================
// Setter Tests (Flag Management)
// ============================================================================

TEST_F(RegexPatternTest, SetFlagCaseInsensitive) {
  RegexPattern pattern("test");
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
  
  pattern.setFlag(RegexPattern::FLAG_CASE_INSENSITIVE, true);
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
  
  pattern.setFlag(RegexPattern::FLAG_CASE_INSENSITIVE, false);
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
}

TEST_F(RegexPatternTest, SetFlagMultiline) {
  RegexPattern pattern("test");
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_MULTILINE));
  
  pattern.setFlag(RegexPattern::FLAG_MULTILINE, true);
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_MULTILINE));
}

TEST_F(RegexPatternTest, SetFlagDotall) {
  RegexPattern pattern("test");
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_DOTALL));
  
  pattern.setFlag(RegexPattern::FLAG_DOTALL, true);
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_DOTALL));
}

TEST_F(RegexPatternTest, SetFlagExtended) {
  RegexPattern pattern("test");
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_EXTENDED));
  
  pattern.setFlag(RegexPattern::FLAG_EXTENDED, true);
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_EXTENDED));
}

TEST_F(RegexPatternTest, SetMultipleFlags) {
  RegexPattern pattern("test");
  
  pattern.setFlag(RegexPattern::FLAG_CASE_INSENSITIVE, true);
  pattern.setFlag(RegexPattern::FLAG_MULTILINE, true);
  
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_MULTILINE));
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_DOTALL));
}

// ============================================================================
// Validation Tests
// ============================================================================

TEST_F(RegexPatternTest, IsValidPatternSimple) {
  EXPECT_TRUE(RegexPattern::isValidPattern("test"));
  EXPECT_TRUE(RegexPattern::isValidPattern("hello.*world"));
  EXPECT_TRUE(RegexPattern::isValidPattern("[a-z]+"));
}

TEST_F(RegexPatternTest, IsValidPatternEmpty) {
  EXPECT_FALSE(RegexPattern::isValidPattern(""));
}

TEST_F(RegexPatternTest, IsValidPatternUnbalancedBrackets) {
  EXPECT_FALSE(RegexPattern::isValidPattern("[unclosed"));
  EXPECT_FALSE(RegexPattern::isValidPattern("unopened]"));
}

TEST_F(RegexPatternTest, IsValidPatternTooLong) {
  std::string longPattern(RegexPattern::MAX_PATTERN_LENGTH + 1, 'a');
  EXPECT_FALSE(RegexPattern::isValidPattern(longPattern));
}

TEST_F(RegexPatternTest, IsSimplePatternLiteral) {
  EXPECT_TRUE(RegexPattern::isSimplePattern("test"));
  EXPECT_TRUE(RegexPattern::isSimplePattern("hello"));
  EXPECT_TRUE(RegexPattern::isSimplePattern("file.txt"));
}

TEST_F(RegexPatternTest, IsSimplePatternComplex) {
  EXPECT_FALSE(RegexPattern::isSimplePattern("test.*"));
  EXPECT_FALSE(RegexPattern::isSimplePattern("[a-z]+"));
  EXPECT_FALSE(RegexPattern::isSimplePattern("hello|world"));
}

TEST_F(RegexPatternTest, NeedsEscapingSimple) {
  EXPECT_FALSE(RegexPattern::needsEscaping("test"));
  EXPECT_FALSE(RegexPattern::needsEscaping("hello"));
}

TEST_F(RegexPatternTest, NeedsEscapingSpecialCharacters) {
  EXPECT_TRUE(RegexPattern::needsEscaping("test*"));
  EXPECT_TRUE(RegexPattern::needsEscaping("hello.world"));
  EXPECT_TRUE(RegexPattern::needsEscaping("[test]"));
}

// ============================================================================
// Pattern Matching Tests
// ============================================================================

TEST_F(RegexPatternTest, MatchesExactString) {
  RegexPattern pattern("hello");
  EXPECT_TRUE(pattern.matches("hello"));
  EXPECT_FALSE(pattern.matches("world"));
}

TEST_F(RegexPatternTest, MatchesCaseInsensitive) {
  RegexPattern pattern("hello", RegexPattern::FLAG_CASE_INSENSITIVE);
  EXPECT_TRUE(pattern.matches("HELLO"));
  EXPECT_TRUE(pattern.matches("Hello"));
  EXPECT_TRUE(pattern.matches("hello"));
}

TEST_F(RegexPatternTest, MatchesWildcard) {
  RegexPattern pattern("test.*");
  EXPECT_TRUE(pattern.matches("test"));
  EXPECT_TRUE(pattern.matches("test123"));
  EXPECT_TRUE(pattern.matches("testABC"));
}

TEST_F(RegexPatternTest, MatchesCharacterClass) {
  RegexPattern pattern("[0-9]+");
  EXPECT_TRUE(pattern.matches("123"));
  EXPECT_TRUE(pattern.matches("456"));
  EXPECT_FALSE(pattern.matches("abc"));
}

TEST_F(RegexPatternTest, MatchesAlternation) {
  RegexPattern pattern("jpg|jpeg|png");
  EXPECT_TRUE(pattern.matches("jpg"));
  EXPECT_TRUE(pattern.matches("jpeg"));
  EXPECT_TRUE(pattern.matches("png"));
  EXPECT_FALSE(pattern.matches("gif"));
}

TEST_F(RegexPatternTest, MatchesAnchor) {
  RegexPattern pattern("^test$");
  EXPECT_TRUE(pattern.matches("test"));
  EXPECT_FALSE(pattern.matches("test123"));
  EXPECT_FALSE(pattern.matches("123test"));
}

// ============================================================================
// Utility Tests
// ============================================================================

TEST_F(RegexPatternTest, EscapeSimpleText) {
  std::string escaped = RegexPattern::escape("test");
  EXPECT_EQ("test", escaped);
}

TEST_F(RegexPatternTest, EscapeSpecialCharacters) {
  std::string escaped = RegexPattern::escape("test.*");
  EXPECT_NE("test.*", escaped);
  EXPECT_TRUE(escaped.find("\\") != std::string::npos);
}

TEST_F(RegexPatternTest, EscapeMultipleSpecialCharacters) {
  std::string escaped = RegexPattern::escape("[a-z].*+");
  EXPECT_TRUE(escaped.find("\\") != std::string::npos);
}

TEST_F(RegexPatternTest, MakeCaseInsensitive) {
  RegexPattern pattern("test");
  std::string insensitive = pattern.makeCaseInsensitive();
  EXPECT_NE("test", insensitive);
}

TEST_F(RegexPatternTest, MakeLiteral) {
  RegexPattern pattern("test.*");
  std::string literal = pattern.makeLiteral();
  EXPECT_NE("test.*", literal);
}

// ============================================================================
// Flag Tests
// ============================================================================

TEST_F(RegexPatternTest, HasFlagNone) {
  RegexPattern pattern("test", RegexPattern::FLAG_NONE);
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_MULTILINE));
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_DOTALL));
}

TEST_F(RegexPatternTest, HasFlagSingle) {
  RegexPattern pattern("test", RegexPattern::FLAG_CASE_INSENSITIVE);
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_MULTILINE));
}

TEST_F(RegexPatternTest, HasFlagMultiple) {
  RegexPattern pattern("test", RegexPattern::FLAG_CASE_INSENSITIVE | RegexPattern::FLAG_MULTILINE);
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_MULTILINE));
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_DOTALL));
}

TEST_F(RegexPatternTest, ToggleFlag) {
  RegexPattern pattern("test");
  
  pattern.setFlag(RegexPattern::FLAG_CASE_INSENSITIVE, true);
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
  
  pattern.setFlag(RegexPattern::FLAG_CASE_INSENSITIVE, false);
  EXPECT_FALSE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
  
  pattern.setFlag(RegexPattern::FLAG_CASE_INSENSITIVE, true);
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
}

// ============================================================================
// Factory Method Tests
// ============================================================================

TEST_F(RegexPatternTest, FromString) {
  RegexPattern pattern = RegexPattern::fromString("test");
  EXPECT_EQ("test", pattern.getPattern());
  EXPECT_EQ(RegexPattern::FLAG_NONE, pattern.getFlags());
}

TEST_F(RegexPatternTest, FromStringWithFlags) {
  RegexPattern pattern = RegexPattern::fromString("test", RegexPattern::FLAG_CASE_INSENSITIVE);
  EXPECT_EQ("test", pattern.getPattern());
  EXPECT_EQ(RegexPattern::FLAG_CASE_INSENSITIVE, pattern.getFlags());
}

TEST_F(RegexPatternTest, LiteralFactory) {
  RegexPattern pattern = RegexPattern::literal("test.*");
  std::string patternStr = pattern.getPattern();
  EXPECT_NE("test.*", patternStr);
  EXPECT_TRUE(patternStr.find("\\") != std::string::npos || patternStr != "test.*");
}

TEST_F(RegexPatternTest, CaseInsensitiveFactory) {
  RegexPattern pattern = RegexPattern::caseInsensitive("test");
  EXPECT_TRUE(pattern.hasFlag(RegexPattern::FLAG_CASE_INSENSITIVE));
  EXPECT_TRUE(pattern.matches("TEST"));
  EXPECT_TRUE(pattern.matches("test"));
}

TEST_F(RegexPatternTest, PhpExtensionFactory) {
  RegexPattern pattern = RegexPattern::phpExtension();
  EXPECT_TRUE(pattern.matches("php"));
  EXPECT_FALSE(pattern.matches("txt"));
}

TEST_F(RegexPatternTest, PythonExtensionFactory) {
  RegexPattern pattern = RegexPattern::pythonExtension();
  EXPECT_TRUE(pattern.matches("py"));
  EXPECT_FALSE(pattern.matches("txt"));
}

TEST_F(RegexPatternTest, ImageExtensionsFactory) {
  RegexPattern pattern = RegexPattern::imageExtensions();
  EXPECT_TRUE(pattern.matches("jpg") || pattern.matches("jpeg") || 
              pattern.matches("png") || pattern.matches("gif"));
}

// ============================================================================
// Comparison Operator Tests
// ============================================================================

TEST_F(RegexPatternTest, EqualityOperator) {
  RegexPattern pattern1("test");
  RegexPattern pattern2("test");
  RegexPattern pattern3("other");
  
  EXPECT_TRUE(pattern1 == pattern2);
  EXPECT_FALSE(pattern1 == pattern3);
}

TEST_F(RegexPatternTest, InequalityOperator) {
  RegexPattern pattern1("test");
  RegexPattern pattern2("other");
  RegexPattern pattern3("test");
  
  EXPECT_TRUE(pattern1 != pattern2);
  EXPECT_FALSE(pattern1 != pattern3);
}

TEST_F(RegexPatternTest, LessThanOperator) {
  RegexPattern pattern1("aaa");
  RegexPattern pattern2("bbb");
  
  EXPECT_TRUE(pattern1 < pattern2);
  EXPECT_FALSE(pattern2 < pattern1);
}

TEST_F(RegexPatternTest, EqualityWithDifferentFlags) {
  RegexPattern pattern1("test", RegexPattern::FLAG_CASE_INSENSITIVE);
  RegexPattern pattern2("test", RegexPattern::FLAG_MULTILINE);
  
  EXPECT_FALSE(pattern1 == pattern2);
}
