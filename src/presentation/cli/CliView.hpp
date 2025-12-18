/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CliView.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 17:09:09 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/17 19:37:33 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLI_VIEW_HPP
#define CLI_VIEW_HPP

#include "application/ports/ILogger.hpp"

#include <string>

class CliView {
 public:
  CliView(application::ports::ILogger& logger);
  ~CliView();

  void displayUsage(const std::string& programName) const;
  void displayError(const std::string& str) const;
  application::ports::ILogger& getLogger() const;

 private:
  CliView(const CliView& other);

  CliView& operator=(const CliView& other);

  application::ports::ILogger& m_logger;
};

#endif  // CLI_VIEW_HPP
