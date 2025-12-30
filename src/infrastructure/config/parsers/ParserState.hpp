/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserState.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 12:55:08 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 14:44:56 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_STATE_HPP
#define PARSER_STATE_HPP

#include <string>

namespace infrastructure {
namespace config {
namespace parser {

class ParserState {
 public:
  enum Context { GLOBAL, HTTP, SERVER, LOCATION };

  Context context;
  std::string blockName;
  std::size_t startLine;

  ParserState(Context ctx, const std::string& name = "", std::size_t line = 0);

  std::string contextToString() const;
  bool isInBlock() const;
};

}  // namespace parser
}  // namespace config
}  // namespace infrastructure

#endif  // PARSER_STATE_HPP
