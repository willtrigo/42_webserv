/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CliView.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 17:09:09 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/13 23:21:43 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLI_VIEW_HPP
#define CLI_VIEW_HPP

#include "application/ports/IStreamWriter.hpp"

#include <string>

class CliView {
 public:
  CliView(application::ports::IStreamWriter& writer);
  ~CliView();

  void displayUsage(const std::string& programName) const;
  void displayError(const std::string& str) const;

 private:
  CliView(const CliView& other);

  CliView& operator=(const CliView& other);

  application::ports::IStreamWriter& m_writer;
};

#endif  // CLI_VIEW_HPP
