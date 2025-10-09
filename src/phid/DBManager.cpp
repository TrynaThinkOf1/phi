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

  SQLite::Statement query(
    *db,
    "SELECT name, emoji, rsa_priv_key, last_known_ip, hardware_profile FROM self WHERE id = 0;");
  if (!query.executeStep()) {
    std::cout << "Selecting from self didnt work because of executing it\n";
    op_code = -1;
    return;
  }
  if (query.isColumnNull(0)) {
    std::cout << "Self column 0 is null\n";
    op_code = -1;
    return;
  }

  this->self.name = query.getColumn(0).getString();
  this->self.emoji = query.getColumn(1).getString();
  this->self.rsa_priv_key = query.getColumn(2).getString();
  this->self.last_known_ip = query.getColumn(3).getString();
  this->self.hardware_profile = query.getColumn(4).getString();

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

void phid::DBManager::initialize_self() {
}

void phid::DBManager::change_self_name(const std::string& new_name) {
  SQLite::Statement check(*db, "SELECT COUNT(*) FROM self;");

  if (!check.executeStep()) {
    std::cout << "Self did not exist\n";
    SQLite::Statement create(*db, "INSERT INTO self (name, emoji, rsa_priv_key, hardware_profile) "
                                  "VALUES (?, NULL, NULL, NULL);");
    create.bind(1, new_name);
    create.exec();

    std::cout << "Created self with name " << new_name << '\n';
    this->self.name = new_name;

    return;
  }

  std::cout << "Self existed\n";

  // no WHERE clause because there will only ever be one row
  SQLite::Statement update(*db, "UPDATE self SET name = ?;");
  update.bind(1, new_name);
  update.exec();

  self.name = new_name;
}

void phid::DBManager::change_self_emoji(const std::string& new_emoji) {
}

void phid::DBManager::change_self_rsa_priv_key(const std::string& new_rsa_priv_key) {
}

void phid::DBManager::change_self_hardware_profile(const std::string& new_hardware_profile) {
}

/** **/