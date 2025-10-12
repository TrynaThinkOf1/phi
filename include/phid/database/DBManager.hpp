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

namespace phid {

struct self_t {
    std::string name;
    std::string emoji;
    std::string rsa_priv_key;
    std::string last_known_ip;
    std::string hardware_profile;

    std::string to_string() {
      return "NAME: " + this->name + "\nEMOJI: " + this->emoji +
             "\nRSA KEY (first 32): " + this->rsa_priv_key.substr(0, 32) +
             "\nIP: " + this->last_known_ip + "\nHARDWARE PROFILE: " + this->hardware_profile;
    }
};

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
    DBManager(int& op_code);
    ~DBManager();
    /** **/

    /** TEMPLATE FUNCTIONS **/

    /** **/

    bool update_self();

    /***/

    void change_self_attribute(const std::string& field, const std::string& value);

    /***/
};

}  // namespace phid

#endif /* DBMANAGER_HPP */
