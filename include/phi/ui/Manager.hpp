/*

 PHI
 2026/01/01

 Phi C++ Project
 include/phi/ui/Manager.hpp

 Zevi Berlin

*/

#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <string>
#include <memory>
#include <vector>
#include <thread>

#include <sys/ioctl.h>
#include <unistd.h>

#include <ftxui/screen/screen.hpp>

#include "phi/database/Database.hpp"
#include "phi/database/structs.hpp"
#include "tasks/TaskMaster.hpp"
#include "tasks/task_struct.hpp"
#include "phi/encryption/Encryptor.hpp"
#include "phi/encryption/secrets.hpp"
#include "phi/ui/color_defs.hpp"

namespace phi::ui {

constexpr int COLS = 150;
constexpr int ROWS = 35;

// winsize.ws_row and winsize.ws_col
inline struct winsize getTerminalSize() {
  struct winsize size{};
  ioctl(STDOUT_FILENO, TIOCGSIZE, &size);
  return size;
}

class Manager {
  private:
    std::shared_ptr<phi::database::Database> DATABASE;
    std::shared_ptr<phi::encryption::Encryptor> ENCRYPTOR;
    std::shared_ptr<phi::tasks::TaskMaster> TASKMASTER;

    ftxui::Screen screen = ftxui::Screen::Create(ftxui::Dimension::Fixed(COLS), ftxui::Dimension::Fixed(ROWS));

  public:
    Manager(std::shared_ptr<phi::database::Database> database,
            std::shared_ptr<phi::encryption::Encryptor> encryptor,
            std::shared_ptr<phi::tasks::TaskMaster> taskmaster);

    /**/

    bool loginPage();
};

}  // namespace phi::ui

#endif /* MANAGER_HPP */
