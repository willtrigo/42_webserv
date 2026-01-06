/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExecutor.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 17:36:32 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 01:43:42 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/cgi/adapters/CgiExecutor.hpp"
#include "infrastructure/cgi/exceptions/CgiExecutionException.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <signal.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace infrastructure {
namespace cgi {
namespace adapters {

CgiExecutor::CgiExecutor(application::ports::ILogger& logger)
    : m_logger(logger),
      m_timeoutSeconds(DEFAULT_TIMEOUT_SECONDS),
      m_maxOutputSize(DEFAULT_MAX_OUTPUT_SIZE) {}

CgiExecutor::~CgiExecutor() {}

CgiExecutor::CgiExecutor(const CgiExecutor& other)
    : m_logger(other.m_logger),
      m_timeoutSeconds(other.m_timeoutSeconds),
      m_maxOutputSize(other.m_maxOutputSize) {}

CgiExecutor& CgiExecutor::operator=(const CgiExecutor& other) {
  if (this != &other) {
    m_timeoutSeconds = other.m_timeoutSeconds;
    m_maxOutputSize = other.m_maxOutputSize;
  }
  return *this;
}

primitives::CgiResponse CgiExecutor::execute(
    const primitives::CgiRequest& request) {
  validateRequest(request);

  m_logger.debug("Executing CGI script: " + request.getScriptPath());

  primitives::CgiExecutionContext context = runChildProcess(request);

  return buildResponse(context);
}

void CgiExecutor::setTimeout(unsigned int seconds) {
  m_timeoutSeconds = seconds;
}

unsigned int CgiExecutor::getTimeout() const { return m_timeoutSeconds; }

void CgiExecutor::setMaxOutputSize(std::size_t bytes) {
  m_maxOutputSize = bytes;
}

std::size_t CgiExecutor::getMaxOutputSize() const { return m_maxOutputSize; }

void CgiExecutor::validateRequest(const primitives::CgiRequest& request) {
  request.validate();

  const std::string& scriptPath = request.getScriptPath();
  if (!fileExists(scriptPath)) {
    throw exceptions::CgiExecutionException(
        "Script not found: " + scriptPath,
        exceptions::CgiExecutionException::SCRIPT_NOT_FOUND);
  }

  validateScriptExecutability(scriptPath);

  const std::string& interpreter = request.getInterpreter();
  if (!interpreter.empty() && interpreter != scriptPath) {
    validateInterpreter(interpreter);
  }
}

void CgiExecutor::validateScriptExecutability(const std::string& scriptPath) {
  if (!isFileExecutable(scriptPath)) {
    throw exceptions::CgiExecutionException(
        "Script is not executable: " + scriptPath,
        exceptions::CgiExecutionException::SCRIPT_NOT_EXECUTABLE);
  }
}

void CgiExecutor::validateInterpreter(const std::string& interpreter) {
  if (!fileExists(interpreter)) {
    throw exceptions::CgiExecutionException(
        "Interpreter not found: " + interpreter,
        exceptions::CgiExecutionException::INTERPRETER_NOT_FOUND);
  }

  if (!isFileExecutable(interpreter)) {
    throw exceptions::CgiExecutionException(
        "Interpreter is not executable: " + interpreter,
        exceptions::CgiExecutionException::INTERPRETER_NOT_FOUND);
  }
}

void CgiExecutor::createPipes(primitives::PipeDescriptors& pipes) {
  int stdinPipe[2];
  int stdoutPipe[2];
  int stderrPipe[2];

  if (pipe(stdinPipe) == -1) {
    throw exceptions::CgiExecutionException(
        "Failed to create stdin pipe: " + getErrorMessage(errno),
        exceptions::CgiExecutionException::PIPE_FAILED);
  }

  if (pipe(stdoutPipe) == -1) {
    close(stdinPipe[0]);
    close(stdinPipe[1]);
    throw exceptions::CgiExecutionException(
        "Failed to create stdout pipe: " + getErrorMessage(errno),
        exceptions::CgiExecutionException::PIPE_FAILED);
  }

  if (pipe(stderrPipe) == -1) {
    close(stdinPipe[0]);
    close(stdinPipe[1]);
    close(stdoutPipe[0]);
    close(stdoutPipe[1]);
    throw exceptions::CgiExecutionException(
        "Failed to create stderr pipe: " + getErrorMessage(errno),
        exceptions::CgiExecutionException::PIPE_FAILED);
  }

  pipes.setStdinReadFd(stdinPipe[0]);
  pipes.setStdinWriteFd(stdinPipe[1]);
  pipes.setStdoutReadFd(stdoutPipe[0]);
  pipes.setStdoutWriteFd(stdoutPipe[1]);
  pipes.setStderrReadFd(stderrPipe[0]);
  pipes.setStderrWriteFd(stderrPipe[1]);

  setCloseOnExec(pipes.getStdinWriteFd());
  setCloseOnExec(pipes.getStdoutReadFd());
  setCloseOnExec(pipes.getStderrReadFd());
}

pid_t CgiExecutor::forkProcess() {
  pid_t pid = fork();

  if (pid == -1) {
    throw exceptions::CgiExecutionException(
        "Failed to fork process: " + getErrorMessage(errno),
        exceptions::CgiExecutionException::FORK_FAILED);
  }

  return pid;
}

void CgiExecutor::executeChildProcess(
    const primitives::CgiRequest& request,
    const primitives::PipeDescriptors& pipes) const {
  try {
    primitives::PipeDescriptors childPipes = pipes;
    childPipes.closeUnusedInChild();
    redirectChildStreams(childPipes);
    executeScript(request);

  } catch (const std::exception& ex) {
    m_logger.error(std::string("CGI child process error: ") + ex.what());
    std::exit(EXIT_FAILURE);
  } catch (...) {
    m_logger.error("CGI child process unknown error");
    std::exit(EXIT_FAILURE);
  }
}

void CgiExecutor::redirectChildStreams(
    const primitives::PipeDescriptors& pipes) {
  if (dup2(pipes.getStdinReadFd(), STDIN_FILENO) == -1) {
    throw exceptions::CgiExecutionException(
        "Failed to redirect stdin: " + getErrorMessage(errno),
        exceptions::CgiExecutionException::PIPE_FAILED);
  }

  if (dup2(pipes.getStdoutWriteFd(), STDOUT_FILENO) == -1) {
    throw exceptions::CgiExecutionException(
        "Failed to redirect stdout: " + getErrorMessage(errno),
        exceptions::CgiExecutionException::PIPE_FAILED);
  }

  if (dup2(pipes.getStderrWriteFd(), STDERR_FILENO) == -1) {
    throw exceptions::CgiExecutionException(
        "Failed to redirect stderr: " + getErrorMessage(errno),
        exceptions::CgiExecutionException::PIPE_FAILED);
  }

  close(pipes.getStdinReadFd());
  close(pipes.getStdoutWriteFd());
  close(pipes.getStderrWriteFd());
}

void CgiExecutor::executeScript(const primitives::CgiRequest& request) const {
  std::vector<std::string> argv = request.buildArgv();
  std::vector<char*> envp = request.buildEnvp();

  std::vector<char*> argvPtrs;
  argvPtrs.reserve(argv.size() + 1);
  for (std::size_t i = 0; i < argv.size(); ++i) {
    argvPtrs.push_back(const_cast<char*>(argv[i].c_str()));
  }
  argvPtrs.push_back(NULL);

  if (execve(argvPtrs[0], &argvPtrs[0], &envp[0]) == -1) {
    std::string error =
        "Failed to execute CGI script: " + request.getScriptPath() + " - " +
        getErrorMessage(errno);
    m_logger.error(error);
    std::exit(EXIT_FAILURE);
  }
}

void CgiExecutor::writeRequestBody(int fileDescriptor,
                                   const std::vector<char>& body) {
  if (body.empty()) {
    return;
  }

  std::size_t totalWritten = 0;
  while (totalWritten < body.size()) {
    ssize_t written =
        write(fileDescriptor, &body[totalWritten], body.size() - totalWritten);

    if (written == -1) {
      if (errno == EINTR) {
        continue;
      }
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        usleep(1000);  // TODO: remove magic number
        continue;
      }
      throw exceptions::CgiExecutionException(
          "Failed to write request body: " + getErrorMessage(errno),
          exceptions::CgiExecutionException::PIPE_FAILED);
    }

    totalWritten += static_cast<std::size_t>(written);
  }
}

std::vector<char> CgiExecutor::readFromPipe(int fileDescriptor,
                                            std::size_t maxSize) {
  std::vector<char> data;
  data.reserve(PIPE_BUFFER_SIZE);

  char buffer[PIPE_BUFFER_SIZE];
  while (data.size() < maxSize) {
    ssize_t bytesRead = read(fileDescriptor, buffer, PIPE_BUFFER_SIZE);

    if (bytesRead == -1) {
      if (errno == EINTR) {
        continue;
      }
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      throw exceptions::CgiExecutionException(
          "Failed to read from pipe: " + getErrorMessage(errno),
          exceptions::CgiExecutionException::PIPE_FAILED);
    }

    if (bytesRead == 0) {
      break;
    }

    data.insert(data.end(), buffer, buffer + bytesRead);

    if (data.size() >= maxSize) {
      break;
    }
  }

  return data;
}

bool CgiExecutor::waitForChildWithTimeout(pid_t childPid, int& status,
                                          unsigned int timeoutSeconds) {
  struct timeval startTime;
  gettimeofday(&startTime, NULL);

  const unsigned int checkIntervalUs = 100000;

  while (true) {
    pid_t result = waitpid(childPid, &status, WNOHANG);

    if (result == -1) {
      if (errno == EINTR) {
        continue;
      }
      throw exceptions::CgiExecutionException(
          "waitpid failed: " + getErrorMessage(errno),
          exceptions::CgiExecutionException::PROCESS_ERROR);
    }

    if (result == childPid) {
      return true;
    }

    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    unsigned int elapsedSeconds =
        static_cast<unsigned int>(currentTime.tv_sec - startTime.tv_sec);

    if (elapsedSeconds >= timeoutSeconds) {
      return false;
    }

    usleep(checkIntervalUs);
  }
}

void CgiExecutor::killProcessGroup(pid_t pid) {
  kill(-pid, SIGTERM);
  usleep(100000);  // TODO: remove magic number
  kill(-pid, SIGKILL);
}

// TODO: refactor this function
primitives::CgiExecutionContext CgiExecutor::runChildProcess(
    const primitives::CgiRequest& request) {
  primitives::CgiExecutionContext context;

  createPipes(context.getPipes());

  setNonBlocking(context.getPipes().getStdoutReadFd());
  setNonBlocking(context.getPipes().getStderrReadFd());

  pid_t childPid = forkProcess();
  context.setChildPid(childPid);

  if (childPid == 0) {
    executeChildProcess(request, context.getPipes());
    std::exit(EXIT_SUCCESS);
  }

  context.getPipes().closeUnusedInParent();

  try {
    writeRequestBody(context.getPipes().getStdinWriteFd(),
                     request.getRequestBody());
  } catch (const std::exception& ex) {
    m_logger.error(std::string("Failed to write request body: ") + ex.what());
  }
  context.getPipes().closeStdinWrite();

  int exitStatus;
  bool finished =
      waitForChildWithTimeout(childPid, exitStatus, m_timeoutSeconds);
  context.setExitStatus(exitStatus);

  if (!finished) {
    context.setTimedOut(true);
    handleTimeout(context);
  } else {
    handleChildExit(context);
  }

  try {
    std::vector<char> output =
        readFromPipe(context.getPipes().getStdoutReadFd(), m_maxOutputSize);
    context.setOutput(output);
  } catch (const std::exception& ex) {
    m_logger.error(std::string("Failed to read stdout: ") + ex.what());
  }

  try {
    std::vector<char> errorOutput =
        readFromPipe(context.getPipes().getStderrReadFd(), m_maxOutputSize);
    context.setErrorOutput(errorOutput);
  } catch (const std::exception& ex) {
    m_logger.error(std::string("Failed to read stderr: ") + ex.what());
  }

  context.getPipes().closeAll();

  return context;
}

void CgiExecutor::handleChildExit(
    const primitives::CgiExecutionContext& context) const {
  if (context.hasExited()) {
    int exitCode = context.getExitCode();
    if (exitCode != 0) {
      std::ostringstream oss;
      oss << "CGI script exited with code " << exitCode;
      m_logger.warn(oss.str());
    }
  } else if (context.wasSignaled()) {
    int signal = context.getTermSignal();
    std::ostringstream oss;
    oss << "CGI script terminated by signal " << signal;
    m_logger.error(oss.str());
  }
}

void CgiExecutor::handleTimeout(
    primitives::CgiExecutionContext& context) const {
  m_logger.error("CGI script execution timeout - killing process");

  killProcessGroup(context.getChildPid());

  int status;
  waitpid(context.getChildPid(), &status, 0);
  context.setExitStatus(status);
}

primitives::CgiResponse CgiExecutor::buildResponse(
    const primitives::CgiExecutionContext& context) {
  if (context.hasTimedOut()) {
    throw exceptions::CgiExecutionException(
        "CGI script execution timeout",
        exceptions::CgiExecutionException::TIMEOUT);
  }

  if (context.hasExited()) {
    int exitCode = context.getExitCode();
    if (exitCode != 0 && exitCode != EXIT_SUCCESS) {
      std::ostringstream oss;
      oss << "CGI script failed with exit code " << exitCode;
      if (!context.getErrorOutput().empty()) {
        std::string errorStr(context.getErrorOutput().begin(),
                             context.getErrorOutput().end());
        oss << ": " << errorStr;
      }
      throw exceptions::CgiExecutionException(
          oss.str(), exceptions::CgiExecutionException::PROCESS_ERROR);
    }
  }

  if (context.getOutput().empty()) {
    throw exceptions::CgiExecutionException(
        "CGI script produced no output",
        exceptions::CgiExecutionException::INVALID_OUTPUT);
  }

  return primitives::CgiResponse::fromRawOutput(context.getOutput());
}

bool CgiExecutor::isFileExecutable(const std::string& path) {
  struct stat bufStat;
  if (stat(path.c_str(), &bufStat) != 0) {
    return false;
  }

  return (bufStat.st_mode & S_IXUSR) != 0 || (bufStat.st_mode & S_IXGRP) != 0 ||
         (bufStat.st_mode & S_IXOTH) != 0;
}

bool CgiExecutor::fileExists(const std::string& path) {
  struct stat bufStat;
  return stat(path.c_str(), &bufStat) == 0;
}

void CgiExecutor::setNonBlocking(int fileDescriptor) {
  int flags = fcntl(fileDescriptor, F_GETFL, 0);
  if (flags == -1) {
    return;
  }
  fcntl(fileDescriptor, F_SETFL, flags | O_NONBLOCK);
}

void CgiExecutor::setCloseOnExec(int fileDescriptor) {
  int flags = fcntl(fileDescriptor, F_GETFD, 0);
  if (flags == -1) {
    return;
  }
  fcntl(fileDescriptor, F_SETFD, flags | FD_CLOEXEC);
}

std::string CgiExecutor::getErrorMessage(int errnoValue) {
  return std::string(std::strerror(errnoValue));
}

}  // namespace adapters
}  // namespace cgi
}  // namespace infrastructure
