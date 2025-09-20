/*

 PHI
 2025/09/19

 Phi C++ Project
 include/phid/database/DBManager.hpp

 Zevi Berlin

*/

#ifndef DBMANAGER_HPP
#define DBMANAGER_HPP

#include <atomic>

#include <SQLiteCpp/Database.h>

namespace phid {

class DBManager {
  private:
    SQLite::Database db =
      SQLite::Database("/usr/local/phi/main.db", SQLite::OPEN_READWRITE);

  public:
    DBManager();
    ~DBManager() = default;
};

// const auto __dbman = std::make_shared<DBManager>();
// const auto dbmanager = std::atomic<std::shared_ptr<DBManager>>(__dbman);

}  // namespace phid

#endif /* DBMANAGER_HPP */
