/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Size.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 20:02:08 by bira              #+#    #+#             */
/*   Updated: 2025/12/29 12:52:08 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/filesystem/value_objects/Size.hpp"
#include "domain/filesystem/exceptions/SizeException.hpp"

using domain::filesystem::value_objects::Size;

class SizeTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Basic Construction Tests
// ============================================================================

TEST_F(SizeTest, DefaultConstructor) {
  Size size;
  EXPECT_EQ(0u, size.getBytes());
}

TEST_F(SizeTest, ConstructWithBytes) {
  Size size(1024);
  EXPECT_EQ(1024u, size.getBytes());
}

TEST_F(SizeTest, CopyConstructor) {
  Size original(2048);
  Size copy(original);
  EXPECT_EQ(original.getBytes(), copy.getBytes());
}

TEST_F(SizeTest, AssignmentOperator) {
  Size size1(1024);
  Size size2(2048);
  size2 = size1;
  EXPECT_EQ(size1.getBytes(), size2.getBytes());
}

// ============================================================================
// String Parsing Tests - The Core TDD Requirements
// ============================================================================

TEST_F(SizeTest, ValidSizeBytes) {
  Size size("1024");
  EXPECT_EQ(1024u, size.getBytes());
}

TEST_F(SizeTest, ValidSizeBytesWithB) {
  Size size("1024B");
  EXPECT_EQ(1024u, size.getBytes());
}

TEST_F(SizeTest, ValidSizeKilobytes) {
  Size size("1K");
  EXPECT_EQ(1024u, size.getBytes());
}

TEST_F(SizeTest, ValidSizeKilobytesLarge) {
  Size size("512K");
  EXPECT_EQ(512u * 1024u, size.getBytes());
}

TEST_F(SizeTest, ValidSizeMegabytes) {
  Size size("1M");
  EXPECT_EQ(1048576u, size.getBytes());
}

TEST_F(SizeTest, ValidSizeMegabytesLarge) {
  Size size("10M");
  EXPECT_EQ(10u * 1024u * 1024u, size.getBytes());
}

TEST_F(SizeTest, ValidSizeGigabytes) {
  Size size("1G");
  EXPECT_EQ(1024u * 1024u * 1024u, size.getBytes());
}

TEST_F(SizeTest, InvalidFormat) {
  EXPECT_THROW(Size("abc"), domain::filesystem::exceptions::SizeException);
}

TEST_F(SizeTest, InvalidFormatWithInvalidUnit) {
  EXPECT_THROW(Size("123X"), domain::filesystem::exceptions::SizeException);
}

TEST_F(SizeTest, NegativeSize) {
  EXPECT_THROW(Size("-100"), domain::filesystem::exceptions::SizeException);
}

TEST_F(SizeTest, EmptyString) {
  EXPECT_THROW(Size(""), domain::filesystem::exceptions::SizeException);
}

TEST_F(SizeTest, OnlyUnit) {
  EXPECT_THROW(Size("K"), domain::filesystem::exceptions::SizeException);
}

TEST_F(SizeTest, FloatingPoint) {
  EXPECT_THROW(Size("1.5M"), domain::filesystem::exceptions::SizeException);
}

// ============================================================================
// Conversion Tests
// ============================================================================

TEST_F(SizeTest, GetKilobytes) {
  Size size(2048);
  EXPECT_EQ(2u, size.getKilobytes());
}

TEST_F(SizeTest, GetMegabytes) {
  Size size(2097152);  // 2 * 1024 * 1024
  EXPECT_EQ(2u, size.getMegabytes());
}

TEST_F(SizeTest, GetGigabytes) {
  Size size(2147483648u);  // 2 * 1024 * 1024 * 1024
  EXPECT_EQ(2u, size.getGigabytes());
}

// ============================================================================
// Static Factory Methods
// ============================================================================

TEST_F(SizeTest, FromString) {
  Size size = Size::fromString("1M");
  EXPECT_EQ(1048576u, size.getBytes());
}

TEST_F(SizeTest, FromKilobytes) {
  Size size = Size::fromKilobytes(1);
  EXPECT_EQ(1024u, size.getBytes());
}

TEST_F(SizeTest, FromMegabytes) {
  Size size = Size::fromMegabytes(1);
  EXPECT_EQ(1048576u, size.getBytes());
}

TEST_F(SizeTest, FromGigabytes) {
  Size size = Size::fromGigabytes(1);
  EXPECT_EQ(1073741824u, size.getBytes());
}

TEST_F(SizeTest, Zero) {
  Size size = Size::zero();
  EXPECT_EQ(0u, size.getBytes());
  EXPECT_TRUE(size.isZero());
}

TEST_F(SizeTest, Kilobyte) {
  Size size = Size::kilobyte();
  EXPECT_EQ(1024u, size.getBytes());
}

TEST_F(SizeTest, Megabyte) {
  Size size = Size::megabyte();
  EXPECT_EQ(1048576u, size.getBytes());
}

TEST_F(SizeTest, Gigabyte) {
  Size size = Size::gigabyte();
  EXPECT_EQ(1073741824u, size.getBytes());
}

// ============================================================================
// Validation Tests
// ============================================================================

TEST_F(SizeTest, IsValidSizeString) {
  EXPECT_TRUE(Size::isValidSizeString("1024"));
  EXPECT_TRUE(Size::isValidSizeString("1K"));
  EXPECT_TRUE(Size::isValidSizeString("1M"));
  EXPECT_TRUE(Size::isValidSizeString("1G"));
  EXPECT_FALSE(Size::isValidSizeString("abc"));
  EXPECT_FALSE(Size::isValidSizeString("-100"));
  EXPECT_FALSE(Size::isValidSizeString(""));
}

// ============================================================================
// Comparison Operators
// ============================================================================

TEST_F(SizeTest, EqualityOperator) {
  Size size1(1024);
  Size size2(1024);
  Size size3(2048);
  
  EXPECT_TRUE(size1 == size2);
  EXPECT_FALSE(size1 == size3);
}

TEST_F(SizeTest, InequalityOperator) {
  Size size1(1024);
  Size size2(2048);
  
  EXPECT_TRUE(size1 != size2);
  EXPECT_FALSE(size1 != size1);
}

TEST_F(SizeTest, LessThanOperator) {
  Size size1(1024);
  Size size2(2048);
  
  EXPECT_TRUE(size1 < size2);
  EXPECT_FALSE(size2 < size1);
  EXPECT_FALSE(size1 < size1);
}

TEST_F(SizeTest, LessThanOrEqualOperator) {
  Size size1(1024);
  Size size2(2048);
  Size size3(1024);
  
  EXPECT_TRUE(size1 <= size2);
  EXPECT_TRUE(size1 <= size3);
  EXPECT_FALSE(size2 <= size1);
}

TEST_F(SizeTest, GreaterThanOperator) {
  Size size1(2048);
  Size size2(1024);
  
  EXPECT_TRUE(size1 > size2);
  EXPECT_FALSE(size2 > size1);
  EXPECT_FALSE(size1 > size1);
}

TEST_F(SizeTest, GreaterThanOrEqualOperator) {
  Size size1(2048);
  Size size2(1024);
  Size size3(2048);
  
  EXPECT_TRUE(size1 >= size2);
  EXPECT_TRUE(size1 >= size3);
  EXPECT_FALSE(size2 >= size1);
}

// ============================================================================
// Arithmetic Operators
// ============================================================================

TEST_F(SizeTest, AdditionOperator) {
  Size size1(1024);
  Size size2(2048);
  Size result = size1 + size2;
  
  EXPECT_EQ(3072u, result.getBytes());
}

TEST_F(SizeTest, SubtractionOperator) {
  Size size1(2048);
  Size size2(1024);
  Size result = size1 - size2;
  
  EXPECT_EQ(1024u, result.getBytes());
}

TEST_F(SizeTest, AdditionAssignmentOperator) {
  Size size1(1024);
  Size size2(2048);
  size1 += size2;
  
  EXPECT_EQ(3072u, size1.getBytes());
}

TEST_F(SizeTest, SubtractionAssignmentOperator) {
  Size size1(2048);
  Size size2(1024);
  size1 -= size2;
  
  EXPECT_EQ(1024u, size1.getBytes());
}

TEST_F(SizeTest, SubtractionUnderflow) {
  Size size1(1024);
  Size size2(2048);
  
  EXPECT_THROW(size1 - size2, domain::filesystem::exceptions::SizeException);
}

// ============================================================================
// ToString Tests
// ============================================================================

TEST_F(SizeTest, ToStringBytes) {
  Size size(1024);
  std::string result = size.toString();
  EXPECT_FALSE(result.empty());
}

TEST_F(SizeTest, ToStringWithUnit) {
  Size size(1024);
  std::string result = size.toString(Size::UNIT_KILOBYTES);
  EXPECT_FALSE(result.empty());
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(SizeTest, ZeroSize) {
  Size size(0);
  EXPECT_TRUE(size.isZero());
  EXPECT_EQ(0u, size.getBytes());
}

TEST_F(SizeTest, LargeSize) {
  // Test with a large but valid size
  std::size_t largeBytes = 1024u * 1024u * 1024u;  // 1GB
  Size size(largeBytes);
  EXPECT_EQ(largeBytes, size.getBytes());
}
