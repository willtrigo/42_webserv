/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_CgiConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 18:16:50 by umeneses          #+#    #+#             */
/*   Updated: 2026/01/05 11:34:49 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "domain/configuration/exceptions/CgiConfigException.hpp"
#include "domain/configuration/value_objects/CgiConfig.hpp"
#include "domain/filesystem/exceptions/PathException.hpp"
#include "domain/shared/exceptions/RegexPatternException.hpp"

#include <gtest/gtest.h>

using namespace domain::configuration::value_objects;
using namespace domain::configuration::exceptions;

class CgiConfigTest : public ::testing::Test {
 protected:
  void SetUp() {}
  void TearDown() {}
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(CgiConfigTest, DefaultConstructor) {
  CgiConfig* config = NULL;
  EXPECT_NO_THROW(config = new CgiConfig());
  if (config != NULL) {
    EXPECT_TRUE(config->getCgiRoot().toString() == "/");
    delete config;
  }
}

TEST_F(CgiConfigTest, ConstructWithValidParameters) {
  domain::filesystem::value_objects::Path cgiRoot("/usr/lib/cgi-bin");
  domain::shared::value_objects::RegexPattern pattern("\\.php$");

  EXPECT_NO_THROW(CgiConfig("/usr/bin/php-cgi", cgiRoot, pattern));
}

TEST_F(CgiConfigTest, ConstructWithEmptyScriptPath) {
  domain::filesystem::value_objects::Path cgiRoot("/usr/lib/cgi-bin");
  domain::shared::value_objects::RegexPattern pattern("\\.php$");

  EXPECT_THROW(CgiConfig("", cgiRoot, pattern), CgiConfigException);
}

TEST_F(CgiConfigTest, ConstructWithRelativeScriptPath) {
  domain::filesystem::value_objects::Path cgiRoot("/usr/lib/cgi-bin");
  domain::shared::value_objects::RegexPattern pattern("\\.php$");

  EXPECT_THROW(CgiConfig("relative/path", cgiRoot, pattern),
               CgiConfigException);
}

TEST_F(CgiConfigTest, CopyConstructor) {
  CgiConfig original;
  original.setScriptPath("/usr/bin/php-cgi");
  original.addParameter("TEST_PARAM", "test_value");

  CgiConfig copy(original);
  EXPECT_EQ(original.getScriptPath(), copy.getScriptPath());
  EXPECT_TRUE(copy.hasParameter("TEST_PARAM"));
}

TEST_F(CgiConfigTest, AssignmentOperator) {
  CgiConfig config1;
  CgiConfig config2;

  config1.setScriptPath("/usr/bin/php-cgi");
  config1.addParameter("PARAM1", "value1");

  config2 = config1;
  EXPECT_EQ(config1.getScriptPath(), config2.getScriptPath());
  EXPECT_TRUE(config2.hasParameter("PARAM1"));
}

TEST_F(CgiConfigTest, SelfAssignment) {
  CgiConfig config;
  config.setScriptPath("/usr/bin/php-cgi");

  config = config;
  EXPECT_EQ("/usr/bin/php-cgi", config.getScriptPath());
}

// ============================================================================
// Getter Tests
// ============================================================================

TEST_F(CgiConfigTest, GetScriptPath) {
  CgiConfig config;
  config.setScriptPath("/usr/bin/php-cgi");

  EXPECT_EQ("/usr/bin/php-cgi", config.getScriptPath());
}

TEST_F(CgiConfigTest, GetCgiRoot) {
  CgiConfig config;
  domain::filesystem::value_objects::Path newRoot("/var/www/cgi-bin");
  config.setCgiRoot(newRoot);

  EXPECT_EQ("/var/www/cgi-bin", config.getCgiRoot().toString());
}

TEST_F(CgiConfigTest, GetExtensionPattern) {
  CgiConfig config;
  EXPECT_NO_THROW(config.getExtensionPattern());
}

TEST_F(CgiConfigTest, GetParameters) {
  CgiConfig config;
  const CgiConfig::ParameterMap& params = config.getParameters();

  EXPECT_FALSE(params.empty());
}

TEST_F(CgiConfigTest, GetParameterExists) {
  CgiConfig config;
  config.addParameter("TEST_VAR", "test_value");

  EXPECT_EQ("test_value", config.getParameter("TEST_VAR"));
}

TEST_F(CgiConfigTest, GetParameterNotExists) {
  CgiConfig config;
  EXPECT_EQ("", config.getParameter("NON_EXISTENT"));
}

TEST_F(CgiConfigTest, HasParameterTrue) {
  CgiConfig config;
  config.addParameter("EXISTS", "value");

  EXPECT_TRUE(config.hasParameter("EXISTS"));
}

TEST_F(CgiConfigTest, HasParameterFalse) {
  CgiConfig config;
  EXPECT_FALSE(config.hasParameter("DOES_NOT_EXIST"));
}

// ============================================================================
// Setter Tests - Script Path
// ============================================================================

TEST_F(CgiConfigTest, SetScriptPathValid) {
  CgiConfig config;
  EXPECT_NO_THROW(config.setScriptPath("/usr/bin/python3"));
  EXPECT_EQ("/usr/bin/python3", config.getScriptPath());
}

TEST_F(CgiConfigTest, SetScriptPathEmpty) {
  CgiConfig config;
  EXPECT_THROW(config.setScriptPath(""), CgiConfigException);
}

TEST_F(CgiConfigTest, SetScriptPathRelative) {
  CgiConfig config;
  EXPECT_THROW(config.setScriptPath("bin/php"), CgiConfigException);
}

TEST_F(CgiConfigTest, SetScriptPathWithWhitespace) {
  CgiConfig config;
  EXPECT_NO_THROW(config.setScriptPath("  /usr/bin/php  "));
  EXPECT_EQ("/usr/bin/php", config.getScriptPath());
}

// ============================================================================
// Setter Tests - CGI Root
// ============================================================================

TEST_F(CgiConfigTest, SetCgiRootValid) {
  CgiConfig config;
  domain::filesystem::value_objects::Path newRoot("/var/www/cgi");

  EXPECT_NO_THROW(config.setCgiRoot(newRoot));
  EXPECT_EQ("/var/www/cgi", config.getCgiRoot().toString());
}

TEST_F(CgiConfigTest, SetCgiRootEmpty) {
  CgiConfig config;

  EXPECT_THROW(domain::filesystem::value_objects::Path(""),
               domain::filesystem::exceptions::PathException);
}

TEST_F(CgiConfigTest, SetCgiRootRelative) {
  CgiConfig config;

  EXPECT_THROW(domain::filesystem::value_objects::Path("cgi-bin"),
               domain::filesystem::exceptions::PathException);
}

// ============================================================================
// Setter Tests - Extension Pattern
// ============================================================================

TEST_F(CgiConfigTest, SetExtensionPatternValid) {
  CgiConfig config;
  domain::shared::value_objects::RegexPattern pattern("\\.py$");

  EXPECT_NO_THROW(config.setExtensionPattern(pattern));
}

TEST_F(CgiConfigTest, SetExtensionPatternEmpty) {
  CgiConfig config;
  domain::shared::value_objects::RegexPattern
      emptyPattern;

  EXPECT_THROW(config.setExtensionPattern(emptyPattern), CgiConfigException);
}

// ============================================================================
// Setter Tests - Parameters
// ============================================================================

TEST_F(CgiConfigTest, AddParameterValid) {
  CgiConfig config;
  EXPECT_NO_THROW(config.addParameter("CUSTOM_VAR", "custom_value"));
  EXPECT_TRUE(config.hasParameter("CUSTOM_VAR"));
  EXPECT_EQ("custom_value", config.getParameter("CUSTOM_VAR"));
}

TEST_F(CgiConfigTest, AddParameterInvalidName) {
  CgiConfig config;
  EXPECT_THROW(config.addParameter("", "value"), CgiConfigException);
}

TEST_F(CgiConfigTest, AddParameterInvalidValue) {
  CgiConfig config;
  std::string invalidValue;
  invalidValue.push_back('\0');

  EXPECT_THROW(config.addParameter("VAR", invalidValue), CgiConfigException);
}

TEST_F(CgiConfigTest, AddParameterDuplicate) {
  CgiConfig config;
  config.addParameter("PARAM1", "value1");

  EXPECT_THROW(config.addParameter("PARAM1", "value2"), CgiConfigException);
}

TEST_F(CgiConfigTest, RemoveParameter) {
  CgiConfig config;
  config.addParameter("TEMP_VAR", "temp_value");
  EXPECT_TRUE(config.hasParameter("TEMP_VAR"));

  config.removeParameter("TEMP_VAR");
  EXPECT_FALSE(config.hasParameter("TEMP_VAR"));
}

TEST_F(CgiConfigTest, RemoveNonExistentParameter) {
  CgiConfig config;
  EXPECT_NO_THROW(config.removeParameter("DOES_NOT_EXIST"));
}

TEST_F(CgiConfigTest, SetParametersValid) {
  CgiConfig config;
  CgiConfig::ParameterMap params;
  params["VAR1"] = "value1";
  params["VAR2"] = "value2";

  EXPECT_NO_THROW(config.setParameters(params));
  EXPECT_TRUE(config.hasParameter("VAR1"));
  EXPECT_TRUE(config.hasParameter("VAR2"));
}

TEST_F(CgiConfigTest, SetParametersInvalidName) {
  CgiConfig config;
  CgiConfig::ParameterMap params;
  params[""] = "value";

  EXPECT_THROW(config.setParameters(params), CgiConfigException);
}

// ============================================================================
// Validation Tests
// ============================================================================

TEST_F(CgiConfigTest, IsValidTrue) {
  CgiConfig config;
  config.setScriptPath("/usr/bin/php-cgi");

  EXPECT_TRUE(config.isValid());
}

TEST_F(CgiConfigTest, IsValidFalseWhenMissingRequiredParameter) {
  domain::filesystem::value_objects::Path cgiRoot("/usr/lib/cgi-bin");
  domain::shared::value_objects::RegexPattern pattern("\\.php$");
  CgiConfig config("/usr/bin/php-cgi", cgiRoot, pattern);

  config.removeParameter(CgiConfig::DEFAULT_REQUEST_METHOD);

  EXPECT_FALSE(config.isValid());
}

TEST_F(CgiConfigTest, ValidateSuccess) {
  CgiConfig config;
  config.setScriptPath("/usr/bin/php-cgi");

  EXPECT_NO_THROW(config.validate());
}

TEST_F(CgiConfigTest, ValidateEmptyScriptPath) {
  CgiConfig config;

  EXPECT_NO_THROW(config.validate());
  EXPECT_TRUE(config.getScriptPath().empty());
}

TEST_F(CgiConfigTest, ValidateMissingRequiredParameter) {
  CgiConfig config;
  config.setScriptPath("/usr/bin/php-cgi");
  config.removeParameter(CgiConfig::DEFAULT_SCRIPT_FILENAME);

  EXPECT_THROW(config.validate(), CgiConfigException);
}

// ============================================================================
// Factory Method Tests
// ============================================================================

TEST_F(CgiConfigTest, CreatePhpCgiDefault) {
  CgiConfig config = CgiConfig::createPhpCgi();

  EXPECT_EQ("/usr/bin/php-cgi", config.getScriptPath());
  EXPECT_TRUE(config.hasParameter("REDIRECT_STATUS"));
  EXPECT_EQ("200", config.getParameter("REDIRECT_STATUS"));
}

TEST_F(CgiConfigTest, CreatePhpCgiCustomPath) {
  CgiConfig config = CgiConfig::createPhpCgi("/opt/php/bin/php-cgi");

  EXPECT_EQ("/opt/php/bin/php-cgi", config.getScriptPath());
}

TEST_F(CgiConfigTest, CreatePythonCgiDefault) {
  CgiConfig config = CgiConfig::createPythonCgi();

  EXPECT_EQ("/usr/bin/python3", config.getScriptPath());
  EXPECT_TRUE(config.hasParameter("PYTHONPATH"));
}

TEST_F(CgiConfigTest, CreatePythonCgiCustomPath) {
  CgiConfig config = CgiConfig::createPythonCgi("/usr/local/bin/python3");

  EXPECT_EQ("/usr/local/bin/python3", config.getScriptPath());
}

TEST_F(CgiConfigTest, CreatePerlCgiDefault) {
  CgiConfig config = CgiConfig::createPerlCgi();

  EXPECT_EQ("/usr/bin/perl", config.getScriptPath());
  EXPECT_TRUE(config.hasParameter("PERL5LIB"));
}

TEST_F(CgiConfigTest, CreatePerlCgiCustomPath) {
  CgiConfig config = CgiConfig::createPerlCgi("/usr/local/bin/perl");

  EXPECT_EQ("/usr/local/bin/perl", config.getScriptPath());
}

// ============================================================================
// Extension Matching Tests (RegexPattern dependency)
// ============================================================================

TEST_F(CgiConfigTest, MatchesExtensionPhp) {
  CgiConfig config = CgiConfig::createPhpCgi();

  EXPECT_TRUE(config.matchesExtension("script.php"));
  EXPECT_TRUE(config.matchesExtension("index.php"));
}

TEST_F(CgiConfigTest, MatchesExtensionPython) {
  CgiConfig config = CgiConfig::createPythonCgi();

  EXPECT_TRUE(config.matchesExtension("script.py"));
  EXPECT_TRUE(config.matchesExtension("app.py"));
}

TEST_F(CgiConfigTest, MatchesExtensionPerl) {
  CgiConfig config = CgiConfig::createPerlCgi();

  EXPECT_TRUE(config.matchesExtension("script.pl"));
  EXPECT_TRUE(config.matchesExtension("script.cgi"));
}

TEST_F(CgiConfigTest, MatchesExtensionNoMatch) {
  CgiConfig config = CgiConfig::createPhpCgi();

  EXPECT_FALSE(config.matchesExtension("script.py"));
  EXPECT_FALSE(config.matchesExtension("index.html"));
}

// ============================================================================
// Path Resolution Tests
// ============================================================================

TEST_F(CgiConfigTest, ResolveScriptPathSimple) {
  CgiConfig config;
  domain::filesystem::value_objects::Path cgiRoot("/var/www/cgi-bin");
  config.setCgiRoot(cgiRoot);

  std::string resolved = config.resolveScriptPath("/script.php");
  EXPECT_EQ("/var/www/cgi-bin/script.php", resolved);
}

TEST_F(CgiConfigTest, ResolveScriptPathWithoutLeadingSlash) {
  CgiConfig config;
  domain::filesystem::value_objects::Path cgiRoot("/var/www/cgi-bin");
  config.setCgiRoot(cgiRoot);

  std::string resolved = config.resolveScriptPath("script.php");
  EXPECT_EQ("/var/www/cgi-bin/script.php", resolved);
}

TEST_F(CgiConfigTest, ResolveScriptPathEmpty) {
  CgiConfig config;
  EXPECT_EQ("", config.resolveScriptPath(""));
}

TEST_F(CgiConfigTest, ResolveScriptPathRootDirectory) {
  CgiConfig config;
  domain::filesystem::value_objects::Path cgiRoot("/");
  config.setCgiRoot(cgiRoot);

  std::string resolved = config.resolveScriptPath("/cgi-bin/script.php");
  EXPECT_EQ("//cgi-bin/script.php", resolved);
}

// ============================================================================
// Environment Building Tests
// ============================================================================

TEST_F(CgiConfigTest, BuildEnvironmentEmpty) {
  CgiConfig config;
  std::map<std::string, std::string> requestInfo;

  CgiConfig::ParameterMap env = config.buildEnvironment(requestInfo);
  EXPECT_FALSE(env.empty());
}

TEST_F(CgiConfigTest, BuildEnvironmentWithRequestInfo) {
  CgiConfig config;
  std::map<std::string, std::string> requestInfo;
  requestInfo["QUERY_STRING"] = "param=value";
  requestInfo["REQUEST_METHOD"] = "GET";

  CgiConfig::ParameterMap env = config.buildEnvironment(requestInfo);
  EXPECT_EQ("param=value", env["QUERY_STRING"]);
  EXPECT_EQ("GET", env["REQUEST_METHOD"]);
}

TEST_F(CgiConfigTest, BuildEnvironmentPreservesParameters) {
  CgiConfig config;
  config.addParameter("CUSTOM_PARAM", "custom_value");
  std::map<std::string, std::string> requestInfo;

  CgiConfig::ParameterMap env = config.buildEnvironment(requestInfo);
  EXPECT_EQ("custom_value", env["CUSTOM_PARAM"]);
}

// ============================================================================
// Comparison Operator Tests
// ============================================================================

TEST_F(CgiConfigTest, EqualityOperatorSame) {
  CgiConfig config1;
  CgiConfig config2;

  config1.setScriptPath("/usr/bin/php-cgi");
  config2.setScriptPath("/usr/bin/php-cgi");

  EXPECT_TRUE(config1 == config2);
}

TEST_F(CgiConfigTest, EqualityOperatorDifferentScriptPath) {
  CgiConfig config1;
  CgiConfig config2;

  config1.setScriptPath("/usr/bin/php-cgi");
  config2.setScriptPath("/usr/bin/python3");

  EXPECT_FALSE(config1 == config2);
}

TEST_F(CgiConfigTest, EqualityOperatorDifferentRoot) {
  CgiConfig config1;
  CgiConfig config2;

  domain::filesystem::value_objects::Path root1("/var/www/cgi");
  domain::filesystem::value_objects::Path root2("/usr/lib/cgi-bin");

  config1.setCgiRoot(root1);
  config2.setCgiRoot(root2);

  EXPECT_FALSE(config1 == config2);
}

TEST_F(CgiConfigTest, InequalityOperator) {
  CgiConfig config1;
  CgiConfig config2;

  config1.setScriptPath("/usr/bin/php-cgi");
  config2.setScriptPath("/usr/bin/python3");

  EXPECT_TRUE(config1 != config2);
}

// ============================================================================
// Utility Tests
// ============================================================================

TEST_F(CgiConfigTest, Clear) {
  CgiConfig config;
  config.setScriptPath("/usr/bin/php-cgi");
  config.addParameter("CUSTOM", "value");

  config.clear();

  EXPECT_TRUE(config.getScriptPath().empty());
  EXPECT_FALSE(config.hasParameter("CUSTOM"));
  EXPECT_EQ("/", config.getCgiRoot().toString());
}

TEST_F(CgiConfigTest, ClearRestoresDefaults) {
  CgiConfig config;
  config.clear();

  EXPECT_TRUE(config.hasParameter(CgiConfig::DEFAULT_GATEWAY_INTERFACE));
  EXPECT_EQ("CGI/1.1",
            config.getParameter(CgiConfig::DEFAULT_GATEWAY_INTERFACE));
}

// ============================================================================
// Integration Tests (with RegexPattern factory methods)
// ============================================================================

TEST_F(CgiConfigTest, PhpConfigMatchesPhpFiles) {
  CgiConfig config = CgiConfig::createPhpCgi();

  EXPECT_TRUE(config.matchesExtension("index.php"));
  EXPECT_TRUE(config.matchesExtension("admin.php"));
  EXPECT_FALSE(config.matchesExtension("index.html"));
}

TEST_F(CgiConfigTest, PythonConfigMatchesPythonFiles) {
  CgiConfig config = CgiConfig::createPythonCgi();

  EXPECT_TRUE(config.matchesExtension("app.py"));
  EXPECT_TRUE(config.matchesExtension("main.py"));
  EXPECT_FALSE(config.matchesExtension("script.php"));
}

TEST_F(CgiConfigTest, PerlConfigMatchesPerlFiles) {
  CgiConfig config = CgiConfig::createPerlCgi();

  EXPECT_TRUE(config.matchesExtension("script.pl"));
  EXPECT_TRUE(config.matchesExtension("test.cgi"));
  EXPECT_FALSE(config.matchesExtension("test.py"));
}
