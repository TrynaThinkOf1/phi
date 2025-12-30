/*

 PHI
 2025/12/26

 Phi C++ Project
 src/TaskMaster.cpp

 Zevi Berlin

*/

#include "tasks/TaskMaster.hpp"

#include <mutex>
#include <string>
#include <memory>

#include <SQLiteCpp/SQLiteCpp.h>

#include "tasks/task_struct.hpp"
#include "utils.hpp"

phi::tasks::TaskMaster::TaskMaster(bool is_phi) {
  if (is_phi) {
    this->recv_task_table = "phi_tasks";
    this->send_task_table = "phid_tasks";
  } else {
    this->recv_task_table = "phid_tasks";
    this->send_task_table = "phi_tasks";
  }

  this->db =
    std::make_unique<SQLite::Database>(expand("~/.phi/tasks.db"), SQLite::OPEN_READWRITE);  // NOLINT

  this->db->exec(
    R"sql(
      CREATE TABLE IF NOT EXISTS phi_tasks (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        code INTEGER NOT NULL,
        data JSON NOT NULL DEFAULT '{}'
      );

      CREATE TABLE IF NOT EXISTS phid_tasks (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        code INTEGER NOT NULL,
        data JSON NOT NULL DEFAULT '{}'
      );
    )sql");

  /**/

  SQLite::Statement get_first(*(this->db), "SELECT id FROM " + this->recv_task_table);
  if (!get_first.executeStep()) {
    this->next_id = 0;
  } else {
    this->next_id = get_first.getColumn("id").getInt();
  }
}