/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileWriter.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 15:43:30 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/14 19:24:45 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_WRITER_HPP
#define FILE_WRITER_HPP

#include "application/ports/IFileWriter.hpp"

#include <fstream>
#include <ios>
#include <string>

namespace infrastructure {
namespace io {

class FileWriter : public application::ports::IFileWriter {
 public:
  explicit FileWriter(const std::string& path, std::ios_base::openmode mode);
  FileWriter(const FileWriter& other);
  virtual ~FileWriter();

  FileWriter& operator=(const FileWriter& others);

  virtual void write(const std::string& str, bool newLine);
  virtual void writeToFile(const std::string& filename,
                           std::ios_base::openmode mode,
                           const std::string& content) const;
  virtual void flush();

 private:
  mutable std::ofstream m_fileStream;
  std::string m_path;
  std::ios_base::openmode m_mode;
};

}  // namespace io
}  // namespace infrastructure

#endif  // FILE_WRITER_HPP
