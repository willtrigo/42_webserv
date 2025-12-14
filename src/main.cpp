/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 17:36:08 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/14 20:13:16 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/io/FileWriter.hpp"
#include "infrastructure/io/StreamWriter.hpp"
#include "infrastructure/logging/Logger.hpp"
#include "presentation/cli/CliController.hpp"
#include "presentation/cli/CliView.hpp"

#include <cstdlib>

int main(int argc, char** argv) {
  infrastructure::io::StreamWriter consoleWriter;
  infrastructure::io::FileWriter logFile("webserv.log", std::ios::app | std::ios::out);
  infrastructure::logging::Logger logger(consoleWriter, logFile);

  CliView view(logger);
  CliController controller(view);
  
  return controller.run(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
