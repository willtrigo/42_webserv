/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PipeDescriptors.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 01:11:49 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/06 01:17:26 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPE_DESCRIPTORS_HPP
#define PIPE_DESCRIPTORS_HPP

namespace infrastructure {
namespace cgi {
namespace primitives {

class PipeDescriptors {
 public:
  PipeDescriptors();
  ~PipeDescriptors();

  PipeDescriptors(const PipeDescriptors& other);
  PipeDescriptors& operator=(const PipeDescriptors& other);

  int getStdinReadFd() const;
  int getStdinWriteFd() const;
  int getStdoutReadFd() const;
  int getStdoutWriteFd() const;
  int getStderrReadFd() const;
  int getStderrWriteFd() const;

  void setStdinReadFd(int fileDescriptor);
  void setStdinWriteFd(int fileDescriptor);
  void setStdoutReadFd(int fileDescriptor);
  void setStdoutWriteFd(int fileDescriptor);
  void setStderrReadFd(int fileDescriptor);
  void setStderrWriteFd(int fileDescriptor);

  bool isStdinReadOpen() const;
  bool isStdinWriteOpen() const;
  bool isStdoutReadOpen() const;
  bool isStdoutWriteOpen() const;
  bool isStderrReadOpen() const;
  bool isStderrWriteOpen() const;

  void closeStdinRead();
  void closeStdinWrite();
  void closeStdoutRead();
  void closeStdoutWrite();
  void closeStderrRead();
  void closeStderrWrite();

  void closeAll();
  void closeUnusedInChild();
  void closeUnusedInParent();

  void invalidateAll();

 private:
  int m_stdinPipe[2];
  int m_stdoutPipe[2];
  int m_stderrPipe[2];

  static const int INVALID_FD = -1;
  static const int READ_END = 0;
  static const int WRITE_END = 1;

  static void closeFd(int& fileDescriptor);
  void copyFrom(const PipeDescriptors& other);
};

}  // namespace primitives
}  // namespace cgi
}  // namespace infrastructure

#endif  // PIPE_DESCRIPTORS_HPP
