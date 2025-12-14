/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 17:36:08 by dande-je          #+#    #+#             */
/*   Updated: 2025/12/13 23:13:36 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infrastructure/io/StreamWriter.hpp"
#include "presentation/cli/CliController.hpp"
#include "presentation/cli/CliView.hpp"

#include <cstdlib>

int main(int argc, char** argv) {
  infrastructure::io::StreamWriter writer;

  CliView view(writer);
  CliController controller(view);
  
  return controller.run(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
