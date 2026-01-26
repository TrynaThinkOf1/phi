/*

 PHI
 2026/01/01

 Phi C++ Project
 src/phi/Manager.cpp

 Zevi Berlin

*/

#include "phi/ui/Manager.hpp"

//---------> [ Config. Separator ] <---------\\ 

void phi::ui::Manager::getContacts() {
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
  }

  this->contacts = contacts;
  this->contact_ids = contact_ids;
}

//------------[ Func. Implementation Separator ]------------\\ 

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

//------------[ Func. Implementation Separator ]------------\\ 

void phi::ui::Manager::loadComponents() {
}

//------------[ Func. Implementation Separator ]------------\\ 

void phi::ui::Manager::addNoti(const std::string& title, const std::string& description,
                               const ftxui::Color& color, double lifespan) {
  this->state.noti.show = true;
  this->state.noti.title = title;
  this->state.noti.description = description;
  this->state.noti.color = color;
  auto now = std::chrono::steady_clock::now();
  this->state.noti.expires = now + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                                     std::chrono::duration<double>(lifespan));
  std::thread([this] {
    std::this_thread::sleep_until(this->state.noti.expires);
    this->state.noti.show = false;
    this->screen.PostEvent(ftxui::Event::Custom);
  }).detach();
}

/*::::::::::::::::::::::::::::::::::::::*\
|*:::::::::[ Access Separator ]:::::::::*|
\*::::::::::::::::::::::::::::::::::::::*/

phi::ui::Manager::Manager(std::shared_ptr<phi::database::Database> database,
                          std::shared_ptr<phi::encryption::Encryptor> encryptor,
                          std::shared_ptr<phi::tasks::TaskMaster> taskmaster)
    : DATABASE(std::move(database)),
      ENCRYPTOR(std::move(encryptor)),
      TASKMASTER(std::move(taskmaster)) {
  this->getContacts();
  this->loadComponents();
}

//------------[ Func. Implementation Separator ]------------\\ 

void phi::ui::Manager::eventLoop() {
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


  // Password input box
  this->createLoginInput();
  //

  // Home-Page buttons
  this->createHomePageButtons(bopt);
  //

  // Contacts menu
  this->createContactsMenu();
  //

  // Contact edit page
  this->createContactEditPage(bopt);
  //

  this->rebuildRoot(root);

  root |= ftxui::CatchEvent([&](ftxui::Event e) {
    // no skipping login
    if (e == ftxui::Event::Escape && this->state.page != phi::ui::Page::Login) {
      this->state.page = phi::ui::Page::Home;
      this->rebuildRoot(this->root);
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
        this->displayable_rsa_key = toB64(selected_contact_t.rsa_key.substr(0, 26)) + "...";
        base = this->renderContactPageUI(this->contact_ids.at(this->selected_contact_id));
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

  this->screen.Loop(ftxui::Renderer(this->root, render_fn));
}