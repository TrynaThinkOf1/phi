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

phid::DBManager::DBManager(int& op_code) {
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

  this->db = new SQLite::Database(expand("~/.phi/main.db"), SQLite::OPEN_READWRITE);

  db->exec(
    R"sql(
      CREATE TABLE IF NOT EXISTS self (
        id INTEGER PRIMARY KEY CHECK (id = 0),
        name TEXT,
        emoji TEXT,
        rsa_priv_key TEXT,
        last_known_ip TEXT,
        hardware_profile TEXT
      );

      CREATE TABLE IF NOT EXISTS contacts (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        emoji TEXT,
        rsa_pub_key TEXT NOT NULL,
        shared_secret TEXT,
        ipv6_addr TEXT,
        hardware_profile TEXT,
        UNIQUE(name), UNIQUE(ipv6_addr)
      );

      CREATE TABLE IF NOT EXISTS messages (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        sender_id INTEGER NOT NULL,
        content TEXT NOT NULL,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
        is_read INTEGER DEFAULT 0,
        FOREIGN KEY(sender_id) REFERENCES contacts(id)
      );
    )sql");

  /**/

  SQLite::Statement query(*db, "SELECT * FROM self WHERE id = 0");
  if (!query.executeStep()) {
    op_code = -1;
    return;
  }

  this->self.name = query.getColumn("name").getString();
  this->self.emoji = query.getColumn("emoji").getString();
  this->self.rsa_priv_key = query.getColumn("rsa_priv_key").getString();
  this->self.last_known_ip = query.getColumn("last_known_ip").getString();
  this->self.hardware_profile = query.getColumn("hardware_profile").getString();

  op_code = 0;
}

phid::DBManager::~DBManager() {
  /*
  Delete the database object.
  */

  delete db;  // auto closes the database
}

/** **/

//====================\\

/** public methods **/

bool phid::DBManager::update_self() {
  SQLite::Statement query(*db, "SELECT * FROM self WHERE id = 0");
  if (!query.executeStep()) {
    return false;
  }

  this->self.name = query.getColumn("name").getString();
  this->self.emoji = query.getColumn("emoji").getString();
  this->self.rsa_priv_key = query.getColumn("rsa_priv_key").getString();
  this->self.last_known_ip = query.getColumn("last_known_ip").getString();
  this->self.hardware_profile = query.getColumn("hardware_profile").getString();

  return true;
}

/***/

void phid::DBManager::change_self_attribute(const std::string& field, const std::string& value) {
  SQLite::Statement exists(*db, "SELECT 1 FROM self");

  if (!exists.executeStep()) {
    SQLite::Statement create(*db, "INSERT INTO self (id, :field) VALUES (0, :value)");
    create.bind(":field", field);
    create.bind(":value", value);
    create.exec();

    this->update_self();

    return;
  }

  SQLite::Statement update(*db, "UPDATE self SET field = :value WHERE id = 0");
  update.bind(":field", field);
  update.bind(":value", value);
  update.exec();

  this->update_self();
}

/** **/