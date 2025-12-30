/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_UploadAccess.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 16:14:55 by umeneses          #+#    #+#             */
/*   Updated: 2025/12/29 17:24:23 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/filesystem/value_objects/UploadAccess.hpp"
#include "domain/filesystem/exceptions/UploadAccessException.hpp"

using namespace domain::filesystem::value_objects;
using namespace domain::filesystem::exceptions;

class UploadAccessTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(UploadAccessTest, DefaultConstructor) {
  UploadAccess access;
  // TODO: Fix linker error - static const ODR-use requires definition in UploadAccess.cpp
  // EXPECT_EQ(UploadAccess::USER_READ_WRITE_GROUP_READ_OTHER_READ, access.getPermissionBits());
  EXPECT_EQ(static_cast<unsigned int>(0x17), access.getPermissionBits()); // USER_ALL | GROUP_READ_BIT | OTHER_READ_BIT = 3|4|16 = 23
}

TEST_F(UploadAccessTest, ConstructWithValidBits) {
  EXPECT_NO_THROW(UploadAccess(UploadAccess::NO_PERMISSION));
  EXPECT_NO_THROW(UploadAccess(UploadAccess::READ_ONLY));
  EXPECT_NO_THROW(UploadAccess(UploadAccess::READ_WRITE));
  EXPECT_NO_THROW(UploadAccess(UploadAccess::USER_READ_BIT));
}

TEST_F(UploadAccessTest, ConstructWithInvalidBits) {
  EXPECT_THROW(UploadAccess(999), UploadAccessException);
  EXPECT_THROW(UploadAccess(UploadAccess::ALL_PERMISSIONS + 1), UploadAccessException);
}

TEST_F(UploadAccessTest, ConstructFromValidString) {
  EXPECT_NO_THROW(UploadAccess("user:rw"));
  EXPECT_NO_THROW(UploadAccess("group:r"));
  EXPECT_NO_THROW(UploadAccess("all:w"));
  EXPECT_NO_THROW(UploadAccess("user:rw group:r all:w"));
  EXPECT_NO_THROW(UploadAccess("none"));
}

TEST_F(UploadAccessTest, CopyConstructor) {
  UploadAccess original(UploadAccess::READ_ONLY);
  UploadAccess copy(original);
  EXPECT_EQ(original.getPermissionBits(), copy.getPermissionBits());
}

// ============================================================================
// Assignment Tests
// ============================================================================

TEST_F(UploadAccessTest, AssignmentOperator) {
  UploadAccess access1(UploadAccess::READ_ONLY);
  UploadAccess access2;
  access2 = access1;
  EXPECT_EQ(access1.getPermissionBits(), access2.getPermissionBits());
}

TEST_F(UploadAccessTest, AssignmentOperatorSelfAssignment) {
  UploadAccess access(UploadAccess::READ_WRITE);
  access = access;
  // TODO: Fix linker error - static const ODR-use requires definition in UploadAccess.cpp
  // EXPECT_EQ(UploadAccess::READ_WRITE, access.getPermissionBits());
  EXPECT_EQ(static_cast<unsigned int>(0x3F), access.getPermissionBits()); // ALL_PERMISSIONS
}

// ============================================================================
// Comparison Tests
// ============================================================================

TEST_F(UploadAccessTest, EqualityOperator) {
  UploadAccess access1(UploadAccess::READ_ONLY);
  UploadAccess access2(UploadAccess::READ_ONLY);
  UploadAccess access3(UploadAccess::READ_WRITE);

  EXPECT_TRUE(access1 == access2);
  EXPECT_FALSE(access1 == access3);
}

TEST_F(UploadAccessTest, InequalityOperator) {
  UploadAccess access1(UploadAccess::READ_ONLY);
  UploadAccess access2(UploadAccess::READ_WRITE);

  EXPECT_TRUE(access1 != access2);
  EXPECT_FALSE(access1 != UploadAccess(UploadAccess::READ_ONLY));
}

TEST_F(UploadAccessTest, LessThanOperator) {
  UploadAccess access1(UploadAccess::USER_READ_BIT);
  UploadAccess access2(UploadAccess::READ_WRITE);

  EXPECT_TRUE(access1 < access2);
  EXPECT_FALSE(access2 < access1);
  EXPECT_FALSE(access1 < UploadAccess(UploadAccess::USER_READ_BIT));
}

TEST_F(UploadAccessTest, LessThanOrEqualOperator) {
  UploadAccess access1(UploadAccess::USER_READ_BIT);
  UploadAccess access2(UploadAccess::READ_WRITE);
  UploadAccess access3(UploadAccess::USER_READ_BIT);

  EXPECT_TRUE(access1 <= access2);
  EXPECT_TRUE(access1 <= access3);
  EXPECT_FALSE(access2 <= access1);
}

TEST_F(UploadAccessTest, GreaterThanOperator) {
  UploadAccess access1(UploadAccess::READ_WRITE);
  UploadAccess access2(UploadAccess::USER_READ_BIT);

  EXPECT_TRUE(access1 > access2);
  EXPECT_FALSE(access2 > access1);
  EXPECT_FALSE(access1 > UploadAccess(UploadAccess::READ_WRITE));
}

TEST_F(UploadAccessTest, GreaterThanOrEqualOperator) {
  UploadAccess access1(UploadAccess::READ_WRITE);
  UploadAccess access2(UploadAccess::USER_READ_BIT);
  UploadAccess access3(UploadAccess::READ_WRITE);

  EXPECT_TRUE(access1 >= access2);
  EXPECT_TRUE(access1 >= access3);
  EXPECT_FALSE(access2 >= access1);
}

// ============================================================================
// Validation Tests
// ============================================================================

TEST_F(UploadAccessTest, IsValidPermission) {
  EXPECT_TRUE(UploadAccess::isValidPermission(UploadAccess::NO_PERMISSION));
  EXPECT_TRUE(UploadAccess::isValidPermission(UploadAccess::READ_ONLY));
  EXPECT_TRUE(UploadAccess::isValidPermission(UploadAccess::READ_WRITE));
  EXPECT_TRUE(UploadAccess::isValidPermission(UploadAccess::ALL_PERMISSIONS));
  EXPECT_FALSE(UploadAccess::isValidPermission(999));
  EXPECT_FALSE(UploadAccess::isValidPermission(UploadAccess::ALL_PERMISSIONS + 1));
}

TEST_F(UploadAccessTest, IsValidAccessStringValid) {
  EXPECT_TRUE(UploadAccess::isValidAccessString("user:rw"));
  EXPECT_TRUE(UploadAccess::isValidAccessString("group:r"));
  EXPECT_TRUE(UploadAccess::isValidAccessString("all:w"));
  EXPECT_TRUE(UploadAccess::isValidAccessString("user:rw group:r"));
  EXPECT_TRUE(UploadAccess::isValidAccessString("none"));
}

TEST_F(UploadAccessTest, IsValidAccessStringInvalid) {
  EXPECT_FALSE(UploadAccess::isValidAccessString(""));
  EXPECT_FALSE(UploadAccess::isValidAccessString("user-rw"));
  EXPECT_FALSE(UploadAccess::isValidAccessString("user:rwx"));
  EXPECT_FALSE(UploadAccess::isValidAccessString("invalid:rw"));
  EXPECT_FALSE(UploadAccess::isValidAccessString("user:invalid"));
}

// ============================================================================
// Access Control Tests - User Permissions
// ============================================================================

TEST_F(UploadAccessTest, CanUserRead) {
  UploadAccess access1(UploadAccess::USER_READ_BIT);
  EXPECT_TRUE(access1.canUserRead());
  
  UploadAccess access2(UploadAccess::USER_WRITE_BIT);
  EXPECT_FALSE(access2.canUserRead());
}

TEST_F(UploadAccessTest, CanUserWrite) {
  UploadAccess access1(UploadAccess::USER_WRITE_BIT);
  EXPECT_TRUE(access1.canUserWrite());
  
  UploadAccess access2(UploadAccess::USER_READ_BIT);
  EXPECT_FALSE(access2.canUserWrite());
}

// ============================================================================
// Access Control Tests - Group Permissions
// ============================================================================

TEST_F(UploadAccessTest, CanGroupRead) {
  UploadAccess access1(UploadAccess::GROUP_READ_BIT);
  EXPECT_TRUE(access1.canGroupRead());
  
  UploadAccess access2(UploadAccess::GROUP_WRITE_BIT);
  EXPECT_FALSE(access2.canGroupRead());
}

TEST_F(UploadAccessTest, CanGroupWrite) {
  UploadAccess access1(UploadAccess::GROUP_WRITE_BIT);
  EXPECT_TRUE(access1.canGroupWrite());
  
  UploadAccess access2(UploadAccess::GROUP_READ_BIT);
  EXPECT_FALSE(access2.canGroupWrite());
}

// ============================================================================
// Access Control Tests - Other Permissions
// ============================================================================

TEST_F(UploadAccessTest, CanOtherRead) {
  UploadAccess access1(UploadAccess::OTHER_READ_BIT);
  EXPECT_TRUE(access1.canOtherRead());
  
  UploadAccess access2(UploadAccess::OTHER_WRITE_BIT);
  EXPECT_FALSE(access2.canOtherRead());
}

TEST_F(UploadAccessTest, CanOtherWrite) {
  UploadAccess access1(UploadAccess::OTHER_WRITE_BIT);
  EXPECT_TRUE(access1.canOtherWrite());
  
  UploadAccess access2(UploadAccess::OTHER_READ_BIT);
  EXPECT_FALSE(access2.canOtherWrite());
}

// ============================================================================
// Access Control Tests - Combined Permissions
// ============================================================================

TEST_F(UploadAccessTest, AllPermissionsReadWrite) {
  UploadAccess access(UploadAccess::READ_WRITE);
  
  EXPECT_TRUE(access.canUserRead());
  EXPECT_TRUE(access.canUserWrite());
  EXPECT_TRUE(access.canGroupRead());
  EXPECT_TRUE(access.canGroupWrite());
  EXPECT_TRUE(access.canOtherRead());
  EXPECT_TRUE(access.canOtherWrite());
}

TEST_F(UploadAccessTest, NoPermissions) {
  UploadAccess access(UploadAccess::NO_PERMISSION);
  
  EXPECT_FALSE(access.canUserRead());
  EXPECT_FALSE(access.canUserWrite());
  EXPECT_FALSE(access.canGroupRead());
  EXPECT_FALSE(access.canGroupWrite());
  EXPECT_FALSE(access.canOtherRead());
  EXPECT_FALSE(access.canOtherWrite());
}

// ============================================================================
// Setter Tests - User Permissions
// ============================================================================

TEST_F(UploadAccessTest, SetUserRead) {
  UploadAccess access(UploadAccess::NO_PERMISSION);
  
  access.setUserRead(true);
  EXPECT_TRUE(access.canUserRead());
  
  access.setUserRead(false);
  EXPECT_FALSE(access.canUserRead());
}

TEST_F(UploadAccessTest, SetUserWrite) {
  UploadAccess access(UploadAccess::NO_PERMISSION);
  
  access.setUserWrite(true);
  EXPECT_TRUE(access.canUserWrite());
  
  access.setUserWrite(false);
  EXPECT_FALSE(access.canUserWrite());
}

// ============================================================================
// Setter Tests - Group Permissions
// ============================================================================

TEST_F(UploadAccessTest, SetGroupRead) {
  UploadAccess access(UploadAccess::NO_PERMISSION);
  
  access.setGroupRead(true);
  EXPECT_TRUE(access.canGroupRead());
  
  access.setGroupRead(false);
  EXPECT_FALSE(access.canGroupRead());
}

TEST_F(UploadAccessTest, SetGroupWrite) {
  UploadAccess access(UploadAccess::NO_PERMISSION);
  
  access.setGroupWrite(true);
  EXPECT_TRUE(access.canGroupWrite());
  
  access.setGroupWrite(false);
  EXPECT_FALSE(access.canGroupWrite());
}

// ============================================================================
// Setter Tests - Other Permissions
// ============================================================================

TEST_F(UploadAccessTest, SetOtherRead) {
  UploadAccess access(UploadAccess::NO_PERMISSION);
  
  access.setOtherRead(true);
  EXPECT_TRUE(access.canOtherRead());
  
  access.setOtherRead(false);
  EXPECT_FALSE(access.canOtherRead());
}

TEST_F(UploadAccessTest, SetOtherWrite) {
  UploadAccess access(UploadAccess::NO_PERMISSION);
  
  access.setOtherWrite(true);
  EXPECT_TRUE(access.canOtherWrite());
  
  access.setOtherWrite(false);
  EXPECT_FALSE(access.canOtherWrite());
}

// ============================================================================
// Setter Tests - Sequential Operations
// ============================================================================

TEST_F(UploadAccessTest, SetMultiplePermissions) {
  UploadAccess access(UploadAccess::NO_PERMISSION);
  
  access.setUserRead(true);
  access.setUserWrite(true);
  access.setGroupRead(true);
  
  EXPECT_TRUE(access.canUserRead());
  EXPECT_TRUE(access.canUserWrite());
  EXPECT_TRUE(access.canGroupRead());
  EXPECT_FALSE(access.canGroupWrite());
  EXPECT_FALSE(access.canOtherRead());
  EXPECT_FALSE(access.canOtherWrite());
}

// ============================================================================
// String Parsing Tests
// ============================================================================

TEST_F(UploadAccessTest, ParseAccessStringUserReadWrite) {
  UploadAccess access("user:rw");
  EXPECT_TRUE(access.canUserRead());
  EXPECT_TRUE(access.canUserWrite());
  EXPECT_FALSE(access.canGroupRead());
  EXPECT_FALSE(access.canGroupWrite());
}

TEST_F(UploadAccessTest, ParseAccessStringGroupRead) {
  UploadAccess access("group:r");
  EXPECT_FALSE(access.canUserRead());
  EXPECT_TRUE(access.canGroupRead());
  EXPECT_FALSE(access.canGroupWrite());
}

TEST_F(UploadAccessTest, ParseAccessStringAllWrite) {
  UploadAccess access("all:w");
  EXPECT_FALSE(access.canUserRead());
  EXPECT_FALSE(access.canGroupRead());
  EXPECT_TRUE(access.canOtherWrite());
}

TEST_F(UploadAccessTest, ParseAccessStringMultiple) {
  UploadAccess access("user:rw group:r all:w");
  EXPECT_TRUE(access.canUserRead());
  EXPECT_TRUE(access.canUserWrite());
  EXPECT_TRUE(access.canGroupRead());
  EXPECT_FALSE(access.canGroupWrite());
  EXPECT_FALSE(access.canOtherRead());
  EXPECT_TRUE(access.canOtherWrite());
}

TEST_F(UploadAccessTest, ParseAccessStringNone) {
  UploadAccess access("none");
  // TODO: Fix linker error - static const ODR-use requires definition in UploadAccess.cpp
  // EXPECT_EQ(UploadAccess::NO_PERMISSION, access.getPermissionBits());
  EXPECT_EQ(static_cast<unsigned int>(0), access.getPermissionBits());
}

TEST_F(UploadAccessTest, ParseAccessStringInvalidFormat) {
  EXPECT_THROW(UploadAccess("user-rw"), UploadAccessException);
  EXPECT_THROW(UploadAccess("userrw"), UploadAccessException);
}

TEST_F(UploadAccessTest, ParseAccessStringInvalidWho) {
  EXPECT_THROW(UploadAccess("invalid:rw"), UploadAccessException);
  EXPECT_THROW(UploadAccess("admin:rw"), UploadAccessException);
}

TEST_F(UploadAccessTest, ParseAccessStringInvalidPermission) {
  EXPECT_THROW(UploadAccess("user:rwx"), UploadAccessException);
  EXPECT_THROW(UploadAccess("user:invalid"), UploadAccessException);
  EXPECT_THROW(UploadAccess("user:rrw"), UploadAccessException);
}

TEST_F(UploadAccessTest, ParseAccessStringEmpty) {
  EXPECT_THROW(UploadAccess(""), UploadAccessException);
}

// ============================================================================
// ToString Tests
// ============================================================================

TEST_F(UploadAccessTest, ToStringNoPermissions) {
  UploadAccess access(UploadAccess::NO_PERMISSION);
  EXPECT_EQ("none", access.toString());
}

TEST_F(UploadAccessTest, ToStringUserReadWrite) {
  UploadAccess access(UploadAccess::USER_ALL);
  EXPECT_EQ("user:rw", access.toString());
}

TEST_F(UploadAccessTest, ToStringGroupRead) {
  UploadAccess access(UploadAccess::GROUP_READ_BIT);
  EXPECT_EQ("group:r", access.toString());
}

TEST_F(UploadAccessTest, ToStringAllWrite) {
  UploadAccess access(UploadAccess::OTHER_WRITE_BIT);
  EXPECT_EQ("all:w", access.toString());
}

TEST_F(UploadAccessTest, ToStringMultiplePermissions) {
  UploadAccess access(UploadAccess::NO_PERMISSION);
  access.setUserRead(true);
  access.setUserWrite(true);
  access.setGroupRead(true);
  access.setOtherWrite(true);
  
  std::string result = access.toString();
  EXPECT_NE(result.find("user:rw"), std::string::npos);
  EXPECT_NE(result.find("group:r"), std::string::npos);
  EXPECT_NE(result.find("all:w"), std::string::npos);
}

// ============================================================================
// Factory Method Tests
// ============================================================================

TEST_F(UploadAccessTest, FactoryReadOnly) {
  UploadAccess access = UploadAccess::readOnly();
  EXPECT_TRUE(access.canUserRead());
  EXPECT_FALSE(access.canUserWrite());
  EXPECT_TRUE(access.canGroupRead());
  EXPECT_FALSE(access.canGroupWrite());
  EXPECT_TRUE(access.canOtherRead());
  EXPECT_FALSE(access.canOtherWrite());
}

TEST_F(UploadAccessTest, FactoryReadWrite) {
  UploadAccess access = UploadAccess::readWrite();
  EXPECT_TRUE(access.canUserRead());
  EXPECT_TRUE(access.canUserWrite());
  EXPECT_TRUE(access.canGroupRead());
  EXPECT_TRUE(access.canGroupWrite());
  EXPECT_TRUE(access.canOtherRead());
  EXPECT_TRUE(access.canOtherWrite());
}

TEST_F(UploadAccessTest, FactoryUserReadWriteGroupReadOtherRead) {
  UploadAccess access = UploadAccess::userReadWriteGroupReadOtherRead();
  EXPECT_TRUE(access.canUserRead());
  EXPECT_TRUE(access.canUserWrite());
  EXPECT_TRUE(access.canGroupRead());
  EXPECT_FALSE(access.canGroupWrite());
  EXPECT_TRUE(access.canOtherRead());
  EXPECT_FALSE(access.canOtherWrite());
}

TEST_F(UploadAccessTest, FactoryUserReadWriteGroupReadOtherNone) {
  UploadAccess access = UploadAccess::userReadWriteGroupReadOtherNone();
  EXPECT_TRUE(access.canUserRead());
  EXPECT_TRUE(access.canUserWrite());
  EXPECT_TRUE(access.canGroupRead());
  EXPECT_FALSE(access.canGroupWrite());
  EXPECT_FALSE(access.canOtherRead());
  EXPECT_FALSE(access.canOtherWrite());
}

TEST_F(UploadAccessTest, FactoryFromString) {
  UploadAccess access = UploadAccess::fromString("user:rw group:r");
  EXPECT_TRUE(access.canUserRead());
  EXPECT_TRUE(access.canUserWrite());
  EXPECT_TRUE(access.canGroupRead());
  EXPECT_FALSE(access.canGroupWrite());
}

TEST_F(UploadAccessTest, FactoryFromBits) {
  // TODO: Fix linker error - static const ODR-use requires definition in UploadAccess.cpp
  // UploadAccess access = UploadAccess::fromBits(UploadAccess::READ_ONLY);
  // EXPECT_EQ(UploadAccess::READ_ONLY, access.getPermissionBits());
  UploadAccess access = UploadAccess::fromBits(0x15); // USER_READ_BIT | GROUP_READ_BIT | OTHER_READ_BIT
  EXPECT_EQ(static_cast<unsigned int>(0x15), access.getPermissionBits());
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(UploadAccessTest, GetPermissionBits) {
  // TODO: Fix linker error - static const ODR-use requires definition in UploadAccess.cpp
  // UploadAccess access(UploadAccess::READ_WRITE);
  // EXPECT_EQ(UploadAccess::READ_WRITE, access.getPermissionBits());
  UploadAccess access(0x3F); // ALL_PERMISSIONS
  EXPECT_EQ(static_cast<unsigned int>(0x3F), access.getPermissionBits());
}