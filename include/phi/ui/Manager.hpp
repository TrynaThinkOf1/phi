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
#include <chrono>

#include <sys/ioctl.h>
#include <unistd.h>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

#include "phi/database/Database.hpp"
#include "phi/database/structs.hpp"
#include "tasks/TaskMaster.hpp"
#include "tasks/task_struct.hpp"
#include "phi/encryption/Encryptor.hpp"
#include "phi/encryption/secrets.hpp"
#include "phi/ui/color_defs.hpp"
#include "phi/ui/utils.hpp"

namespace phi::ui {

constexpr int COLS = 150;
constexpr int ROWS = 35;

// winsize.ws_row and winsize.ws_col
inline struct winsize getTerminalSize() {
  struct winsize size{};
  ioctl(STDOUT_FILENO, TIOCGSIZE, &size);
  return size;
}

struct Components {
    ftxui::Component login_input;
    ftxui::Component contact_menu;

    std::vector<ftxui::Component> toVev() const {
      return {this->login_input, this->contact_menu};
    }
};

enum class Page { Login, Home };

struct State {
    Page page;
    bool show_noti;
    int noti_ls_milli;
};

class Manager {
  private:
    std::shared_ptr<phi::database::Database> DATABASE;
    std::shared_ptr<phi::encryption::Encryptor> ENCRYPTOR;
    std::shared_ptr<phi::tasks::TaskMaster> TASKMASTER;

    ftxui::ScreenInteractive screen =
      ftxui::ScreenInteractive::TerminalOutput();  //::FixedSize(COLS, ROWS);

    State state;
    Components components;

  public:
    Manager(std::shared_ptr<phi::database::Database> database,
            std::shared_ptr<phi::encryption::Encryptor> encryptor,
            std::shared_ptr<phi::tasks::TaskMaster> taskmaster);

    /**/

    void eventLoop();

    /**/

    // functions below are inside of `Manager_login_page.cpp`
    ftxui::Element renderLoginUI();
    ftxui::Element renderHomeUI();
};

}  // namespace phi::ui

#endif /* MANAGER_HPP */
