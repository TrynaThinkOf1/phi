/*

  PHI
  2025/09/20

  Phi C++ Project
  src/phi/main.cpp

  Zevi Berlin

*/

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <exception>
#include <thread>
#include <map>
#include <tuple>

#include "termcolor/termcolor.hpp"

#include "phi/database/Database.hpp"
#include "phi/encryption/MessageTypes.hpp"
#include "phi/encryption/Encryptor.hpp"
#include "phi/encryption/secrets.hpp"
#include "phi/do_setup.hpp"
#include "utils.hpp"

namespace tmc = termcolor;

int main() {
  int erc = 0;

  if (!createDataFiles(erc)) {
    std::cout << tmc::bright_red;
    switch (erc) {
      case 1:
        std::cout << "⛔️ Failed to create data file directory (~/.phi/) ⛔️\n";
        break;
      case 2:
        std::cout << "⛔️ Failed to create database file (~/.phi/main.db) ⛔️\n";
        break;
      case 3:
        std::cout << "⛔️ Failed to create profile holder (~/.phi/self.json) ⛔️\n";
        break;
      default:
        break;
    }
    std::cout << tmc::reset;
    return erc;
  }  // exit if not valid permissions

  /**/

  const std::shared_ptr<phi::encryption::Encryptor> ENCRYPTOR =
    std::make_shared<phi::encryption::Encryptor>();

  /**/

  const std::shared_ptr<phi::database::Database> DATABASE =
    std::make_shared<phi::database::Database>(erc);
  if (erc == 1) {
    std::cout << tmc::bright_red
              << "⛔️ Failed to create self file (~/.phi/self.json) due to invalid permissions ⛔️\n"
              << tmc::reset;
    return erc;
  } else if (erc == 2) {
    std::cout << tmc::yellow << "⚠️ Profile not found, needs setup ⚠️\n" << tmc::reset;

    std::cout << "Would you like to set up your profile right now (y/n)? ";
    char conf;
    std::cin >> conf;

    if (conf == 'y') {
      if (!do_setup(DATABASE, ENCRYPTOR)) {
        std::cout << tmc::bright_red << "Setup failed." << tmc::reset << "\n";
        return erc;
      }
    } else {
      std::cout << tmc::red << "\nExiting without creating profile.\n" << tmc::reset;
      return 0;
    }
  }

  ENCRYPTOR->changePrivKey(DATABASE->self.rsa_priv_key);

  /**/

  /* TESTING PORTION */

  /* Self -- working
  {
    std::string pub, priv;
    ENCRYPTOR->rsaGenPair(pub, priv);

    std::map<std::string, std::string> self_tests{
      {"name", "M. ZEVIUS SPARTANEOUS"}, {"emoji", "🥳"}, {"rsa_pub_key", pub}, {"rsa_priv_key",
  priv}};

    std::cout << "CURRENT SELF: " << DATABASE->self.toString() << "\n";

    for (const auto& [key, value] : self_tests) {
      if (!DATABASE->changeSelfAttribute(key, value, erc)) {
        if (erc == 1) {
          std::cout << tmc::bright_red << "Couldn't access self!\n" << tmc::reset;
          return erc;
        }
      }

      std::cout << "NEW SELF: " << DATABASE->self.toString() << "\n";
    }
  }
  */

  /* Contacts -- working
  {
    std::string pub, priv;
    ENCRYPTOR->rsaGenPair(pub, priv);  // does NOT change the private key it stores

    phi::encryption::KXP me{};
    phi::encryption::genKXP(me);

    phi::encryption::KXP contactius{};
    phi::encryption::genKXP(contactius);

    std::vector<unsigned char> me_ss, contactius_ss;
    phi::encryption::derive_shared_secret(true, me, contactius.pk, me_ss);
    phi::encryption::derive_shared_secret(false, contactius, me.pk, contactius_ss);

    if (me_ss != contactius_ss) {
      std::cout << tmc::bright_red << "Shared secrets did not derive correctly!\n" << tmc::reset;
      return 1;
    }

    std::string shared_secret = phi::encryption::secret_to_str(me_ss);
    std::cout << "Shared secret: " << toHex(shared_secret) << "\n";

    int contactius_id;
    if (!DATABASE->createContact("L. CONTACTIUS AGNACIOUS", "💕", "AAAA::BBBB::CCCC::DDDD",
                                 shared_secret, pub, erc, contactius_id)) {
      std::cout << tmc::bright_red;
      switch (erc) {
        case 1:
          std::cout << "The name `L. CONTACTIUS AGNACIOUS` already exists in the database!\n";
          break;
        case 2:
          std::cout
            << "The IPv6 address `AAAA::BBBB::CCCC::DDDD` already exists in the database!\n";
          break;
        case 3:
          std::cout << "Unknown error occurred while adding contact!\n";
        default:
          break;
      }
      std::cout << tmc::reset;
      return erc;
    }

    std::cout << "GET ALL CONTACTS {\n";
    std::unique_ptr<std::vector<std::tuple<int, std::string, std::string>>> contacts =
      DATABASE->getAllContacts();
    for (const auto& info : *contacts) {
      std::cout << "    " << std::get<0>(info) << " -> (" << std::get<1>(info) << ", "
                << std::get<2>(info) << ")\n";
    }

    std::cout << "}\nGET CONTACT (" << std::to_string(contactius_id) << "): ";
    phi::database::contact_t con;
    if (!DATABASE->getContact(contactius_id, con, erc)) {
      if (erc == 1) {
        std::cout << tmc::bright_red
                  << "FAILED\nContact with that id does not exist in the database!\n"
                  << tmc::reset;
        return erc;
      }
    }
    std::cout << con.toString() << "\n";

    std::cout << "CHANGING CONTACT NAME & IP\n";
    if (!(DATABASE->changeContactAttribute(contactius_id, "name", "Ts Guy", erc) &&
          DATABASE->changeContactAttribute(contactius_id, "addr", "1111::2222::3333::4444", erc) &&
          DATABASE->getContact(contactius_id, con, erc))) {
      if (erc == 1) {
        std::cout << tmc::bright_red
                  << "FAILED\nContact with that id does not exist in the database!\n"
                  << tmc::reset;
        return erc;
      }
    }
    std::cout << "NEW CONTACT:\n" << con.toString() << "\n";

    DATABASE->deleteContact(contactius_id);

    if (!DATABASE->getContact(contactius_id, con, erc)) {
      if (erc == 1) {
        std::cout << tmc::green << "Contact deleted successfully!\n" << tmc::reset;
      }
    }
  }
  */

  ///* Messages -- testing
  //*/

  ///* Errors -- testing
  //*/

  /* */

  return 0;
}