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

#include "phi/database/structs.hpp"
#include "datetime.hpp"
#include "utils/file_utils.hpp"

namespace phi::database {

class Database {
  private:
    mutable std::mutex mtx;

    std::unique_ptr<SQLite::Database> db;

    const std::string self_path = expand("~/.phi/self.json");

    std::string password;

    std::unique_ptr<SQLite::Statement> contact_check_query;
    std::unique_lock<std::mutex> checkContact(std::unique_lock<std::mutex>&& lock, int contact_id,
                                              bool& exists);

  public:
    self_t self;

    /*
      erc: 0 if no error, 1 if self is invalid
    */
    Database(int& erc);

    bool login(const std::string& password);

    void createTables();

    bool changePassword(const std::string& oldpass, const std::string& newpass,
                        const std::string& newhint);

    /** **/

    /*
      erc: 0 if none, 1 if self can't be created
    */
    bool createSelf(const std::string& name, const std::string& emoji, const std::string& hint,
                    const std::string& rsa_pub_key, const std::string& rsa_priv_key,
                    const std::string& ipv6_addr, const std::string& hardware_profile, int& erc);

    /*
      erc: 0 if none, 1 if self can't be opened
    */
    bool updateSelf(const self_t& new_self);

    /** **/

    /*
      erc: 0 if none, 1 if name exists, 2 if IPv6 addr already exists, 3 if unknown error

      op_id gets set to -1 if operation fails
    */
    bool createContact(const std::string& name, const std::string& emoji, const std::string& addr,
                       const std::string& shared_secret, const std::string& rsa_key, int& erc,
                       int& op_id);

    /*
      returns a vector of tuples, the tuples being:

      {id, name, emoji} or a nullptr if there are no contacts
    */
    std::unique_ptr<std::vector<std::tuple<int, std::string, std::string>>> getAllContacts();

    bool getContact(int contact_id, contact_t& op);

    bool updateContact(const contact_t& current, const contact_t& to_set);

    /*
      this will NOT give any indication as to whether or
      not the operation succeeded because errors aren't that important
    */
    void deleteContact(int contact_id);

    /** **/

    /*
      erc: 0 if none, 1 if contact doesn't exist
    */
    bool createMessage(int contact_id, bool sender, const std::string& content, int& erc);

    /*
      erc: 0 if none, 1 if contact doesn't exist, 2 if no messages

      returns a nullptr if there are no messages
    */
    std::unique_ptr<std::vector<int>> getAllMessagesWithContact(int contact_id, int& erc);

    /*
      erc: 0 if none, 1 if message doesn't exist
    */
    message_t getMessage(int message_id, int& erc);

    /*
      this will NOT give any indication as to whether or not the operation succeeded
    */
    void deliverMessage(int message_id);

    /*
      the purpose of this function is such that it will be hidden from a conversation,
      it does NOT affect the other contact's stored version of the conversation
    */
    void eraseMessage(int message_id);

    /** **/

    /*
      this will NOT give any indication as to whether or not the operation succeeded because
      errors aren't that important
    */
    void createError(const std::string& title, const std::string& description);

    /*
      returns a nullptr if there are no errors
    */
    std::unique_ptr<std::vector<int>> getAllErrors();

    /*
      erc: 0 if none, 1 if the error doesn't exist
    */
    error_t getError(int error_id, int& erc);

    /*
      this will NOT give any indication as to whether or not the operation succeeded because
      errors aren't that important
    */
    void deleteError(int error_id);
};

}  // namespace phi::database

#endif /* DATABASE_HPP */
