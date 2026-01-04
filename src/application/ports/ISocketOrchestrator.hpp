/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ISocketOrchestrator.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 21:53:04 by dande-je          #+#    #+#             */
/*   Updated: 2026/01/03 06:38:25 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISOCKET_ORCHESTRATOR_HPP
#define ISOCKET_ORCHESTRATOR_HPP

#include <cstddef>

namespace application {
namespace ports {

class ISocketOrchestrator {
 public:
  virtual ~ISocketOrchestrator() {}

  virtual void initialize() = 0;
  virtual void run() = 0;
  virtual void shutdown() = 0;
  virtual bool isRunning() const = 0;

  virtual size_t getActiveConnectionCount() const = 0;
};

}  // namespace ports
}  // namespace application

#endif  // ISOCKETORCHESTRATOR_HPP
