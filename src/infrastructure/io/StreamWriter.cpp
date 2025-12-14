/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StreamWriter.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 17:24:58 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/14 12:58:14 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/io/StreamWriter.hpp"

#include <iostream>
#include <ostream>
#include <stdexcept>

namespace infrastructure {
namespace io {

StreamWriter::StreamWriter() {}

StreamWriter::StreamWriter(const StreamWriter& /*unused*/) {}

StreamWriter::~StreamWriter() { flush(); }

StreamWriter& StreamWriter::operator=(const StreamWriter& /*unused*/) {
  return *this;
}

void StreamWriter::print(std::ostream& ostr, const std::string& str,
                         bool newLine) const {
  if (!ostr.good()) {
    throw std::runtime_error("Output stream in invalid state; cannot print.");
  }

  ostr << str;
  if (newLine) {
    ostr << std::endl;
  }
}

void StreamWriter::flush() { std::cout.flush(); }

}  // namespace io
}  // namespace infrastructure
