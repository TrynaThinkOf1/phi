/*

 PHI
 2026/01/01

 Phi C++ Project
 src/phi/Manager.cpp

 Zevi Berlin

*/

#include "phi/ui/Manager.hpp"

#include <memory>

#include "phi/database/Database.hpp"
#include "phi/encryption/Encryptor.hpp"
#include "tasks/TaskMaster.hpp"

phi::ui::Manager::Manager(std::shared_ptr<phi::database::Database> database,
                          std::shared_ptr<phi::encryption::Encryptor> encryptor,
                          std::shared_ptr<phi::tasks::TaskMaster> taskmaster)
    : DATABASE(std::move(database)),
      ENCRYPTOR(std::move(encryptor)),
      TASKMASTER(std::move(taskmaster)) {
}

/***/

void phi::ui::Manager::eventLoop() {
  bool should_exit = false;

  ftxui::InputOption popt;
  popt.password = true;

  std::string password;
  this->components.login_input = ftxui::Input(&password, "", popt);
  this->components.login_input |= ftxui::CatchEvent([&](const ftxui::Event& e) {
    if (e == ftxui::Event::Return) {
      if (DATABASE->login(password)) {
        this->state.page = phi::ui::Page::Home;
      } else {
        should_exit = true;
      }
      return true;
    }

    return false;
  });


  auto total_layout = ftxui::Container::Vertical({this->components.toVev()});
  auto renderer = ftxui::Renderer(total_layout, [&] {
    if (should_exit) this->screen.Exit();

    switch (this->state.page) {
      case Page::Login:
        return this->renderLoginUI();
      case Page::Home:
        return this->renderHomeUI();
    }
  });
  this->screen.Loop(renderer);
}