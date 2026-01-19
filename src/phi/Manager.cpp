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

std::tuple<std::vector<std::string>, std::vector<int>> phi::ui::Manager::getContacts() {
  std::unique_ptr<std::vector<std::tuple<int, std::string, std::string>>> actuals =
    DATABASE->getAllContacts();

  std::vector<std::string> contacts;
  std::vector<int> contact_ids;

  if (actuals != nullptr) {
    contacts.resize(actuals->size());
    contact_ids.resize(actuals->size());
    for (size_t i = 0; i < actuals->size(); i++) {
      auto tup = actuals->at(i);

      contacts[i] =
        std::get<2>(tup) + " " + std::get<1>(tup) + " (" + std::to_string(std::get<0>(tup)) + ")";
      contact_ids[i] = std::get<0>(tup);
    }

    return std::make_tuple(contacts, contact_ids);
  }

  return {};
}

/***/

void phi::ui::Manager::eventLoop() {
  ftxui::Component root = ftxui::Container::Vertical({});
  bool should_exit = false;


  phi::database::contact_t selected_contact_t{};
  int selected_contact_id = 0;

  auto actuals_tup = this->getContacts();
  std::vector<std::string> contacts = std::get<0>(actuals_tup);
  std::vector<int> contact_ids = std::get<1>(actuals_tup);


  // Password input box
  std::string password;

  ftxui::InputOption popt;
  popt.password = true;

  this->components.login_input = ftxui::Input(&password, "", popt);
  this->components.login_input |= ftxui::CatchEvent([&](const ftxui::Event& e) {
    if (e == ftxui::Event::Return) {
      if (this->DATABASE->login(password)) {
        this->DATABASE->createTables();
        this->state.page = phi::ui::Page::Home;
        rebuildRoot(root);
      } else {
        should_exit = true;
      }
      return true;
    }

    return false;
  });
  //

  // these will be set during home page rendering
  this->components.contact_request_menu = ftxui::Menu({});
  this->components.error_menu = ftxui::Menu({});
  //

  // Home-Page buttons
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

  auto conversation_menu_button = ftxui::Button(
    "Conversations", [&] { this->state.page = phi::ui::Page::ConversationsMenu; }, bopt);

  auto contact_menu_button = ftxui::Button(
    "Contacts",
    [&] {
      this->state.page = phi::ui::Page::ContactsMenu;
      actuals_tup = this->getContacts();
      contacts = std::get<0>(actuals_tup);
      contact_ids = std::get<1>(actuals_tup);
      rebuildRoot(root);
    },
    bopt);

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

  this->components.home_button_layout = ftxui::Container::Vertical(
    {conversation_menu_button, contact_menu_button, contact_request_menu_button, edit_self,
     change_db_password, settings, error_menu_button, screensaver_button, exit_button});
  //

  // Contacts menu
  this->components.contacts_menu =
    ftxui::Menu(&contacts, &selected_contact_id) | ftxui::CatchEvent([&](ftxui::Event e) {
      if (e == ftxui::Event::Return) {
        if (!this->DATABASE->getContact(contact_ids.at(selected_contact_id), selected_contact_t)) {
          this->state.page = phi::ui::Page::ContactDoesNotExist;
        } else {
          this->state.page = phi::ui::Page::EditContact;
        }
        rebuildRoot(root);
        return true;
      }
      return false;
    });
  //

  // Contact edit page
#define ENTER_CATCHER ftxui::CatchEvent([&](ftxui::Event e) { return e == ftxui::Event::Return; })

  ftxui::InputOption ropt;
  ropt.transform = [](ftxui::InputState s) {
    return s.element | ftxui::color(phi::ui::colors::BLUE_BABY) | ftxui::borderRounded |
           ftxui::color(phi::ui::colors::GOLD);
  };

  ftxui::InputOption dopt;
  dopt.transform = [](ftxui::InputState s) {
    return s.element | ftxui::color(phi::ui::colors::BLUE_BABY) | ftxui::borderRounded |
           ftxui::color(phi::ui::colors::GOLD) | ftxui::dim;
  };

  std::string displayable_rsa_key;

  auto emoji_input = ftxui::Input(&selected_contact_t.emoji, "...", ropt) | ENTER_CATCHER;
  auto name_input = ftxui::Input(&selected_contact_t.name, "...", ropt) | ENTER_CATCHER;
  auto rsa_input =
    ftxui::Input(&displayable_rsa_key, "...", dopt) | ftxui::CatchEvent([](ftxui::Event e) {
      if (e.is_character()) return true;              // block typing / paste
      if (e == ftxui::Event::Backspace) return true;  // block backspace
      if (e == ftxui::Event::Delete) return true;     // block delete
      if (e == ftxui::Event::CtrlV) return true;      // block paste (ctrl-v)
      if (e == ftxui::Event::CtrlU) return true;      // other edit shortcuts
      return false;                                   // allow navigation, Return, Tab, etc.
    });
  auto addr_input = ftxui::Input(&selected_contact_t.addr, "...", ropt) | ENTER_CATCHER;

  auto save_changes = ftxui::Button(
    "Save Changes",
    [&] {
      phi::database::contact_t current;
      if (!this->DATABASE->getContact(contact_ids.at(selected_contact_id), current)) {
        should_exit = true;
        return;
      }

      if (!this->DATABASE->updateContact(current, selected_contact_t)) {
        this->state.page = phi::ui::Page::ContactDoesNotExist;
        rebuildRoot(root);
      }

      this->state.noti.show = true;
      this->state.noti.title = "Changes Saved";
      this->state.noti.description = "The profile changes made to the contact with ID " +
                                     std::to_string(current.id) +
                                     " were saved to the database successfully.";
      auto now = std::chrono::steady_clock::now();
      this->state.noti.expires =
        now + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                std::chrono::duration<double>(3.0));
      std::thread([this] {
        std::this_thread::sleep_until(this->state.noti.expires);
        this->state.noti.reset();
        this->screen.PostEvent(ftxui::Event::Custom);
      }).detach();
    },
    bopt);  // bopt is above for homepage

  this->components.contact_page =
    ftxui::Container::Vertical({emoji_input, name_input, rsa_input, addr_input, save_changes});

#undef ENTER_CATCHER
  //

  rebuildRoot(root);

  root |= ftxui::CatchEvent([&](ftxui::Event e) {
    // no skipping login
    if (e == ftxui::Event::Escape && this->state.page != phi::ui::Page::Login) {
      this->state.page = phi::ui::Page::Home;
      rebuildRoot(root);
      return true;
    }
    return false;
  });


  auto render_fn = [&] {
    if (should_exit) this->screen.Exit();

    ftxui::Element base;

    switch (this->state.page) {
      case phi::ui::Page::Login:
        base = this->renderLoginUI();
        break;

      case phi::ui::Page::Home:
        base = this->renderHomeUI();
        break;

      case phi::ui::Page::ContactsMenu:
        base = this->renderContactsMenuUI();
        break;

      case phi::ui::Page::EditContact:
        displayable_rsa_key = toB64(selected_contact_t.rsa_key.substr(0, 26)) + "...";
        base = this->renderContactPageUI(contact_ids.at(selected_contact_id));
        break;

      case phi::ui::Page::ContactDoesNotExist:
        base = this->contactDoesNotExist();
        break;

      default:  // if I leave an unfinished/no default then it loops back around to login
        base = this->renderHomeUI();
        break;
    }

    //

    if (!this->state.noti.show) return base;

    ftxui::Element overlay = ftxui::vbox({
      ftxui::hbox({ftxui::filler(), renderNotification()}),
      ftxui::filler(),
    });

    return ftxui::dbox({base, overlay}) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, phi::ui::COLS) |
           ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, phi::ui::ROWS) | ftxui::center;
  };
  this->screen.Loop(ftxui::Renderer(root, render_fn));
}

void phi::ui::Manager::rebuildRoot(ftxui::Component& root) const {
  root->DetachAllChildren();

  switch (this->state.page) {
    case phi::ui::Page::Login:
      root->Add(this->components.login_input);
      this->components.login_input->TakeFocus();
      break;
    case phi::ui::Page::Home:
      root->Add(this->components.home_button_layout);
      this->components.home_button_layout->TakeFocus();
      break;
    case phi::ui::Page::ContactsMenu:
      root->Add(this->components.contacts_menu);
      this->components.contacts_menu->TakeFocus();
      break;
    case phi::ui::Page::EditContact:
      root->Add(this->components.contact_page);
      this->components.contact_page->TakeFocus();
      break;
  }
}