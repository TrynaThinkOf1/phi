/*

 PHI
 2026/01/01

 Phi C++ Project
 src/phi/Manager.cpp

 Zevi Berlin

*/

#include "phi/ui/Manager.hpp"

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

  // Password input box
  ftxui::InputOption popt;
  popt.password = true;

  std::string password;
  this->components.login_input = ftxui::Input(&password, "", popt);
  this->components.login_input |= ftxui::CatchEvent([&](const ftxui::Event& e) {
    if (e == ftxui::Event::Return) {
      if (this->DATABASE->login(password)) {
        this->DATABASE->createTables();
        this->state.page = phi::ui::Page::Home;
      } else {
        should_exit = true;
      }
      return true;
    }

    return false;
  });
  //

  // these will be set during home page rendering
  this->components.contact_menu = ftxui::Menu({});
  this->components.contact_request_menu = ftxui::Menu({});
  this->components.error_menu = ftxui::Menu({});
  //

  ftxui::ButtonOption bopt;
  bopt.transform = [](const ftxui::EntryState& s) {
    ftxui::Element e = ftxui::text(s.label) | ftxui::hcenter | ftxui::vcenter;

    if (s.focused) {
      e = e | ftxui::bold | ftxui::borderHeavy | ftxui::color(phi::ui::colors::PURPLE_HAZE) |
          ftxui::bgcolor(phi::ui::colors::GOLD);
    } else {
      e = e | ftxui::borderRounded | ftxui::color(phi::ui::colors::GOLD) |
          ftxui::bgcolor(phi::ui::colors::PURPLE_HAZE);
    }

    return e | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 32) |
           ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3);
  };

  // Home-Page buttons
  auto conversation_menu_button = ftxui::Button(
    "Conversations", [&] { this->state.page = phi::ui::Page::ConversationsMenu; }, bopt);

  auto contact_menu_button =
    ftxui::Button("Contacts", [&] { this->state.page = phi::ui::Page::ContactsMenu; }, bopt);

  auto contact_request_menu_button = ftxui::Button(
    "Add/Accept New Contact", [&] { this->state.page = phi::ui::Page::ContactRequestsMenu; }, bopt);

  auto edit_self =
    ftxui::Button("Edit Self Profile", [&] { this->state.page = phi::ui::Page::EditSelf; }, bopt);

  auto change_db_password = ftxui::Button(
    "Change Database Password", [&] { this->state.page = phi::ui::Page::ChangeDatabasePassword; },
    bopt);

  auto settings =
    ftxui::Button("Settings", [&] { this->state.page = phi::ui::Page::Settings; }, bopt);

  auto error_menu_button =
    ftxui::Button("View Errors", [&] { this->state.page = phi::ui::Page::ViewErrorsMenu; }, bopt);

  auto screensaver_button =
    ftxui::Button("Screensaver", [&] { this->state.page = phi::ui::Page::Screensaver; }, bopt);

  auto exit_button = ftxui::Button("Exit", [&] { should_exit = true; }, bopt);

  this->components.home_button_layout_left = ftxui::Container::Vertical(
    {conversation_menu_button, contact_menu_button, contact_request_menu_button, edit_self,
     change_db_password, settings, error_menu_button, screensaver_button, exit_button});
  //


  auto total_layout = ftxui::Container::Vertical({this->components.toVec()});
  auto renderer = ftxui::Renderer(total_layout, [&] {
    if (should_exit) this->screen.Exit();

    switch (this->state.page) {
      case Page::Login:
        return this->renderLoginUI();
      case Page::Home:
        return this->renderHomeUI();
      case Page::Screensaver:
        return this->renderScreensaver();
      default:  // if I leave an unfinished/no default then it loops back around to login
        return this->renderHomeUI();
    }
  });
  this->screen.Loop(renderer);
}