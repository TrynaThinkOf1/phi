/*

 PHI
 2026/01/02

 Phi C++ Project
 src/phi/Manager_render_functions.cpp

 Zevi Berlin

*/

#include "phi/ui/Manager.hpp"

#define homebox                                                                          \
  ftxui::size(ftxui::WIDTH, ftxui::EQUAL, phi::ui::COLS) |                               \
    ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, phi::ui::ROWS) |                            \
    ftxui::color(phi::ui::colors::PURPLE_HAZE) | ftxui::bgcolor(phi::ui::colors::DEEP) | \
    ftxui::borderRounded | ftxui::color(phi::ui::colors::SHALLOW) | ftxui::center

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
         homebox;
}

/**/

ftxui::Element phi::ui::Manager::renderHomeUI() const {
  auto logo = ftxui::paragraphAlignCenter(phi::ui::constants::logo_text);
  logo |= ftxui::bold;
  logo |= ftxui::bgcolor(phi::ui::colors::PURPLE_HAZE);
  logo |= ftxui::borderHeavy;
  logo |= ftxui::color(phi::ui::colors::GOLD);

  auto left_buttons =
    this->components.home_button_layout->Render() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 32);
  auto left_column = ftxui::vbox({ftxui::filler(), left_buttons | ftxui::center, ftxui::filler()}) |
                     ftxui::bgcolor(phi::ui::colors::PURPLE_HAZE) |
                     ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 34) |
                     ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, phi::ui::ROWS) | ftxui::borderHeavy |
                     ftxui::color(phi::ui::colors::GOLD);

  return ftxui::hbox({ftxui::filler(), left_column, ftxui::filler(), ftxui::filler(), logo}) |
         homebox;
}

/**/

ftxui::Element phi::ui::Manager::renderContactsMenuUI() const {
  return ftxui::vbox(
           {ftxui::text("esc to go back home") | ftxui::color(phi::ui::colors::BLUE_BABY) |
              ftxui::center,
            ftxui::separatorEmpty(), ftxui::separatorEmpty(),
            ftxui::hbox(
              {ftxui::filler(),
               this->components.contacts_menu->Render() | ftxui::color(phi::ui::colors::BLUE_BABY) |
                 ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, phi::ui::COLS / 3) |
                 ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, phi::ui::ROWS / 2) |
                 ftxui::borderRounded | ftxui::color(phi::ui::colors::GOLD) | ftxui::center,
               ftxui::filler()})}) |
         homebox;
}

ftxui::Element phi::ui::Manager::renderContactPageUI(int contact_id) const {
  auto id_head = ftxui::text("ID: " + std::to_string(contact_id)) | ftxui::bold |
                 ftxui::color(phi::ui::colors::GOLD);

  return ftxui::vbox({ftxui::text("esc to go back home") |
                        ftxui::color(phi::ui::colors::BLUE_BABY) | ftxui::center,
                      ftxui::separatorEmpty(), ftxui::separatorEmpty(), id_head | ftxui::center,

                      ftxui::hbox({ftxui::vbox({
                                     ftxui::filler(),
                                     ftxui::align_right(ftxui::text("EMOJI")),
                                     ftxui::filler(),
                                     ftxui::align_right(ftxui::text("NAME")),
                                     ftxui::filler(),
                                     ftxui::align_right(ftxui::text("RSA KEY (B64)")),
                                     ftxui::filler(),
                                     ftxui::align_right(ftxui::text("ADDRESS")),
                                     ftxui::filler(),
                                     ftxui::filler(),
                                   }) |
                                     ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 13) |
                                     ftxui::color(phi::ui::colors::GOLD),
                                   ftxui::separatorEmpty(),
                                   this->components.contact_page->Render() |
                                     ftxui::bgcolor(phi::ui::colors::PURPLE_HAZE) |
                                     ftxui::color(phi::ui::colors::GOLD) |
                                     ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 42) | ftxui::center}) |
                        ftxui::bgcolor(phi::ui::colors::PURPLE_HAZE) | ftxui::borderRounded |
                        ftxui::color(phi::ui::colors::GOLD) | ftxui::center}) |
         homebox;
}

ftxui::Element phi::ui::Manager::contactDoesNotExist() const {
  auto content = ftxui::paragraphAlignCenter(phi::ui::constants::contact_does_not_exist);
  content |= ftxui::bold;

  auto content_box = ftxui::vbox(ftxui::filler(), content, ftxui::filler());
  content_box = ftxui::hbox(ftxui::filler(), content_box, ftxui::filler()) |
                ftxui::bgcolor(phi::ui::colors::PURPLE_HAZE) | ftxui::borderRounded |
                ftxui::color(phi::ui::colors::RED) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 131) |
                ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 19);

  return ftxui::vbox({ftxui::text("esc to go back home") |
                        ftxui::color(phi::ui::colors::BLUE_BABY) | ftxui::center,
                      ftxui::separatorEmpty(), ftxui::separatorEmpty(),
                      content_box | ftxui::center}) |
         homebox;
}

ftxui::Element phi::ui::Manager::renderNotification() const {
  return ftxui::vbox({ftxui::text(this->state.noti.title) | ftxui::bold, ftxui::separator(),
                      ftxui::paragraph(this->state.noti.description), ftxui::filler()}) |
         ftxui::borderRounded |  // ftxui::bgcolor(phi::ui::colors::DEEP) |
         ftxui::color(this->state.noti.color) |
         ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 40) | ftxui::align_right;
}