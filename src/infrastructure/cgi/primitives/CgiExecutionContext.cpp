/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExecutionContext.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 01:19:29 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 04:22:01 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/cgi/primitives/CgiExecutionContext.hpp"

#include <sys/wait.h>

namespace infrastructure {
namespace cgi {
namespace primitives {

CgiExecutionContext::CgiExecutionContext()
    : m_childPid(INVALID_PID),
      m_exitStatus(INVALID_STATUS),
      m_timedOut(false) {}

CgiExecutionContext::~CgiExecutionContext() {}

CgiExecutionContext::CgiExecutionContext(const CgiExecutionContext& other) {
  copyFrom(other);
}

CgiExecutionContext& CgiExecutionContext::operator=(
    const CgiExecutionContext& other) {
  if (this != &other) {
    copyFrom(other);
  }
  return *this;
}

pid_t CgiExecutionContext::getChildPid() const { return m_childPid; }

void CgiExecutionContext::setChildPid(pid_t pid) { m_childPid = pid; }

PipeDescriptors& CgiExecutionContext::getPipes() { return m_pipes; }

const PipeDescriptors& CgiExecutionContext::getPipes() const { return m_pipes; }

void CgiExecutionContext::setPipes(const PipeDescriptors& pipes) {
  m_pipes = pipes;
}

const std::vector<char>& CgiExecutionContext::getOutput() const {
  return m_output;
}

void CgiExecutionContext::setOutput(const std::vector<char>& output) {
  m_output = output;
}

void CgiExecutionContext::appendOutput(const std::vector<char>& data) {
  m_output.insert(m_output.end(), data.begin(), data.end());
}

void CgiExecutionContext::clearOutput() { m_output.clear(); }

const std::vector<char>& CgiExecutionContext::getErrorOutput() const {
  return m_errorOutput;
}

void CgiExecutionContext::setErrorOutput(const std::vector<char>& errorOutput) {
  m_errorOutput = errorOutput;
}

void CgiExecutionContext::appendErrorOutput(const std::vector<char>& data) {
  m_errorOutput.insert(m_errorOutput.end(), data.begin(), data.end());
}

void CgiExecutionContext::clearErrorOutput() { m_errorOutput.clear(); }

int CgiExecutionContext::getExitStatus() const { return m_exitStatus; }

void CgiExecutionContext::setExitStatus(int status) { m_exitStatus = status; }

bool CgiExecutionContext::hasTimedOut() const { return m_timedOut; }

void CgiExecutionContext::setTimedOut(bool timedOut) { m_timedOut = timedOut; }

bool CgiExecutionContext::isChildRunning() const {
  return m_childPid != INVALID_PID;
}

bool CgiExecutionContext::hasExited() const {
  return WIFEXITED(m_exitStatus) != 0;
}

bool CgiExecutionContext::wasSignaled() const {
  return WIFSIGNALED(m_exitStatus) != 0;
}

int CgiExecutionContext::getExitCode() const {
  if (hasExited()) {
    return WEXITSTATUS(m_exitStatus);
  }
  return -1;
}

int CgiExecutionContext::getTermSignal() const {
  if (wasSignaled()) {
    return WTERMSIG(m_exitStatus);
  }
  return -1;
}

void CgiExecutionContext::reset() {
  m_childPid = INVALID_PID;
  m_pipes.closeAll();
  m_pipes.invalidateAll();
  m_output.clear();
  m_errorOutput.clear();
  m_exitStatus = INVALID_STATUS;
  m_timedOut = false;
}

void CgiExecutionContext::copyFrom(const CgiExecutionContext& other) {
  m_childPid = other.m_childPid;
  m_pipes = other.m_pipes;
  m_output = other.m_output;
  m_errorOutput = other.m_errorOutput;
  m_exitStatus = other.m_exitStatus;
  m_timedOut = other.m_timedOut;
}

}  // namespace primitives
}  // namespace cgi
}  // namespace infrastructure
