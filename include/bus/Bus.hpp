/*

 PHI
 2025/12/26

 Phi C++ Project
 include/phi/bus/Bus.hpp

 Zevi Berlin

*/

#ifndef BUS_HPP
#define BUS_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include "nlohmann/json.hpp"

#include "bus/tasks.hpp"
#include "utils.hpp"

using json = nlohmann::json;

namespace phi::bus {

/*
I've decided to go with a system that utilizes the atomic rewrite
  feature of Unix-like operating systems. Rather than establish a lock bit
  or use FLOCK, I'm going to just check that the file exists (therefore
  establish that connnection is unique) and then rename the file to the var
  PHI/D_LOCK_PATH and then edit from there.
*/
class Bus {
  private:
    task_t current_task{};
    int backup = 0;

    const std::string PATH = expand("~/.phi/bus.phi");
    std::string LOCK_PATH;

    /*
      erc: 0 if none, 1 if file couldn't be opened, 2 if rename didn't work
    */
    void _getFirstTask(int& erc);

  public:
    /*
      erc: 0 if none, 1 if file couldn't be opened, 2 if rename didn't work
    */
    Bus(bool is_phi, int& erc) {
      this->LOCK_PATH = (is_phi ? expand("~/.phi/bus.phi.lock") : expand("~/.phi/bus.phi.lockd"));

      _getFirstTask(erc);
    };

    /*
      erc: 0 if none, 1 if file couldn't be opened, 2 if rename didn't work, 3 if no tasks
    */
    bool nextTask(bool prev_scs, int& erc);

    /*
      erc: 0 if none, 1 if file couldn't be opened, 2 if rename didn't work
    */
    bool addTask(const task_t& task, int& erc);

    task_t getCurrentTask() const {
      return this->current_task;
    }
};

}  // namespace phi::bus

#endif /* BUS_HPP */