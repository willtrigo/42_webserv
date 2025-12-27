/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IFileWriter.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 15:43:45 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/19 11:18:14 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IFILE_WRITER_HPP
#define IFILE_WRITER_HPP

#include <ios>
#include <string>

namespace application {
namespace ports {

class IFileWriter {
 public:
  virtual ~IFileWriter() {}

  virtual void write(const std::string& str, bool newLine) = 0;
  virtual void writeToFile(const std::string& filename,
                           std::ios_base::openmode mode,
                           const std::string& content) const = 0;
  virtual void flush() = 0;
};

}  // namespace ports
}  // namespace application

#endif  // IFILE_WRITER_HPP
