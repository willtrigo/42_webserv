/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExecutor.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 17:36:13 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 01:30:17 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_EXECUTOR_HPP
#define CGI_EXECUTOR_HPP

#include "application/ports/ILogger.hpp"
#include "infrastructure/cgi/primitives/CgiExecutionContext.hpp"
#include "infrastructure/cgi/primitives/CgiRequest.hpp"
#include "infrastructure/cgi/primitives/CgiResponse.hpp"

#include <string>
#include <vector>

namespace infrastructure {
namespace cgi {
namespace adapters {

class CgiExecutor {
 public:
  explicit CgiExecutor(application::ports::ILogger& logger);
  virtual ~CgiExecutor();

  CgiExecutor(const CgiExecutor& other);
  CgiExecutor& operator=(const CgiExecutor& other);

  primitives::CgiResponse execute(const primitives::CgiRequest& request);

  void setTimeout(unsigned int seconds);
  unsigned int getTimeout() const;

  void setMaxOutputSize(std::size_t bytes);
  std::size_t getMaxOutputSize() const;

  static const unsigned int DEFAULT_TIMEOUT_SECONDS = 30;
  static const std::size_t DEFAULT_MAX_OUTPUT_SIZE = 10485760;
  static const std::size_t PIPE_BUFFER_SIZE = 4096;

 private:
  application::ports::ILogger& m_logger;
  unsigned int m_timeoutSeconds;
  std::size_t m_maxOutputSize;

  static void validateRequest(const primitives::CgiRequest& request);
  static void validateScriptExecutability(const std::string& scriptPath);
  static void validateInterpreter(const std::string& interpreter);

  static void createPipes(primitives::PipeDescriptors& pipes);

  static pid_t forkProcess();
  void executeChildProcess(const primitives::CgiRequest& request,
                           const primitives::PipeDescriptors& pipes) const;
  static void redirectChildStreams(const primitives::PipeDescriptors& pipes);
  void executeScript(const primitives::CgiRequest& request) const;

  static void writeRequestBody(int fileDescriptor, const std::vector<char>& body);
  static std::vector<char> readFromPipe(int fileDescriptor, std::size_t maxSize);

  static bool waitForChildWithTimeout(pid_t childPid, int& status,
                               unsigned int timeoutSeconds);
  static void killProcessGroup(pid_t pid);

  primitives::CgiExecutionContext runChildProcess(
      const primitives::CgiRequest& request);
  void handleChildExit(const primitives::CgiExecutionContext& context) const;
  void handleTimeout(primitives::CgiExecutionContext& context) const;

  static primitives::CgiResponse buildResponse(
      const primitives::CgiExecutionContext& context);

  static bool isFileExecutable(const std::string& path);
  static bool fileExists(const std::string& path);
  static void setNonBlocking(int fileDescriptor);
  static void setCloseOnExec(int fileDescriptor);
  static std::string getErrorMessage(int errnoValue);
};

}  // namespace adapters
}  // namespace cgi
}  // namespace infrastructure

#endif  // CGI_EXECUTOR_HPP
