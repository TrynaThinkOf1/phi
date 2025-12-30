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
  this->db = std::make_unique<SQLite::Database>(expand("~/.phi/tasks.db"),
                                                SQLite::OPEN_READWRITE);  // NOLINT

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

  this->get_first_query = std::make_unique<SQLite::Statement>(
    *(this->db), "SELECT id FROM phi" + std::string(is_phi ? "" : "d") + "_tasks");

  this->get_next_task_query = std::make_unique<SQLite::Statement>(
    *(this->db), "SELECT * FROM phi" + std::string(is_phi ? "" : "d") + "_tasks WHERE id = :id");

  this->delete_task_query = std::make_unique<SQLite::Statement>(
    *(this->db), "DELETE FROM phi" + std::string(is_phi ? "" : "d") + "_tasks WHERE id = :id");

  this->add_task_query = std::make_unique<SQLite::Statement>(
    *(this->db), "INSERT INTO phi" + std::string(is_phi ? "d" : "") +
                   "_tasks (code, data) VALUES (:code, :data)");

  if (!this->get_first_query->executeStep()) {
    this->next_id = 0;
  } else {
    this->next_id = this->get_first_query->getColumn("id").getInt();
  }
  this->get_first_query->reset();
}

/***/

void phi::tasks::TaskMaster::resetQueue() {
  std::lock_guard<std::mutex> lock(this->mtx);

  if (!this->get_first_query->executeStep()) {
    this->next_id = 0;
  } else {
    this->next_id = this->get_first_query->getColumn("id").getInt();
  }

  this->get_first_query->reset();
}

bool phi::tasks::TaskMaster::loadNextTask(bool prev_scs, int& erc) {
  std::unique_lock<std::mutex> lock(this->mtx);
}

/**/