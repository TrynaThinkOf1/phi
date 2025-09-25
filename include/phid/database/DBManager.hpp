/*

 PHI
 2025/09/19

 Phi C++ Project
 include/phid/database/DBManager.hpp

 Zevi Berlin

*/

#ifndef DBMANAGER_HPP
#define DBMANAGER_HPP

#include <shared_mutex>
#include <string>

#include <SQLiteCpp/Database.h>

namespace phid {

class DBManager {
  private:
    mutable std::shared_mutex mutex_;

    SQLite::Database db =
      SQLite::Database("/var/phi/main.db", SQLite::OPEN_READWRITE);

  public:
    DBManager();
    ~DBManager() = default;

    static void close() {
    }
};

}  // namespace phid

#endif /* DBMANAGER_HPP */
