/*

 PHI
 2026/01/02

 Phi C++ Project
 src/phi/Manager_login_page.cpp

 Zevi Berlin

*/

#include "phi/ui/Manager.hpp"

const std::string header_text = R"ascii(_     ___   ____ ___ _   _ 
| |   / _ \ / ___|_ _| \ | |
| |  | | | | |  _ | ||  \| |
| |__| |_| | |_| || || |\  |
|_____\___/ \____|___|_| \_|
)ascii";

ftxui::Element phi::ui::Manager::renderLoginUI() {
  auto header = ftxui::paragraphAlignCenter(header_text);
  header |= ftxui::bold;
  header |= ftxui::borderRounded;
  header |= ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 31);
  header |= ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 8);

  return ftxui::vbox({ftxui::separatorEmpty(), ftxui::separatorEmpty(),
                      header | ftxui::color(phi::ui::colors::PURPLE_LIGHT) | ftxui::center,
                      ftxui::separatorEmpty(),
                      this->components.login_input->Render() |
                        ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20) | ftxui::center}) |
         ftxui::size(ftxui::WIDTH, ftxui::EQUAL, phi::ui::COLS) |
         ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, phi::ui::ROWS) | ftxui::borderRounded |
         ftxui::center;
}

/**/

ftxui::Element phi::ui::Manager::renderHomeUI() {
  return ftxui::vbox({}) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, phi::ui::COLS) |
         ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, phi::ui::ROWS) | ftxui::borderRounded |
         ftxui::center;
}
