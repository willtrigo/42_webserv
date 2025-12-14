/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TerminalColor.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 17:09:31 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/12 17:34:46 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TERMINAL_COLOR_HPP
#define TERMINAL_COLOR_HPP

#include <string>
#include <utility>

enum StrColor {
  BLACK,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  PURPLE,
  CYAN,
  WHITE,
  ORANGE,
  RESET
};

enum BgColor {
  BG_BLACK,
  BG_RED,
  BG_GREEN,
  BG_YELLOW,
  BG_BLUE,
  BG_PURPLE,
  BG_CYAN,
  BG_WHITE,
  BG_ORANGE,
  BG_RESET
};

enum Style { BOLD, ITALIC, UNDERLINE };

class TerminalColor {
 public:
  static std::string getColor(StrColor strColor);
  static std::string getColor(BgColor bgColor);
  static std::string getColor(Style style);
  static std::string setColor(StrColor strColor, const std::string& str);
  static std::string setColor(BgColor bgColor, const std::string& str);
  static std::string setColor(Style style, const std::string& str);
  static std::string setColor(BgColor bgColor, StrColor strColor,
                              const std::string& str);
  static std::string setColor(BgColor bgColor, Style style,
                              const std::string& str);
  static std::string setColor(StrColor strColor, Style style,
                              const std::string& str);
  static std::string setColor(BgColor bgColor, StrColor strColor, Style style,
                              const std::string& str);

 private:
  TerminalColor(const TerminalColor&);
  ~TerminalColor();

  TerminalColor& operator=(const TerminalColor&);

  static const std::pair<StrColor, std::string> STR_COLOR_MAP[];
  static const std::pair<BgColor, std::string> BG_COLOR_MAP[];
  static const std::pair<Style, std::string> STYLE_MAP[];
};

#endif  // TERMINAL_COLOR_HPP
