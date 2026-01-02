/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockIncludeProcessor.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umeneses <umeneses@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/01/02 11:41:20 by umeneses         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/parsers/IncludeProcessor.hpp"

namespace infrastructure {
namespace config {
namespace parsers {

// Stub implementation to satisfy linker when ConfigParser/GlobalDirectiveHandler
// are included but IncludeProcessor tests are not yet implemented
IncludeProcessor::IncludeProcessor(application::ports::ILogger& logger)
    : m_logger(logger) {}

IncludeProcessor::~IncludeProcessor() {}

void IncludeProcessor::processIncludes(
    const std::string& /* pattern */,
    domain::configuration::entities::HttpConfig& /* httpConfig */,
    std::size_t /* currentDepth */) {
  // Stub: does nothing - tests for ConfigParser not yet enabled
}

}  // namespace parsers
}  // namespace config
}  // namespace infrastructure
