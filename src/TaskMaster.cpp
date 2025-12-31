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

phi::tasks::TaskMaster::TaskMaster(bool is_phi, int& erc) {
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

  this->number_tasks_query =
    "SELECT COUNT(id) FROM phi" + std::string(is_phi ? "" : "d") + "_tasks";

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
    erc = 0;
  } else {
    this->next_id = this->get_first_query->getColumn("id").getInt();
    // retrieve first task in such a way that it isn't removed nor repeated
    this->get_next_task_query->bind(":id", this->next_id);
    this->get_next_task_query->executeStep();
    if (!this->current_task.data_from_json_str(
          this->get_next_task_query->getColumn("data").getString())) {
      erc = 1;
      this->current_task.data_from_json_str("{}");
      this->current_task.code = -1;
    } else {
      this->current_task.code = this->get_next_task_query->getColumn("code").getInt();
      erc = 0;
    }
  }

  this->get_first_query->reset();
  this->get_next_task_query->reset();
}

/***/

std::unique_lock<std::mutex> phi::tasks::TaskMaster::resetQueue(
  std::unique_lock<std::mutex>&& lock) {
  if (!this->get_first_query->executeStep()) {
    this->next_id = 0;
  } else {
    this->next_id = this->get_first_query->getColumn("id").getInt();
  }

  this->get_first_query->reset();

  return std::move(lock);
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

  if (prev_scs) lock = this->deleteTask(std::move(lock));

  this->next_id++;

  if (this->db->execAndGet(this->number_tasks_query).getInt() > 0) {
    this->get_next_task_query->bind(":id", this->next_id);

    if (!this->get_next_task_query->executeStep()) {
      lock = this->resetQueue(std::move(lock));
      this->get_next_task_query->reset();
      this->get_next_task_query->bind(":id", this->next_id);
      // no need to validate because nonzero # of tasks is verified and this->next_id is the head
      this->get_next_task_query->executeStep();
    }

    if (!this->current_task.data_from_json_str(
          this->get_next_task_query->getColumn("data").getString())) {
      this->get_next_task_query->reset();
      this->current_task.code = -1;
      this->current_task.data_from_json_str("{}");

      lock = this->deleteTask(std::move(lock));

      erc = 2;
      return false;
    }

    this->current_task.code = this->get_next_task_query->getColumn("code").getInt();
  } else {
    erc = 1;
    return false;
  }

  this->get_next_task_query->reset();
  erc = 0;
  return true;
}

/**/

void phi::tasks::TaskMaster::addTask(const phi::tasks::task_t& task) {
  std::lock_guard<std::mutex> lock(this->mtx);

  this->add_task_query->bind(":code", task.code);
  this->add_task_query->bind(":data", task.data_to_json_str());
  this->add_task_query->exec();
  this->add_task_query->reset();
}

std::unique_lock<std::mutex> phi::tasks::TaskMaster::deleteTask(
  std::unique_lock<std::mutex>&& lock) {
  this->delete_task_query->bind(":id", this->next_id);
  this->delete_task_query->exec();
  this->delete_task_query->reset();

  return std::move(lock);
}