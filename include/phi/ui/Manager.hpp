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
#include <tuple>

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
#include "phi/ui/constants.hpp"

//---------> [ Config. Separator ] <---------\\ 

namespace phi::ui {

constexpr int COLS = 150;
constexpr int ROWS = 35;

// winsize.ws_row and winsize.ws_col
inline struct winsize getTerminalSize() {
  struct winsize size{};
  ioctl(STDOUT_FILENO, TIOCGSIZE, &size);
  return size;
}

//================={ Header Item Separator }=================\\ 

struct Components {
    ftxui::Component login_input;

    ftxui::Component home_button_layout;

    ftxui::Component contacts_menu;
    ftxui::Component contact_page;

    ftxui::Component contact_request_menu;
    ftxui::Component error_menu;
};

//================={ Header Item Separator }=================\\ 

enum class Page : unsigned char {
  Login,
  Home,
  ContactsMenu,
  EditContact,
  ContactDoesNotExist,
  ContactRequestsMenu,
  ConversationsMenu,
  ViewErrorsMenu,
  EditSelf,
  ChangeDatabasePassword,
  Settings,
  Screensaver
};

//================={ Header Item Separator }=================\\ 

struct Notification {
    bool show;

    std::string title;
    std::string description;

    std::chrono::time_point<std::chrono::steady_clock> expires;

    //=====[ Declaration Separator ]=====\\ 

    void reset() {
      this->show = false;
      this->title = "";
      this->description = "";
      this->expires = std::chrono::steady_clock::now();
    }
};

//================={ Header Item Separator }=================\\ 

struct State {
    Page page;

    Notification noti;
};

//================={ Header Item Separator }=================\\ 

class Manager {
  private:
    std::shared_ptr<phi::database::Database> DATABASE;
    std::shared_ptr<phi::encryption::Encryptor> ENCRYPTOR;
    std::shared_ptr<phi::tasks::TaskMaster> TASKMASTER;

    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::TerminalOutput();

    State state{Page::Login, {false, "", "", {}}};
    Components components;

    //=====[ Declaration Separator ]=====\\ 

    std::tuple<std::vector<std::string>, std::vector<int>> getContacts();

    void rebuildRoot(ftxui::Component& root) const;

    void loadComponents();

    //=====[ Declaration Separator ]=====\\ 

    bool should_exit = false;
    ftxui::Component root = ftxui::Container::Vertical({});

    std::string password;

  public:
    Manager(std::shared_ptr<phi::database::Database> database,
            std::shared_ptr<phi::encryption::Encryptor> encryptor,
            std::shared_ptr<phi::tasks::TaskMaster> taskmaster);

    //=====[ Declaration Separator ]=====\\ 

    void eventLoop();

    // functions below are inside of `Manager_render_functions.cpp`
    ftxui::Element renderLoginUI() const;
    ftxui::Element renderHomeUI() const;
    ftxui::Element renderContactsMenuUI() const;
    ftxui::Element renderContactPageUI(int contact_id) const;
    ftxui::Element contactDoesNotExist() const;
    ftxui::Element renderScreensaver() const;  // not this one, `Manager_screensaver.cpp`
    ftxui::Element renderNotification() const;

    // functions below are inside of `Manager_components.cpp`
    void createLoginInput();
};

}  // namespace phi::ui

#endif /* MANAGER_HPP */
