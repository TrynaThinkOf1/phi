/*

 PHI
 2026/01/02

 Phi C++ Project
 src/phi/Manager_render_functions.cpp

 Zevi Berlin

*/

#include "phi/ui/Manager.hpp"

ftxui::Element phi::ui::Manager::renderLoginUI() const {
  auto header = ftxui::paragraphAlignCenter(phi::ui::constants::header_text);
  header |= ftxui::bold;

  auto header_box = ftxui::vbox(ftxui::filler(), header, ftxui::filler());
  header_box = ftxui::hbox(ftxui::filler(), header_box, ftxui::filler()) |
               ftxui::bgcolor(phi::ui::colors::PURPLE_HAZE) | ftxui::borderRounded |
               ftxui::color(phi::ui::colors::GOLD) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 104) |
               ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 20);

  return ftxui::vbox({ftxui::separatorEmpty(), ftxui::separatorEmpty(), header_box | ftxui::center,
                      ftxui::separatorEmpty(),
                      this->components.login_input->Render() |
                        ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20) | ftxui::center}) |
         ftxui::size(ftxui::WIDTH, ftxui::EQUAL, phi::ui::COLS) |
         ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, phi::ui::ROWS) |
         ftxui::bgcolor(phi::ui::colors::DEEP) | ftxui::color(phi::ui::colors::SHALLOW) |
         ftxui::borderRounded | ftxui::center;
}

/**/

ftxui::Element phi::ui::Manager::renderHomeUI() const {
  auto logo = ftxui::paragraphAlignCenter(phi::ui::constants::logo_text);
  logo |= ftxui::bold;
  logo |= ftxui::bgcolor(phi::ui::colors::PURPLE_HAZE);
  logo |= ftxui::borderHeavy;
  logo |= ftxui::color(phi::ui::colors::GOLD);

  auto left_buttons = this->components.home_button_layout_left->Render() |
                      ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 32);
  auto left_column = ftxui::vbox({ftxui::filler(), left_buttons | ftxui::center, ftxui::filler()}) |
                     ftxui::bgcolor(phi::ui::colors::PURPLE_HAZE) |
                     ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 34) |
                     ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, phi::ui::ROWS) | ftxui::borderHeavy |
                     ftxui::color(phi::ui::colors::GOLD);

  return ftxui::hbox({ftxui::filler(), left_column, ftxui::filler(), ftxui::filler(), logo}) |
         ftxui::size(ftxui::WIDTH, ftxui::EQUAL, phi::ui::COLS) |
         ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, phi::ui::ROWS) |
         ftxui::color(phi::ui::colors::PURPLE_HAZE) | ftxui::bgcolor(phi::ui::colors::DEEP) |
         ftxui::borderRounded | ftxui::color(phi::ui::colors::SHALLOW) | ftxui::center;
}

/**/

ftxui::Element phi::ui::Manager::renderContactMenuUI() const {
  std::unique_ptr<std::vector<std::tuple<int, std::string, std::string>>> actuals =
    DATABASE->getAllContacts();
  std::vector<std::string> contacts(actuals->size());

  if (actuals != nullptr) {  // if it is a nullptr, well the menu is empty anyway
    for (size_t i = 0; i < actuals->size(); i++) {
      auto tup = actuals->at(i);

      contacts[i] =
        std::get<2>(tup) + " " + std::get<1>(tup) + " (" + std::to_string(std::get<0>(tup)) + ")";
    }
  }

  return ftxui::vbox({});
}