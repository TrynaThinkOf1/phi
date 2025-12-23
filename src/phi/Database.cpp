/*

 PHI
 2025/09/19

 Phi C++ Project
 src/phi/Database.cpp

 Zevi Berlin

*/

#include "phi/database/Database.hpp"

#include <iostream>
#include <mutex>
#include <string>
#include <sstream>
#include <memory>
#include <fstream>
#include <vector>
#include <tuple>
#include <ctime>

#include <SQLiteCpp/SQLiteCpp.h>
#include <sqlite3.h>  // for error codes
#include "nlohmann/json.hpp"

#include "phi/database/structs.hpp"
#include "datetime.hpp"
#include "utils.hpp"

using json = nlohmann::json;

/** **/

phi::database::Database::Database(int& erc) {
  /*
    erc: 0 if no error, 1 if self can't be created, 2 if self is invalid
  */

  this->db =
    std::make_unique<SQLite::Database>(expand("~/.phi/main.db"), SQLite::OPEN_READWRITE);  // NOLINT

  db->exec(
    R"sql(
      CREATE TABLE IF NOT EXISTS contacts (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT DEFAULT "Contact",
        emoji TEXT DEFAULT "👤",
        addr TEXT NOT NULL,
        shared_secret TEXT NOT NULL,
        rsa_key TEXT NOT NULL,
        UNIQUE(addr)
      );

      CREATE TABLE IF NOT EXISTS messages (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        contact_id INTEGER NOT NULL,
        sender BOOLEAN NOT NULL,
        content TEXT NOT NULL,
        timestamp UNSIGNED BIG INT NOT NULL,
        delivered BOOLEAN DEFAULT FALSE,
        FOREIGN KEY (contact_id) REFERENCES contacts(id)
      );

      CREATE TABLE IF NOT EXISTS errors (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title TEXT NOT NULL,
        description TEXT,
        timestamp UNSIGNED BIG INT NOT NULL
      );
    )sql");

  /**/

  std::fstream file(this->self_path, std::fstream::in);
  if (!file.is_open()) {
    erc = 1;
    return;
  }

  std::stringstream buf;
  buf << file.rdbuf();
  file.close();

  // checks whether the file is empty or an invalid JSON
  if (buf.tellp() == std::streampos(0) || !json::accept(buf)) {
    erc = 2;
    return;
  }

  // no need to store value (a boolean about validity) because validity is checked above
  this->self.from_json_str(buf.str());

  erc = 0;
}

/** **/

bool phi::database::Database::createSelf(const std::string& name, const std::string& emoji,
                                         const std::string& rsa_pub_key,
                                         const std::string& rsa_priv_key,
                                         const std::string& ipv6_addr,
                                         const std::string& hardware_profile, int& erc) {
  /*
    erc: 0 if none, 1 if self can't be created
  */

  this->self.name = name;
  this->self.emoji = emoji;
  this->self.rsa_pub_key = rsa_pub_key;
  this->self.rsa_priv_key = rsa_priv_key;
  this->self.last_known_ip = ipv6_addr;
  this->self.hardware_profile = hardware_profile;

  std::fstream file(this->self_path, std::fstream::out);
  if (!file.is_open()) {
    erc = 1;
    return false;
  }

  file << this->self.to_json_str();
  file.close();

  erc = 0;
  return true;
}

bool phi::database::Database::changeSelfAttribute(const std::string& field,
                                                  const std::string& value, int& erc) {
  /*
    erc: 0 if none, 1 if self can't be accessed
  */

  // dereference the self structure's map entry of the
  // field, which holds a pointer to the string. Set that
  // string to the value
  *this->self.MAP[field] = value;

  std::fstream file(this->self_path, std::fstream::out);
  if (!file.is_open()) {
    erc = 1;
    return false;
  }

  file << this->self.to_json_str();
  file.close();

  erc = 0;
  return true;
}

/** **/

bool phi::database::Database::createContact(const std::string& name, const std::string& emoji,
                                            const std::string& addr,
                                            const std::string& shared_Secret,
                                            const std::string& rsa_key, int& erc, int& op_id) {
  /*
    erc: 0 if none, 1 if name exists, 2 if IPv6 addr already exists, 3 if unknown error
  */

  op_id = -1;

  SQLite::Statement check(*(this->db), "SELECT 1 FROM contacts WHERE :field = :value");
  check.bind(":field", "name");
  check.bind(":value", name);

  int result = check.tryExecuteStep();
  if (result == SQLITE_OK) {  // returns true if there is a row of data to select from
    erc = 1;
    return false;
  }

  check.reset();
  check.bind(":field", "addr");
  check.bind(":value", addr);

  result = check.tryExecuteStep();
  if (result == SQLITE_OK) {  // returns true if there is a row of data to select from
    erc = 2;
    return false;
  }

  SQLite::Statement create(*(this->db),
                           R"sql(INSERT INTO contacts (name, emoji, addr, shared_secret, rsa_key)
            VALUES (:name, :emoji, :addr, :ss, :key)
            RETURNING id)sql");
  create.bind(":name", name);
  create.bind(":emoji", emoji);
  create.bind(":addr", addr);
  create.bind(":ss", shared_Secret);
  create.bind(":key", rsa_key);

  result = create.tryExecuteStep();
  if (result != SQLITE_OK) {
    erc = 3;
    return false;
  }

  op_id = create.getColumn("id").getInt();
  return true;
}

std::unique_ptr<std::vector<std::tuple<int, std::string, std::string>>>
phi::database::Database::getAllContacts() {
  /*
    returns a vector of tuples, the tuples being:
    {id, name, emoji}
  */

  int rows = this->db->execAndGet("SELECT COUNT(id) FROM contacts").getInt();
  if (rows == 0) return nullptr;

  auto contacts = std::make_unique<std::vector<std::tuple<int, std::string, std::string>>>(rows);

  SQLite::Statement get_contacts(*(this->db), "SELECT id, name, emoji FROM contacts");
  int idx = 0;
  while (get_contacts.tryExecuteStep() == SQLITE_OK) {
    (*contacts)[idx] = std::make_tuple<int, std::string, std::string>(
      get_contacts.getColumn("id").getInt(), get_contacts.getColumn("name").getString(),
      get_contacts.getColumn("emoji").getString());
  }

  return contacts;
}

bool phi::database::Database::getContact(int contact_id, phi::database::contact_t& op, int& erc) {
  /*
    erc: 0 if none, 1 if contact doesn't exist
  */

  SQLite::Statement query(*(this->db), "SELECT * FROM contacts WHERE id = :id");
  query.bind(":id", contact_id);

  if (query.tryExecuteStep() != SQLITE_OK) {
    erc = 1;
    return false;
  }

  op.id = contact_id;
  op.name = query.getColumn("name").getString();
  op.emoji = query.getColumn("emoji").getString();
  op.addr = query.getColumn("addr").getString();
  op.shared_secret = query.getColumn("shared_secret").getString();
  op.rsa_key = query.getColumn("rsa_key").getString();

  erc = 0;
  return true;
}

bool phi::database::Database::changeContactAttribute(int contact_id, const std::string& field,
                                                     const std::string& value, int& erc) {
  /*
    erc: 0 if none, 1 if contact doesn't exist, 2 if field doesn't exist
  */

  if (this->db->execAndGet("SELECT 1 FROM contacts WHERE id = " + std::to_string(contact_id))
        .getInt() == 0) {
    erc = 1;
    return false;
  }

  SQLite::Statement change(*(this->db), "UPDATE contacts SET field = :value WHERE id = :id");
  change.bind(":field", field);
  change.bind(":value", value);
  change.bind(":id", contact_id);

  if (change.tryExecuteStep() != SQLITE_OK) {
    erc = 2;
    return false;
  }

  erc = 0;
  return true;
}

void phi::database::Database::deleteContact(int contact_id) {
  this->db->exec("DELETE FROM contacts WHERE id = " + std::to_string(contact_id));
}

/** **/

bool phi::database::Database::createMessage(int contact_id, bool sender, const std::string& content,
                                            int& erc) {
  /*
    erc: 0 if none, 1 if contact doesn't exist
  */

  if (this->db->execAndGet("SELECT 1 FROM contacts WHERE id = " + std::to_string(contact_id))
        .getInt() == 0) {
    erc = 1;
    return false;
  }

  time_t timestamp = phi::time::getCurrentTime();

  SQLite::Statement add(*(this->db), R"sql(
  INSERT INTO messages (contact_id, sender, content, timestamp, delivered)
  VALUES (:id, :sender, :content, :time))sql");
  add.bind(":id", contact_id);
  // this is stu, the database column type is literally BOOLEAN
  add.bind(":sender", static_cast<int>(sender));
  add.bind(":content", content);
  add.bind(":time", static_cast<long long>(timestamp));

  add.exec();

  erc = 0;
  return true;
}

std::unique_ptr<std::vector<int>> phi::database::Database::getAllMessagesWithContact(int contact_id,
                                                                                     int& erc) {
  /*
    erc: 0 if none, 1 if contact doesn't exist, 2 if no messages exist
  */

  if (this->db->execAndGet("SELECT 1 FROM contacts WHERE id = " + std::to_string(contact_id))
        .getInt() == 0) {
    erc = 1;
    return nullptr;
  }

  // clang-format off
  int rows = this->db->execAndGet("SELECT COUNT(id) FROM messages WHERE contact_id = " + std::to_string(contact_id)).getInt(); // clang
  if (rows == 0) {
    erc = 2;
    return nullptr;
  }
  // clang-format on

  auto messages = std::make_unique<std::vector<int>>(rows);

  SQLite::Statement get_messages(*(this->db), "SELECT id FROM messages WHERE contact_id = :id");
  get_messages.bind(":id", contact_id);
  int idx = 0;
  while (get_messages.tryExecuteStep() == SQLITE_OK) {
    (*messages)[idx] = get_messages.getColumn("id").getInt();
  }

  erc = 0;
  return messages;
}

phi::database::message_t phi::database::Database::getMessage(int message_id, int& erc) {
  /*
    erc: 0 if none, 1 if message doesn't exist
  */

  phi::database::message_t message{};

  SQLite::Statement get_message(*(this->db), "SELECT * FROM messages WHERE id = :id");
  get_message.bind(":id", message_id);

  if (get_message.tryExecuteStep() != SQLITE_OK) {
    erc = 1;
    return message;  // return it empty
  }

  message.id = get_message.getColumn("id").getInt();
  message.contact_id = get_message.getColumn("contact_id").getInt();
  message.sender = static_cast<bool>(get_message.getColumn("sender").getInt());
  message.content = get_message.getColumn("content").getString();
  message.timestamp = phi::time::timeToStr(get_message.getColumn("timestamp").getInt64());
  message.delivered = static_cast<bool>(get_message.getColumn("delivered").getInt());

  return message;
}

/** **/

bool phi::database::Database::createError(const std::string& title, const std::string& description,
                                          int& erc) {
}

bool phi::database::Database::getAllErrors(std::vector<int>& op, int& erc) {
}

bool phi::database::Database::getError(int error_id, error_t& op, int& erc) {
}

bool phi::database::Database::deleteError(int error_id, int& erc) {
}