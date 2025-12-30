/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserState.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 14:45:08 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/30 14:46:25 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/config/parsers/ParserState.hpp"

namespace infrastructure {
namespace config {
namespace parser {

ParserState::ParserState(Context ctx, const std::string& name, std::size_t line)
    : context(ctx), blockName(name), startLine(line) {}

std::string ParserState::contextToString() const {
  switch (context) {
    case GLOBAL: return "global";
    case HTTP: return "http";
    case SERVER: return "server";
    case LOCATION: return "location";
    default: return "unknown";
  }
}

bool ParserState::isInBlock() const {
  return context != GLOBAL;
}

}  // namespace parser
}  // namespace config
}  // namespace infrastructure
