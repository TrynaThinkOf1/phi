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

struct Self {
    std::string name;
    std::string emoji;
    std::string rsa_priv_key;
    std::string last_known_ip;
    std::string hardware_profile;
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
    Self self;

    /** CONSTRUCTOR & DESTRUCTOR **/
    DBManager(int& op_code);
    ~DBManager();
    /** **/

    void initialize_self();

    void change_self_name(const std::string& new_name);
    void change_self_emoji(const std::string& new_emoji);
    void change_self_rsa_priv_key(const std::string& new_rsa_priv_key);
    void change_self_hardware_profile(const std::string& new_hardware_profile);

    /***/
};

}  // namespace phid

#endif /* DBMANAGER_HPP */
