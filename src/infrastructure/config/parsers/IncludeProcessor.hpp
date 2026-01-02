/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IncludeProcessor.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 21:44:33 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 22:04:11 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDE_PROCESSOR_HPP
#define INCLUDE_PROCESSOR_HPP

#include "application/ports/ILogger.hpp"
#include "domain/configuration/entities/HttpConfig.hpp"

#include <string>
#include <vector>

namespace infrastructure {
namespace config {
namespace parsers {

class IncludeProcessor {
 public:
  static const std::size_t MAX_INCLUDE_DEPTH = 10;

  explicit IncludeProcessor(application::ports::ILogger& logger);
  ~IncludeProcessor();

  void processIncludes(const std::string& pattern,
                       domain::configuration::entities::HttpConfig& httpConfig,
                       std::size_t currentDepth = 0);

 private:
  application::ports::ILogger& m_logger;

  std::vector<std::string> expandGlobPattern(const std::string& pattern);
  std::vector<std::string> listDirectory(const std::string& dirPath);
  bool matchesPattern(const std::string& filename, const std::string& pattern);
  std::string extractDirectory(const std::string& path);
  std::string extractFilename(const std::string& path);
  bool isAbsolutePath(const std::string& path);
  bool fileExists(const std::string& path);
};

}  // namespace parsers
}  // namespace config
}  // namespace infrastructure

#endif  // INCLUDE_PROCESSOR_HPP
