/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StreamWriter.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 17:25:43 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/14 12:58:22 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STREAM_WRITER_HPP
#define STREAM_WRITER_HPP

#include "application/ports/IStreamWriter.hpp"

#include <iostream>
#include <string>

namespace infrastructure {
namespace io {

class StreamWriter : public application::ports::IStreamWriter {
 public:
  StreamWriter();
  StreamWriter(const StreamWriter&);
  virtual ~StreamWriter();

  StreamWriter& operator=(const StreamWriter&);

  virtual void print(std::ostream& ostr, const std::string& str,
                     bool newLine) const;
  virtual void flush();
};

}  // namespace io
}  // namespace infrastructure

#endif  // STREAM_WRITER_HPP
