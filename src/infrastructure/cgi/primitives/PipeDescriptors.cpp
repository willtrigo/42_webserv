/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PipeDescriptors.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 01:14:29 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 01:17:03 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/cgi/primitives/PipeDescriptors.hpp"

#include <unistd.h>

namespace infrastructure {
namespace cgi {
namespace primitives {

PipeDescriptors::PipeDescriptors() {
  m_stdinPipe[READ_END] = INVALID_FD;
  m_stdinPipe[WRITE_END] = INVALID_FD;
  m_stdoutPipe[READ_END] = INVALID_FD;
  m_stdoutPipe[WRITE_END] = INVALID_FD;
  m_stderrPipe[READ_END] = INVALID_FD;
  m_stderrPipe[WRITE_END] = INVALID_FD;
}

PipeDescriptors::~PipeDescriptors() { closeAll(); }

PipeDescriptors::PipeDescriptors(const PipeDescriptors& other) {
  copyFrom(other);
}

PipeDescriptors& PipeDescriptors::operator=(const PipeDescriptors& other) {
  if (this != &other) {
    closeAll();
    copyFrom(other);
  }
  return *this;
}

int PipeDescriptors::getStdinReadFd() const { return m_stdinPipe[READ_END]; }

int PipeDescriptors::getStdinWriteFd() const { return m_stdinPipe[WRITE_END]; }

int PipeDescriptors::getStdoutReadFd() const { return m_stdoutPipe[READ_END]; }

int PipeDescriptors::getStdoutWriteFd() const {
  return m_stdoutPipe[WRITE_END];
}

int PipeDescriptors::getStderrReadFd() const { return m_stderrPipe[READ_END]; }

int PipeDescriptors::getStderrWriteFd() const {
  return m_stderrPipe[WRITE_END];
}

void PipeDescriptors::setStdinReadFd(int fileDescriptor) { m_stdinPipe[READ_END] = fileDescriptor; }

void PipeDescriptors::setStdinWriteFd(int fileDescriptor) { m_stdinPipe[WRITE_END] = fileDescriptor; }

void PipeDescriptors::setStdoutReadFd(int fileDescriptor) { m_stdoutPipe[READ_END] = fileDescriptor; }

void PipeDescriptors::setStdoutWriteFd(int fileDescriptor) { m_stdoutPipe[WRITE_END] = fileDescriptor; }

void PipeDescriptors::setStderrReadFd(int fileDescriptor) { m_stderrPipe[READ_END] = fileDescriptor; }

void PipeDescriptors::setStderrWriteFd(int fileDescriptor) { m_stderrPipe[WRITE_END] = fileDescriptor; }

bool PipeDescriptors::isStdinReadOpen() const {
  return m_stdinPipe[READ_END] != INVALID_FD;
}

bool PipeDescriptors::isStdinWriteOpen() const {
  return m_stdinPipe[WRITE_END] != INVALID_FD;
}

bool PipeDescriptors::isStdoutReadOpen() const {
  return m_stdoutPipe[READ_END] != INVALID_FD;
}

bool PipeDescriptors::isStdoutWriteOpen() const {
  return m_stdoutPipe[WRITE_END] != INVALID_FD;
}

bool PipeDescriptors::isStderrReadOpen() const {
  return m_stderrPipe[READ_END] != INVALID_FD;
}

bool PipeDescriptors::isStderrWriteOpen() const {
  return m_stderrPipe[WRITE_END] != INVALID_FD;
}

void PipeDescriptors::closeStdinRead() { closeFd(m_stdinPipe[READ_END]); }

void PipeDescriptors::closeStdinWrite() { closeFd(m_stdinPipe[WRITE_END]); }

void PipeDescriptors::closeStdoutRead() { closeFd(m_stdoutPipe[READ_END]); }

void PipeDescriptors::closeStdoutWrite() { closeFd(m_stdoutPipe[WRITE_END]); }

void PipeDescriptors::closeStderrRead() { closeFd(m_stderrPipe[READ_END]); }

void PipeDescriptors::closeStderrWrite() { closeFd(m_stderrPipe[WRITE_END]); }

void PipeDescriptors::closeAll() {
  closeStdinRead();
  closeStdinWrite();
  closeStdoutRead();
  closeStdoutWrite();
  closeStderrRead();
  closeStderrWrite();
}

void PipeDescriptors::closeUnusedInChild() {
  closeStdinWrite();
  closeStdoutRead();
  closeStderrRead();
}

void PipeDescriptors::closeUnusedInParent() {
  closeStdinRead();
  closeStdoutWrite();
  closeStderrWrite();
}

void PipeDescriptors::invalidateAll() {
  m_stdinPipe[READ_END] = INVALID_FD;
  m_stdinPipe[WRITE_END] = INVALID_FD;
  m_stdoutPipe[READ_END] = INVALID_FD;
  m_stdoutPipe[WRITE_END] = INVALID_FD;
  m_stderrPipe[READ_END] = INVALID_FD;
  m_stderrPipe[WRITE_END] = INVALID_FD;
}

void PipeDescriptors::closeFd(int& fileDescriptor) {
  if (fileDescriptor != INVALID_FD) {
    close(fileDescriptor);
    fileDescriptor = INVALID_FD;
  }
}

void PipeDescriptors::copyFrom(const PipeDescriptors& other) {
  m_stdinPipe[READ_END] = other.m_stdinPipe[READ_END];
  m_stdinPipe[WRITE_END] = other.m_stdinPipe[WRITE_END];
  m_stdoutPipe[READ_END] = other.m_stdoutPipe[READ_END];
  m_stdoutPipe[WRITE_END] = other.m_stdoutPipe[WRITE_END];
  m_stderrPipe[READ_END] = other.m_stderrPipe[READ_END];
  m_stderrPipe[WRITE_END] = other.m_stderrPipe[WRITE_END];
}

}  // namespace primitives
}  // namespace cgi
}  // namespace infrastructure
