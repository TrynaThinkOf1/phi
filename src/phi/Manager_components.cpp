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

//------------[ Func. Implementation Separator ]------------\\ 

void phi::ui::Manager::createHomePageButtons(const ftxui::ButtonOption& bopt) {
  auto conversation_menu_button = ftxui::Button(
    "Conversations", [&] { this->state.page = phi::ui::Page::ConversationsMenu; }, bopt);

  auto contact_menu_button = ftxui::Button(
    "Contacts",
    [&] {
      this->state.page = phi::ui::Page::ContactsMenu;
      this->getContacts();
      this->rebuildRoot(root);
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
}

//------------[ Func. Implementation Separator ]------------\\ 

void phi::ui::Manager::createContactsMenu() {
  this->components.contacts_menu =
    ftxui::Menu(&contacts, &this->selected_contact_id) | ftxui::CatchEvent([&](ftxui::Event e) {
      if (e == ftxui::Event::Return) {
        if (!this->DATABASE->getContact(contact_ids.at(this->selected_contact_id),
                                        this->selected_contact_t)) {
          this->state.page = phi::ui::Page::ContactDoesNotExist;
        } else {
          this->state.page = phi::ui::Page::EditContact;
        }
        this->rebuildRoot(root);
        return true;
      }
      return false;
    });
}

//------------[ Func. Implementation Separator ]------------\\ 

void phi::ui::Manager::createContactEditPage(const ftxui::ButtonOption& bopt) {
#define ENTER_CATCHER \
  ftxui::CatchEvent([&](const ftxui::Event& e) { return e == ftxui::Event::Return; })

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

  
  auto emoji_input = ftxui::Input(&this->selected_contact_t.emoji, "...", ropt) | ENTER_CATCHER;
  auto name_input = ftxui::Input(&this->selected_contact_t.name, "...", ropt) | ENTER_CATCHER;
  auto rsa_input = ftxui::Input(&this->displayable_rsa_key, "...", dopt) |
                   ftxui::CatchEvent([](const ftxui::Event& e) {
                     if (e.is_character()) return true;              // block typing / paste
                     if (e == ftxui::Event::Backspace) return true;  // block backspace
                     if (e == ftxui::Event::Delete) return true;     // block delete
                     if (e == ftxui::Event::CtrlV) return true;      // block paste (ctrl-v)
                     if (e == ftxui::Event::CtrlU) return true;      // other edit shortcuts
                     return false;  // allow navigation, Return, Tab, etc.
                   });
  auto addr_input = ftxui::Input(&this->selected_contact_t.addr, "...", ropt) | ENTER_CATCHER;

  auto save_changes = ftxui::Button(
    "Save Changes",
    [&] {
      phi::database::contact_t current;
      if (!this->DATABASE->getContact(this->contact_ids.at(this->selected_contact_id), current) ||
          !this->DATABASE->updateContact(current, this->selected_contact_t)) {
        this->state.page = phi::ui::Page::ContactDoesNotExist;
        this->rebuildRoot(this->root);
      }

      this->addNoti("Changes Saved",
                    "The profile changes made to the contact with ID " +
                      std::to_string(current.id) + " were saved to the database successfully.",
                    3.0);
    },
    bopt);


  this->components.contact_page =
    ftxui::Container::Vertical({emoji_input, name_input, rsa_input, addr_input, save_changes});

#undef ENTER_CATCHER
}