/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TerminalColor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 17:09:41 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/28 15:24:24 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/utils/TerminalColor.hpp" // TODO: moving to infrastructure/logging

#include <stdexcept>
#include <utility>

const std::pair<StrColor, std::string> TerminalColor::STR_COLOR_MAP[] = {
    std::make_pair(BLACK, "\033[30m"),
    std::make_pair(RED, "\033[31m"),
    std::make_pair(GREEN, "\033[32m"),
    std::make_pair(YELLOW, "\033[33m"),
    std::make_pair(BLUE, "\033[34m"),
    std::make_pair(PURPLE, "\033[35m"),
    std::make_pair(CYAN, "\033[36m"),
    std::make_pair(WHITE, "\033[37m"),
    std::make_pair(ORANGE, "\x1B[38;5;202m"),
    std::make_pair(RESET, "\033[0m"),
};

const std::pair<BgColor, std::string> TerminalColor::BG_COLOR_MAP[] = {
    std::make_pair(BG_BLACK, "\033[40m"),
    std::make_pair(BG_RED, "\033[41m"),
    std::make_pair(BG_GREEN, "\033[42m"),
    std::make_pair(BG_YELLOW, "\033[43m"),
    std::make_pair(BG_BLUE, "\033[44m"),
    std::make_pair(BG_PURPLE, "\033[45m"),
    std::make_pair(BG_CYAN, "\033[46m"),
    std::make_pair(BG_WHITE, "\033[47m"),
    std::make_pair(BG_ORANGE, "\x1B[48;5;202m"),
    std::make_pair(BG_RESET, "\033[49m"),
};

const std::pair<Style, std::string> TerminalColor::STYLE_MAP[] = {
    std::make_pair(BOLD, "\033[1m"),
    std::make_pair(ITALIC, "\033[3m"),
    std::make_pair(UNDERLINE, "\033[4m"),
};

TerminalColor::TerminalColor(const TerminalColor& /*unused*/) {}

TerminalColor::~TerminalColor() {}

TerminalColor& TerminalColor::operator=(const TerminalColor& /*unused*/) {
  throw std::runtime_error("TerminalColor assignment is not allowed");
}

std::string TerminalColor::getColor(StrColor strColor) {
  return STR_COLOR_MAP[strColor].second;
}

std::string TerminalColor::getColor(BgColor bgColor) {
  return BG_COLOR_MAP[bgColor].second;
}

std::string TerminalColor::getColor(Style style) {
  return STYLE_MAP[style].second;
}

std::string TerminalColor::setColor(StrColor strColor, const std::string& str) {
  std::string result;

  result += STR_COLOR_MAP[strColor].second;
  result += str;
  result += STR_COLOR_MAP[RESET].second;
  return result;
}

std::string TerminalColor::setColor(BgColor bgColor, const std::string& str) {
  std::string result;

  result += BG_COLOR_MAP[bgColor].second;
  result += str;
  result += STR_COLOR_MAP[RESET].second;
  return result;
}

std::string TerminalColor::setColor(Style style, const std::string& str) {
  std::string result;

  result += STYLE_MAP[style].second;
  result += str;
  result += STR_COLOR_MAP[RESET].second;
  return result;
}

std::string TerminalColor::setColor(BgColor bgColor, StrColor strColor,
                                    const std::string& str) {
  std::string result;

  result += BG_COLOR_MAP[bgColor].second;
  result += STR_COLOR_MAP[strColor].second;
  result += str;
  result += STR_COLOR_MAP[RESET].second;
  return result;
}

std::string TerminalColor::setColor(StrColor strColor, Style style,
                                    const std::string& str) {
  std::string result;

  result += STYLE_MAP[style].second;
  result += STR_COLOR_MAP[strColor].second;
  result += str;
  result += STR_COLOR_MAP[RESET].second;
  return result;
}

std::string TerminalColor::setColor(BgColor bgColor, StrColor strColor,
                                    Style style, const std::string& str) {
  std::string result;

  result += STYLE_MAP[style].second;
  result += BG_COLOR_MAP[bgColor].second;
  result += STR_COLOR_MAP[strColor].second;
  result += str;
  result += STR_COLOR_MAP[RESET].second;
  return result;
}
