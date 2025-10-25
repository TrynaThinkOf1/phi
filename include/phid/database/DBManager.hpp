/*

 PHI
 2025/09/19

 Phi C++ Project
 include/phid/database/DBManager.hpp

 Zevi Berlin

*/

#ifndef DBMANAGER_HPP
#define DBMANAGER_HPP

#include <mutex>
#include <string>

#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/VariadicBind.h>

#include "utils.hpp"

#define LINE_LIM (const int)32

namespace phid {

/** STRUCTS **/

struct self_t {  // NOLINT -- I want to use self_t as self type
    std::string name;
    std::string emoji;
    std::string rsa_pub_key;
    std::string rsa_priv_key;
    std::string last_known_ip;
    std::string hardware_profile;

    /***/

    std::string toString() const {
      return "NAME: " + this->name + "\nEMOJI: " + this->emoji +
             "\nRSA PUB KEY (first 32): " + toHex(this->rsa_pub_key.substr(0, LINE_LIM)) +
             "\nRSA PRIV KEY (first 32): " + toHex(this->rsa_priv_key.substr(0, LINE_LIM)) +
             "\nIP: " + this->last_known_ip + "\nHARDWARE PROFILE: " + this->hardware_profile;
    }
};

struct contact_t {  // NOLINT ^
    std::string name;
    std::string emoji;
    std::string rsa_pub_key;
    std::string shared_secret;
    std::string ipv6_addr;
    std::string hardware_profile;

    /***/

    std::string toString() const {
      return "NAME: " + this->name + "\nEMOJI: " + this->emoji +
             "\nRSA PUB KEY (first 32): " + toHex(this->rsa_pub_key.substr(0, LINE_LIM)) +
             "\nSHARED SECRET: " + toHex(this->shared_secret + "\nIP: ") + this->ipv6_addr +
             "\nHARDWARE PROFILE: " + this->hardware_profile;
    }
};

struct message_t {  // NOLINT ^^
    int contact_id;
    std::string content;
    std::string timestamp;
    bool is_read;
    bool is_delivered;

    /***/

    std::string toString() const {
      return "CONTACT (ID): " + std::to_string(this->contact_id) + "\nCONTENT: " + this->content +
             "\nTIMESTAMP: " + this->timestamp + "\nREAD: " + (is_read ? "true" : "false") +
             "\nDELIVERED: " + (is_delivered ? "true" : "false");
    }
};

struct derror_t {  // NOLINT ^^^
    int derror_id;
    std::string title;
    std::string description;
    std::string timestamp;
    bool is_reported;

    /***/

    std::string toString() const {
      return "ID: " + std::to_string(this->derror_id) + "\nTITLE: " + this->title +
             "\nDESCRIPTION: " + this->description + "\nTIMESTAMP: " + this->timestamp +
             "\nREPORTED: " + (is_reported ? "true" : "false");
    }
};

/** **/

class DBManager {
  private:
    /*****      *****\
     LIFETIME OBJECTS
    \*****      *****/
    mutable std::mutex _mtx;

    SQLite::Database* db;
    /***** *****\
    \***** *****/
  public:
    self_t self;

    /** CONSTRUCTOR & DESTRUCTOR **/
    DBManager(bool& scs);
    ~DBManager();
    /** **/

    /** SELF **/

    void createSelf(std::string name, std::string emoji, const std::string& rsa_pub_key,
                    const std::string& rsa_priv_key, const std::string& ipv6_addr,
                    const std::string& hardware_profile);

    bool updateSelf();

    void changeSelfAttribute(const std::string& field, const std::string& value);

    /** **/

    /** CONTACTS **/

    int createContact(std::string name, std::string emoji, const std::string& rsa_pub_key,
                      const std::string& shared_secret, const std::string& ipv6_addr,
                      const std::string& hardware_profile);

    bool getContact(int contact_id, contact_t& op);

    int changeContactAttribute(int contact_id, const std::string& field, const std::string& value);

    void deleteContact(int contact_id);

    /** **/
};

}  // namespace phid

#endif /* DBMANAGER_HPP */
