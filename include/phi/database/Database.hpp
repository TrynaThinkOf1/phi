/*

 PHI
 2025/09/19

 Phi C++ Project
 include/phi/database/Database.hpp

 Zevi Berlin

*/

#ifndef DATABASE_HPP
#define DATABASE_HPP

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

namespace phi::database {

class Database {
  private:
    mutable std::mutex mtx;

    std::unique_ptr<SQLite::Database> db;

    const std::string self_path = expand("~/.phi/self.json");

  public:
    self_t self;

    Database(int& erc);

    /** **/

    bool createSelf(const std::string& name, const std::string& emoji,
                    const std::string& rsa_pub_key, const std::string& rsa_priv_key,
                    const std::string& ipv6_addr, const std::string& hardware_profile, int& erc);

    bool changeSelfAttribute(const std::string& field, const std::string& value, int& erc);

    /** **/

    bool createContact(const std::string& name, const std::string& emoji, const std::string& addr,
                       const std::string& shared_secret, const std::string& rsa_key, int& erc,
                       int& op_id);

    std::unique_ptr<std::vector<std::tuple<int, std::string, std::string>>> getAllContacts();

    bool getContact(int contact_id, contact_t& op, int& erc);

    bool changeContactAttribute(int contact_id, const std::string& field, const std::string& value,
                                int& erc);

    void deleteContact(int contact_id);

    /** **/

    bool createMessage(int contact_id, bool sender, const std::string& content, int& erc);

    std::unique_ptr<std::vector<int>> getAllMessagesWithContact(int contact_id, int& erc);

    message_t getMessage(int message_id, int& erc);

    /** **/

    bool createError(const std::string& title, const std::string& description, int& erc);

    bool getAllErrors(std::vector<int>& op, int& erc);

    bool getError(int error_id, error_t& op, int& erc);

    bool deleteError(int error_id, int& erc);
};

}  // namespace phi::database

#endif /* DATABASE_HPP */
