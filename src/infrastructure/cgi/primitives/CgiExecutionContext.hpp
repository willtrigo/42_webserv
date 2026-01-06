/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExecutionContext.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 01:18:57 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 01:19:08 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_EXECUTION_CONTEXT_HPP
#define CGI_EXECUTION_CONTEXT_HPP

#include "infrastructure/cgi/primitives/PipeDescriptors.hpp"

#include <sys/types.h>
#include <vector>

namespace infrastructure {
namespace cgi {
namespace primitives {

class CgiExecutionContext {
 public:
  CgiExecutionContext();
  ~CgiExecutionContext();

  CgiExecutionContext(const CgiExecutionContext& other);
  CgiExecutionContext& operator=(const CgiExecutionContext& other);

  pid_t getChildPid() const;
  void setChildPid(pid_t pid);

  PipeDescriptors& getPipes();
  const PipeDescriptors& getPipes() const;
  void setPipes(const PipeDescriptors& pipes);

  const std::vector<char>& getOutput() const;
  void setOutput(const std::vector<char>& output);
  void appendOutput(const std::vector<char>& data);
  void clearOutput();

  const std::vector<char>& getErrorOutput() const;
  void setErrorOutput(const std::vector<char>& errorOutput);
  void appendErrorOutput(const std::vector<char>& data);
  void clearErrorOutput();

  int getExitStatus() const;
  void setExitStatus(int status);

  bool hasTimedOut() const;
  void setTimedOut(bool timedOut);

  bool isChildRunning() const;
  bool hasExited() const;
  bool wasSignaled() const;
  int getExitCode() const;
  int getTermSignal() const;

  void reset();

 private:
  pid_t m_childPid;
  PipeDescriptors m_pipes;
  std::vector<char> m_output;
  std::vector<char> m_errorOutput;
  int m_exitStatus;
  bool m_timedOut;

  static const pid_t INVALID_PID = -1;
  static const int INVALID_STATUS = -1;

  void copyFrom(const CgiExecutionContext& other);
};

}  // namespace primitives
}  // namespace cgi
}  // namespace infrastructure

#endif  // CGI_EXECUTION_CONTEXT_HP
