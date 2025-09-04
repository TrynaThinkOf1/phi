/*

 PHI
 2025/09/03

 Phi C++ Project
 include/database/DBManager.hpp

 Zevi Berlin

*/

#ifndef DBMANAGER_HPP
#define DBMANAGER_HPP

#include <string>
#include <memory>
#include <sqlite3.h>

namespace phi {

class DBManager {
  private:
    std::unique_ptr<sqlite3> database;

  public:
    DBManager();
};

}  // namespace phi

#endif /* DBMANAGER_HPP */
