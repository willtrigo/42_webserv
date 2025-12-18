/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IStreamWriter.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 17:24:41 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 11:16:32 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISTREAM_WRITER_HPP
#define ISTREAM_WRITER_HPP

#include <ostream>
#include <string>

namespace application {
namespace ports {

class IStreamWriter {
 public:
  virtual ~IStreamWriter() {}

  virtual void print(std::ostream& ostr, const std::string& str,
                     bool newLine) const = 0;
  virtual void flush() = 0;
};

}  // namespace ports
}  // namespace application

#endif  // ISTREAM_WRITER_HPP
