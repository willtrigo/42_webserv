/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Permission.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 15:37:33 by umeneses          #+#    #+#             */
/*   Updated: 2026/01/03 16:16:12 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "domain/filesystem/value_objects/Permission.hpp"
#include "domain/filesystem/exceptions/PermissionException.hpp"

using namespace domain::filesystem::value_objects;
class PermissionTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(PermissionTest, DefaultConstructor) {
  Permission permission;
  // Default is 0644 (OWNER_READ_WRITE_GROUP_READ_OTHER_READ)
  EXPECT_EQ(0644u, permission.getOctalValue());
}

TEST_F(PermissionTest, ConstructWithValidOctalPermission) {
  Permission permission(0755);
  EXPECT_EQ(0755u, permission.getOctalValue());
}

TEST_F(PermissionTest, WellKnownPermissions) {
  EXPECT_NO_THROW(Permission(0755));
  EXPECT_NO_THROW(Permission(0644));
  EXPECT_NO_THROW(Permission(0777));
  EXPECT_NO_THROW(Permission(0000));
}

TEST_F(PermissionTest, InvalidPermissionTooHigh) {
  // Maximum valid is 07777 (isValidPermission checks <= MAX_PERMISSION)
  EXPECT_THROW(Permission(010000), domain::filesystem::exceptions::PermissionException);  // 07777 + 1
  EXPECT_THROW(Permission(077777), domain::filesystem::exceptions::PermissionException);
}

TEST_F(PermissionTest, ValidPermissionBoundary) {
  // 07777 is the maximum valid permission - should NOT throw
  EXPECT_NO_THROW(Permission(07777));
  // 010000 is 07777 + 1 - should throw
  EXPECT_THROW(Permission(010000), domain::filesystem::exceptions::PermissionException);
}

TEST_F(PermissionTest, CopyConstructor) {
  Permission original(0755);
  Permission copy(original);
  EXPECT_EQ(original.getOctalValue(), copy.getOctalValue());
}

TEST_F(PermissionTest, AssignmentOperator) {
  Permission perm1(0755);
  Permission perm2(0644);
  
  perm2 = perm1;
  EXPECT_EQ(perm1.getOctalValue(), perm2.getOctalValue());
}

TEST_F(PermissionTest, AssignmentOperatorSelfAssignment) {
  Permission perm(0755);
  perm = perm;
  EXPECT_EQ(0755u, perm.getOctalValue());
}


// ============================================================================
// String Parsing Tests
// ============================================================================
TEST_F(PermissionTest, ConstructFromOctalString) {
  Permission perm("0755");
  EXPECT_EQ(0755u, perm.getOctalValue());
}

TEST_F(PermissionTest, ConstructFromSymbolicString) {
  Permission perm("rwxr-xr-x");  // 0755
  EXPECT_EQ(0755u, perm.getOctalValue());
}

TEST_F(PermissionTest, ParseInvalidString) {
  EXPECT_THROW(Permission("invalid"), domain::filesystem::exceptions::PermissionException);
}


// ===========================================================================
// Permission Bits Tests
// ============================================================================
TEST_F(PermissionTest, CanReadOwner) {
  Permission perm(0400);
  EXPECT_TRUE(perm.canRead(Permission::CLASS_OWNER));
  EXPECT_FALSE(perm.canRead(Permission::CLASS_GROUP));
  EXPECT_FALSE(perm.canRead(Permission::CLASS_OTHER));
}

TEST_F(PermissionTest, CanWriteOwner) {
  Permission perm(0200);
  EXPECT_TRUE(perm.canWrite(Permission::CLASS_OWNER));
  EXPECT_FALSE(perm.canWrite(Permission::CLASS_GROUP));
  EXPECT_FALSE(perm.canWrite(Permission::CLASS_OTHER));
}

TEST_F(PermissionTest, CanExecuteOwner) {
  Permission perm(0100);
  EXPECT_TRUE(perm.canExecute(Permission::CLASS_OWNER));
  EXPECT_FALSE(perm.canExecute(Permission::CLASS_GROUP));
  EXPECT_FALSE(perm.canExecute(Permission::CLASS_OTHER));
}

TEST_F(PermissionTest, CanReadGroup) {
  Permission perm(0040);
  EXPECT_FALSE(perm.canRead(Permission::CLASS_OWNER));
  EXPECT_TRUE(perm.canRead(Permission::CLASS_GROUP));
  EXPECT_FALSE(perm.canRead(Permission::CLASS_OTHER));
}

TEST_F(PermissionTest, CanWriteGroup) {
  Permission perm(0020);
  EXPECT_FALSE(perm.canWrite(Permission::CLASS_OWNER));
  EXPECT_TRUE(perm.canWrite(Permission::CLASS_GROUP));
  EXPECT_FALSE(perm.canWrite(Permission::CLASS_OTHER));
}

TEST_F(PermissionTest, CanExecuteGroup) {
  Permission perm(0010);
  EXPECT_FALSE(perm.canExecute(Permission::CLASS_OWNER));
  EXPECT_TRUE(perm.canExecute(Permission::CLASS_GROUP));
  EXPECT_FALSE(perm.canExecute(Permission::CLASS_OTHER));
}

TEST_F(PermissionTest, CanReadOther) {
  Permission perm(0004);
  EXPECT_FALSE(perm.canRead(Permission::CLASS_OWNER));
  EXPECT_FALSE(perm.canRead(Permission::CLASS_GROUP));
  EXPECT_TRUE(perm.canRead(Permission::CLASS_OTHER));
}

TEST_F(PermissionTest, CanWriteOther) {
  Permission perm(0002);
  EXPECT_FALSE(perm.canWrite(Permission::CLASS_OWNER));
  EXPECT_FALSE(perm.canWrite(Permission::CLASS_GROUP));
  EXPECT_TRUE(perm.canWrite(Permission::CLASS_OTHER));
}

TEST_F(PermissionTest, CanExecuteOther) {
  Permission perm(0001);
  EXPECT_TRUE(perm.canExecute(Permission::CLASS_OTHER));
  EXPECT_FALSE(perm.canExecute(Permission::CLASS_OWNER));
  EXPECT_FALSE(perm.canExecute(Permission::CLASS_GROUP));
}

TEST_F(PermissionTest, CombinedPermissions) {
  Permission perm(0755);
  // Owner: rwx (7)
  EXPECT_TRUE(perm.canRead(Permission::CLASS_OWNER));
  EXPECT_TRUE(perm.canWrite(Permission::CLASS_OWNER));
  EXPECT_TRUE(perm.canExecute(Permission::CLASS_OWNER));
  // Group: r-x (5)
  EXPECT_TRUE(perm.canRead(Permission::CLASS_GROUP));
  EXPECT_FALSE(perm.canWrite(Permission::CLASS_GROUP));
  EXPECT_TRUE(perm.canExecute(Permission::CLASS_GROUP));
  // Other: r-x (5)
  EXPECT_TRUE(perm.canRead(Permission::CLASS_OTHER));
  EXPECT_FALSE(perm.canWrite(Permission::CLASS_OTHER));
  EXPECT_TRUE(perm.canExecute(Permission::CLASS_OTHER));
}

TEST_F(PermissionTest, CanReadClassAll) {
  Permission perm(0444);  // All classes have read
  EXPECT_TRUE(perm.canRead(Permission::CLASS_ALL));
  
  Permission perm2(0622);
  EXPECT_FALSE(perm2.canRead(Permission::CLASS_ALL));
}

TEST_F(PermissionTest, CanWriteClassAll) {
  Permission perm(0222);  // All classes have write
  EXPECT_TRUE(perm.canWrite(Permission::CLASS_ALL));
  
  Permission perm2(0644);  // Not all have write
  EXPECT_FALSE(perm2.canWrite(Permission::CLASS_ALL));
}

TEST_F(PermissionTest, CanExecuteClassAll) {
  Permission perm(0111);  // All classes have execute
  EXPECT_TRUE(perm.canExecute(Permission::CLASS_ALL));
  
  Permission perm2(0722);  // Not all have execute
  EXPECT_FALSE(perm2.canExecute(Permission::CLASS_ALL));
}


// ===========================================================================
// Special Bits Tests
// ===========================================================================
TEST_F(PermissionTest, SetUidBit) {
  Permission perm(04755);
  EXPECT_TRUE(perm.hasSetUid());
}

TEST_F(PermissionTest, SetGidBit) {
  Permission perm(02755);
  EXPECT_TRUE(perm.hasSetGid());
}

TEST_F(PermissionTest, StickyBit) {
  Permission perm(01777);
  EXPECT_TRUE(perm.hasStickyBit());
}


// ===========================================================================
// Permission Validation Tests
// ===========================================================================
TEST_F(PermissionTest, IsValidPermission) {
  EXPECT_TRUE(Permission::isValidPermission(0755));
  EXPECT_TRUE(Permission::isValidPermission(0644));
  EXPECT_TRUE(Permission::isValidPermission(0000));
  EXPECT_TRUE(Permission::isValidPermission(07777));
  EXPECT_FALSE(Permission::isValidPermission(010000));
  EXPECT_FALSE(Permission::isValidPermission(077777));
}

TEST_F(PermissionTest, IsValidPermissionString) {
  EXPECT_TRUE(Permission::isValidPermissionString("0755"));
  EXPECT_TRUE(Permission::isValidPermissionString("0644"));
  EXPECT_TRUE(Permission::isValidPermissionString("0000"));
  EXPECT_TRUE(Permission::isValidPermissionString("07777"));
  EXPECT_FALSE(Permission::isValidPermissionString(""));
  EXPECT_FALSE(Permission::isValidPermissionString("0788"));  // 8 not octal
  EXPECT_FALSE(Permission::isValidPermissionString("0999"));  // 9 not octal
  EXPECT_FALSE(Permission::isValidPermissionString("010000"));  // Too high
  EXPECT_FALSE(Permission::isValidPermissionString("invalid"));
}

TEST_F(PermissionTest, IsValidSymbolicString) {
  EXPECT_TRUE(Permission::isValidSymbolicString("rwxr-xr-x"));
  EXPECT_TRUE(Permission::isValidSymbolicString("---------"));
  EXPECT_TRUE(Permission::isValidSymbolicString("rwxrwxrwx"));
  EXPECT_FALSE(Permission::isValidSymbolicString(""));
  EXPECT_FALSE(Permission::isValidSymbolicString("rwx"));  // Too short
  EXPECT_FALSE(Permission::isValidSymbolicString("rwxrwxrwxrwx"));  // Too long
  EXPECT_FALSE(Permission::isValidSymbolicString("invalid"));
  EXPECT_FALSE(Permission::isValidSymbolicString("abcdefghi"));
}

TEST_F(PermissionTest, IsValidSymbolicStringWithSpecialBits) {
  // 10 character format: "SST rwxr-xr-x"
  EXPECT_TRUE(Permission::isValidSymbolicString("SST rwxr-xr-x"));
  EXPECT_TRUE(Permission::isValidSymbolicString("--- rwxr-xr-x"));
  EXPECT_TRUE(Permission::isValidSymbolicString("S-- rwxr-xr-x"));
}

TEST_F(PermissionTest, ValidationEdgeCases) {
  // Whitespace
  EXPECT_FALSE(Permission::isValidPermissionString(" 0755"));
  EXPECT_FALSE(Permission::isValidPermissionString("0755 "));
  
  // Negative values represented as unsigned will be very large
  // So this tests boundary correctly
}


// ===========================================================================
// Operation Tests
// ===========================================================================
TEST_F(PermissionTest, SetReadPermissionAdd) {
  Permission perm(0000);
  perm.setRead(Permission::CLASS_OWNER, true);
  EXPECT_TRUE(perm.canRead(Permission::CLASS_OWNER));
  EXPECT_EQ(0400u, perm.getOctalValue());
}

TEST_F(PermissionTest, SetReadPermissionRemove) {
  Permission perm(0444);
  perm.setRead(Permission::CLASS_OWNER, false);
  EXPECT_FALSE(perm.canRead(Permission::CLASS_OWNER));
  EXPECT_EQ(0044u, perm.getOctalValue());
}

TEST_F(PermissionTest, SetWritePermissionAdd) {
  Permission perm(0000);
  perm.setWrite(Permission::CLASS_GROUP, true);
  EXPECT_TRUE(perm.canWrite(Permission::CLASS_GROUP));
  EXPECT_EQ(0020u, perm.getOctalValue());
}

TEST_F(PermissionTest, SetWritePermissionRemove) {
  Permission perm(0222);
  perm.setWrite(Permission::CLASS_GROUP, false);
  EXPECT_FALSE(perm.canWrite(Permission::CLASS_GROUP));
  EXPECT_EQ(0202u, perm.getOctalValue());
}

TEST_F(PermissionTest, SetExecutePermissionAdd) {
  Permission perm(0000);
  perm.setExecute(Permission::CLASS_OTHER, true);
  EXPECT_TRUE(perm.canExecute(Permission::CLASS_OTHER));
  EXPECT_EQ(0001u, perm.getOctalValue());
}

TEST_F(PermissionTest, SetExecutePermissionRemove) {
  Permission perm(0111);
  perm.setExecute(Permission::CLASS_OTHER, false);
  EXPECT_FALSE(perm.canExecute(Permission::CLASS_OTHER));
  EXPECT_EQ(0110u, perm.getOctalValue());
}

TEST_F(PermissionTest, SetReadClassAll) {
  Permission perm(0000);
  perm.setRead(Permission::CLASS_ALL, true);
  EXPECT_TRUE(perm.canRead(Permission::CLASS_OWNER));
  EXPECT_TRUE(perm.canRead(Permission::CLASS_GROUP));
  EXPECT_TRUE(perm.canRead(Permission::CLASS_OTHER));
  EXPECT_EQ(0444u, perm.getOctalValue());
}

TEST_F(PermissionTest, SequentialOperations) {
  Permission perm(0000);
  perm.setRead(Permission::CLASS_OWNER, true);
  perm.setWrite(Permission::CLASS_OWNER, true);
  perm.setExecute(Permission::CLASS_OWNER, true);
  EXPECT_EQ(0700u, perm.getOctalValue());
  
  perm.setRead(Permission::CLASS_GROUP, true);
  perm.setExecute(Permission::CLASS_GROUP, true);
  EXPECT_EQ(0750u, perm.getOctalValue());
}

TEST_F(PermissionTest, TogglePermission) {
  Permission perm(0644);
  // Add execute for owner
  perm.setExecute(Permission::CLASS_OWNER, true);
  EXPECT_EQ(0744u, perm.getOctalValue());
  // Remove it again
  perm.setExecute(Permission::CLASS_OWNER, false);
  EXPECT_EQ(0644u, perm.getOctalValue());
}

TEST_F(PermissionTest, SetSpecialBitsSetUid) {
  Permission perm(0755);
  perm.setSetUid(true);
  EXPECT_TRUE(perm.hasSetUid());
  EXPECT_EQ(04755u, perm.getOctalValue());
  
  perm.setSetUid(false);
  EXPECT_FALSE(perm.hasSetUid());
  EXPECT_EQ(0755u, perm.getOctalValue());
}

TEST_F(PermissionTest, SetSpecialBitsSetGid) {
  Permission perm(0755);
  perm.setSetGid(true);
  EXPECT_TRUE(perm.hasSetGid());
  EXPECT_EQ(02755u, perm.getOctalValue());
  
  perm.setSetGid(false);
  EXPECT_FALSE(perm.hasSetGid());
  EXPECT_EQ(0755u, perm.getOctalValue());
}

TEST_F(PermissionTest, SetSpecialBitsStickyBit) {
  Permission perm(0777);
  perm.setStickyBit(true);
  EXPECT_TRUE(perm.hasStickyBit());
  EXPECT_EQ(01777u, perm.getOctalValue());
  
  perm.setStickyBit(false);
  EXPECT_FALSE(perm.hasStickyBit());
  EXPECT_EQ(0777u, perm.getOctalValue());
}

TEST_F(PermissionTest, ComparisonOperators) {
  Permission perm1(0755);
  Permission perm2(0644);
  
  EXPECT_TRUE(perm1 > perm2);
  EXPECT_TRUE(perm2 < perm1);
  EXPECT_TRUE(perm1 != perm2);
  EXPECT_FALSE(perm1 == perm2);
  EXPECT_TRUE(perm1 >= perm2);
  EXPECT_TRUE(perm2 <= perm1);
}

TEST_F(PermissionTest, ComparisonOperatorsEqual) {
  Permission perm1(0755);
  Permission perm2(0755);
  
  EXPECT_TRUE(perm1 == perm2);
  EXPECT_FALSE(perm1 != perm2);
  EXPECT_TRUE(perm1 >= perm2);
  EXPECT_TRUE(perm1 <= perm2);
}


// ===========================================================================
// Factory Methods Tests
// ===========================================================================
TEST_F(PermissionTest, ReadOnlyFactory) {
  Permission perm = Permission::readOnly();
  EXPECT_EQ(0444u, perm.getOctalValue());
}

TEST_F(PermissionTest, ReadWriteFactory) {
  Permission perm = Permission::readWrite();
  EXPECT_EQ(0666u, perm.getOctalValue());
}

TEST_F(PermissionTest, ReadWriteExecuteFactory) {
  Permission perm = Permission::readWriteExecute();
  EXPECT_EQ(0777u, perm.getOctalValue());
}


// ===========================================================================
// String Conversion Tests
// ===========================================================================
TEST_F(PermissionTest, ToStringOctal) {
  Permission perm(0755);
  EXPECT_EQ("0755", perm.toString());
}

TEST_F(PermissionTest, ToSymbolicString) {
  Permission perm(0755);
  EXPECT_EQ("--- rwxr-xr-x", perm.toSymbolicString());
}

