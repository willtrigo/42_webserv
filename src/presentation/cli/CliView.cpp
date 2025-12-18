/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CliView.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 17:09:16 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/17 19:38:14 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "application/ports/ILogger.hpp"
#include "presentation/cli/CliView.hpp"

#include <stdexcept>
#include <string>

CliView::CliView(application::ports::ILogger& logger) : m_logger(logger) {}

CliView::CliView(const CliView& other) : m_logger(other.m_logger) {}

CliView::~CliView() {}

CliView& CliView::operator=(const CliView& other) {
  if (this != &other) {
    return *this;
  }
  throw std::runtime_error("CliView assignment is not allowed");
}

void CliView::displayUsage(const std::string& programName) const {
  this->m_logger.error("Usage: " + programName + " <config_file>");
}

void CliView::displayError(const std::string& str) const {
  this->m_logger.error(str);
}

application::ports::ILogger& CliView::getLogger() const {
  return this->m_logger;
}
