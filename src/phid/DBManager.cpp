/*

 PHI
 2025/09/19

 Phi C++ Project
 src/phid/DBManager.cpp

 Zevi Berlin

*/

#include "phid/database/DBManager.hpp"

#include <iostream>
#include <mutex>
#include <string>

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>

#include "utils.hpp"

/** CONSTRUCTOR & DESTRUCTOR **/

phid::DBManager::DBManager(bool& scs) {
  /*
  Create/open the SQLite3 database file at
   /var/phi/main.db which will store all
   self, contact, message and error information.
   Also create a Self object to store the user's
   information w/o having to repeatedly query
   the database.

  Args:
   op - output, 0 if success, -1 if self isn't defined
  */

  this->db = new SQLite::Database(expand("~/.phi/main.db"), SQLite::OPEN_READWRITE);  // NOLINT

  db->exec(
    R"sql(
      CREATE TABLE IF NOT EXISTS self (
        id INTEGER PRIMARY KEY CHECK (id = 0),
        name TEXT DEFAULT "Me",
        emoji TEXT DEFAULT "🪞",
        rsa_pub_key TEXT NOT NULL,
        rsa_priv_key TEXT NOT NULL,
        last_known_ip TEXT NOT NULL,
        hardware_profile TEXT DEFAULT "Unknown / Unknown"
      );

      CREATE TABLE IF NOT EXISTS contacts (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT DEFAULT "Contact",
        emoji TEXT DEFAULT "👤",
        rsa_pub_key TEXT NOT NULL,
        shared_secret TEXT NOT NULL,
        ipv6_addr TEXT NOT NULL,
        hardware_profile TEXT NOT NULL,
        UNIQUE(ipv6_addr)
      );

      CREATE TABLE IF NOT EXISTS messages (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        contact_id INTEGER NOT NULL,
        content TEXT NOT NULL,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
        is_read BOOLEAN DEFAULT FALSE,
        is_delivered BOOLEAN DEFAULT FALSE,
        FOREIGN KEY(contact_id) REFERENCES contacts(id)
      );

      CREATE TABLE IF NOT EXISTS derrors (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title TEXT NOT NULL,
        description TEXT,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
        is_reported BOOLEAN DEFAULT FALSE
      );
    )sql");

  /**/

  SQLite::Statement query(*(this->db), "SELECT * FROM self WHERE id = 0");
  if (!query.executeStep()) {
    scs = false;
    return;
  }

  this->self.name = query.getColumn("name").getString();
  this->self.emoji = query.getColumn("emoji").getString();
  this->self.rsa_pub_key = query.getColumn("rsa_pub_key").getString();
  this->self.rsa_priv_key = query.getColumn("rsa_priv_key").getString();
  this->self.last_known_ip = query.getColumn("last_known_ip").getString();
  this->self.hardware_profile = query.getColumn("hardware_profile").getString();

  scs = true;
}

phid::DBManager::~DBManager() {
  /*
  Delete the database object.
  */

  delete db;  // auto closes the database
}

/** **/

//====================\\

/*** public methods ***/

/** SELF **/

void phid::DBManager::createSelf(std::string name, std::string emoji,
                                 const std::string& rsa_pub_key, const std::string& rsa_priv_key,
                                 const std::string& ipv6_addr,
                                 const std::string& hardware_profile) {
  /*
  In the beginning, god said "let there be self"

  Args:
   highly self-explanatory
  */

  if (name.empty()) {
    name = "Me";
  }
  if (emoji.empty()) {
    emoji = "🪞";
  }

  this->db->exec("DELETE FROM self");

  SQLite::Statement create(
    *(this->db),
    R"sql(INSERT INTO self (id, name, emoji, rsa_pub_key, rsa_priv_key, last_known_ip, hardware_profile)
            VALUES (0, :name, :emoji, :pub, :priv, :ip, :prof))sql");
  create.bind(":name", name);
  create.bind(":emoji", emoji);
  create.bind(":pub", rsa_pub_key);
  create.bind(":priv", rsa_priv_key);
  create.bind(":ip", ipv6_addr);
  create.bind(":prof", hardware_profile);
  create.executeStep();

  this->updateSelf();
}

bool phid::DBManager::updateSelf() {
  /*
  Sometimes it is necessary to update
   the public phid::self_t struct which
   holds readily available information
   about the user because a field has
   changed

  Returns:
   self exists ? true : false
  */

  SQLite::Statement check(*(this->db), "SELECT * FROM self WHERE id = 0");
  if (!check.executeStep()) {
    return false;
  }

  this->self.name = check.getColumn("name").getString();
  this->self.emoji = check.getColumn("emoji").getString();
  this->self.rsa_pub_key = check.getColumn("rsa_pub_key").getString();
  this->self.rsa_priv_key = check.getColumn("rsa_priv_key").getString();
  this->self.last_known_ip = check.getColumn("last_known_ip").getString();
  this->self.hardware_profile = check.getColumn("hardware_profile").getString();

  return true;
}

void phid::DBManager::changeSelfAttribute(const std::string& field, const std::string& value) {
  /*
  Rather than define individual functions
   or a template function for this, I simply
   trust that anybody writing code is not
   stupid enough to allow an update of a field
   which does not exist (probably stupud of me)

  Args:
   field - string for the field (e.g. "name")
   valie - string for the new value (e.g. "Zevi")
  */

  SQLite::Statement exists(*(this->db), "SELECT 1 FROM self");

  if (!exists.executeStep()) {
    return;
  }

  SQLite::Statement update(*(this->db), "UPDATE self SET field = :value WHERE id = 0");
  update.bind(":field", field);
  update.bind(":value", value);
  update.exec();

  this->updateSelf();
}

/** **/

/** CONTACTS **/

int phid::DBManager::createContact(std::string name, std::string emoji,
                                   const std::string& rsa_pub_key, const std::string& shared_secret,
                                   const std::string& ipv6_addr,
                                   const std::string& hardware_profile) {
  /*
  Just create a new contact

  Args:
   highly self-explanatory

  Returns:
   ID of the newly created contact
  */
  if (name.empty()) {
    name = "Contact";
  }
  if (emoji.empty()) {
    emoji = "👤";
  }

  SQLite::Statement create(
    *(this->db),
    R"sql(INSERT INTO contacts (name, emoji, rsa_pub_key, shared_secret, ipv6_addr, hardware_profile)
            VALUES (:name, :emoji, :pub, :ss, :ip, :prof)
            RETURNING id)sql");
  create.bind(":name", name);
  create.bind(":emoji", emoji);
  create.bind(":pub", rsa_pub_key);
  create.bind(":ss", shared_secret);
  create.bind(":ip", ipv6_addr);
  create.bind(":prof", hardware_profile);

  if (!create.executeStep()) {
    return -1;
  }
  return create.getColumn("id").getInt();
}

bool phid::DBManager::getContact(int contact_id, phid::contact_t& op) {
  /*
  Retrieve contact information from
   the database and use it to fill a
   contact_t struct

  Args:
   contact_id - id (primary key) of contact
   op - output contact_t struct

  Returns:
   contact existed ? true : false
  */

  SQLite::Statement query(*(this->db), "SELECT * FROM contacts WHERE id = :id");
  query.bind(":id", contact_id);

  if (!query.executeStep()) {
    return false;
  }

  op.name = query.getColumn("name").getString();
  op.emoji = query.getColumn("emoji").getString();
  op.rsa_pub_key = query.getColumn("rsa_pub_key").getString();
  op.shared_secret = query.getColumn("shared_secret").getString();
  op.ipv6_addr = query.getColumn("ipv6_addr").getString();
  op.hardware_profile = query.getColumn("hardware_profile").getString();

  return true;
}

int phid::DBManager::changeContactAttribute(int contact_id, const std::string& field,
                                            const std::string& value) {
  /*
  Rather than define individual functions
   or a template function for this, I simply
   trust that anybody writing code is not
   stupid enough to allow an update of a field
   which does not exist (probably stupud of me)

  Args:
   contact_id - id (primary key) of contact
   field - string for the field (e.g. "name")
   valie - string for the new value (e.g. "Bartholomew")

   Returns:
    0 if successful, -1 if contact doesn't exist, -2 if ipv6_addr isn't unique
  */

  SQLite::Statement exists(*(this->db), "SELECT 1 FROM contacts");

  if (!exists.executeStep()) {
    return -1;
  }

  SQLite::Statement update(*(this->db), "UPDATE contacts SET field = :value WHERE id = :id");
  update.bind(":field", field);
  update.bind(":value", value);
  update.bind(":id", contact_id);

  if (!update.executeStep()) {
    return -2;
  }

  return 0;
}

void phid::DBManager::deleteContact(int contact_id) {
  /*
  Delete a contact

  Args:
   contact_id - id (primary key) of contact to delete
  */

  SQLite::Statement del(*(this->db), "DELETE FROM contacts WHERE ID = :id");
  del.bind(":id", contact_id);
  del.exec();
}

/** **/

/*** ***/