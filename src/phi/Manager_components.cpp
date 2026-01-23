/*

 PHI
 2026/01/23

 Phi C++ Project
 src/phi/Manager_components.cpp

 Zevi Berlin

*/

#include "phi/ui/Manager.hpp"

//---------> [ Config. Separator ] <---------\\ 

void phi::ui::Manager::createLoginInput() {
  ftxui::InputOption popt;
  popt.password = true;

  this->components.login_input = ftxui::Input(&(this->password), "", popt);
  this->components.login_input |= ftxui::CatchEvent([&](const ftxui::Event& e) {
    if (e == ftxui::Event::Return) {
      if (this->DATABASE->login(this->password)) {
        this->DATABASE->createTables();
        this->state.page = phi::ui::Page::Home;
        this->rebuildRoot(this->root);
      } else {
        this->should_exit = true;
      }
      return true;
    }

    return false;
  });
}