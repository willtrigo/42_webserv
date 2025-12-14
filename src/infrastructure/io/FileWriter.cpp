/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileWriter.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 15:50:37 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/14 19:55:48 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/io/FileWriter.hpp"

#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>

namespace infrastructure {
namespace io {

FileWriter::FileWriter(const std::string& path, std::ios_base::openmode mode)
    : m_path(path), m_mode(mode) {
  this->m_fileStream.open(path.c_str(), mode);
  if (!this->m_fileStream.is_open()) {
    throw std::runtime_error("FileWriter: Failed to open '" + path +
                             "'(check path/permissions)");
  }

  if ((mode & std::ios_base::trunc) != 0) {
    this->m_fileStream.flush();
  }
}

FileWriter::FileWriter(const FileWriter& other)
    : m_path(other.m_path), m_mode(other.m_mode) {
  this->m_fileStream.open(this->m_path.c_str(), this->m_mode);
  if (!this->m_fileStream.is_open()) {
    throw std::runtime_error("FileWriter: Failed to open '" + this->m_path + "' in copy constructor");
  }
}

FileWriter::~FileWriter() {
  if (this->m_fileStream.is_open()) {
    this->m_fileStream.flush();
    this->m_fileStream.close();
  }
}

FileWriter& FileWriter::operator=(const FileWriter& other) {
  if (this != &other) {
    if (this->m_fileStream.is_open()) {
      this->m_fileStream.close();
    }

    this->m_path = other.m_path;
    this->m_mode = other.m_mode;

    this->m_fileStream.open(this->m_path.c_str(), this->m_mode);
    if (!this->m_fileStream.is_open()) {
      throw std::runtime_error("FileWriter: Failed to open '" + this->m_path + "' in assignment operator");
    }
  }
  return *this;
}

void FileWriter::write(const std::string& str, bool newLine) {
  if (!this->m_fileStream.is_open() || !this->m_fileStream.good()) {
    throw std::runtime_error("FileWriter: Stream invalid for path '" +
                             this->m_path + "'");
  }

  this->m_fileStream << str;
  if (newLine) {
    this->m_fileStream << '\n';
  }

  if (!this->m_fileStream.good()) {
    throw std::runtime_error("FileWriter: write failed on '" + this->m_path + "'");
  }
}

void FileWriter::writeToFile(const std::string& filename,
                             std::ios_base::openmode mode,
                             const std::string& content) const {
  FileWriter tempFile(filename, mode);
  tempFile.write(content, true);
  tempFile.flush();
}

void FileWriter::flush() {
  if (this->m_fileStream.is_open()) {
    this->m_fileStream.flush();
    if (!this->m_fileStream.good()) {
      throw std::runtime_error("FileWriter: Flush failed on '" + this->m_path +
                               "'");
    }
  }
}

}  // namespace io
}  // namespace infrastructure
