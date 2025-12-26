/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockConfigParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: test                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 00:00:00 by test              #+#    #+#             */
/*   Updated: 2025/12/24 00:00:00 by test             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOCK_CONFIG_PARSER_HPP
#define MOCK_CONFIG_PARSER_HPP

#include "application/ports/IConfigParser.hpp"
#include <string>
#include <vector>

namespace tests {
namespace mocks {

class MockConfigParser : public application::ports::IConfigParser {
 public:
  MockConfigParser() : m_parsePathCalled(false), m_mergeIncludesCalled(false) {}
  virtual ~MockConfigParser() {}

  virtual void parsePath(const std::string& configPath) {
    m_parsePathCalled = true;
    m_lastParsedPath = configPath;
    m_parsedPaths.push_back(configPath);
  }

  virtual void mergeIncludes(const std::string& includePath) {
    m_mergeIncludesCalled = true;
    m_lastIncludePath = includePath;
    m_includedPaths.push_back(includePath);
  }

  bool wasParsePathCalled() const { return m_parsePathCalled; }
  bool wasMergeIncludesCalled() const { return m_mergeIncludesCalled; }
  const std::string& getLastParsedPath() const { return m_lastParsedPath; }
  const std::string& getLastIncludePath() const { return m_lastIncludePath; }
  const std::vector<std::string>& getParsedPaths() const { return m_parsedPaths; }
  const std::vector<std::string>& getIncludedPaths() const { return m_includedPaths; }

  void reset() {
    m_parsePathCalled = false;
    m_mergeIncludesCalled = false;
    m_lastParsedPath.clear();
    m_lastIncludePath.clear();
    m_parsedPaths.clear();
    m_includedPaths.clear();
  }

 private:
  bool m_parsePathCalled;
  bool m_mergeIncludesCalled;
  std::string m_lastParsedPath;
  std::string m_lastIncludePath;
  std::vector<std::string> m_parsedPaths;
  std::vector<std::string> m_includedPaths;
};

}  // namespace mocks
}  // namespace tests

#endif  // MOCK_CONFIG_PARSER_HPP
