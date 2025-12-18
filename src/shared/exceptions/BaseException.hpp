/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 11:22:20 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/18 12:34:39 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASE_EXCEPTION_HPP
#define BASE_EXCEPTION_HPP

#include <exception>
#include <string>

namespace shared {
namespace exceptions {

class BaseException : public std::exception {
 public:
  virtual ~BaseException() throw();
  virtual const char* what() const throw();

 protected:
  BaseException(const std::string& msg, int code);

  mutable std::string m_whatMsg;

 private:
  BaseException(const BaseException&);

  BaseException& operator=(const BaseException&);
};

}  // namespace exceptions
}  // namespace shared

#endif  // BASE_EXCEPTION_HPP
