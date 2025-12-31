/*

 PHI
 2025/12/26

 Phi C++ Project
 include/phi/tasks/TaskMaster.hpp

 Zevi Berlin

*/

#ifndef TASKMASTER_HPP
#define TASKMASTER_HPP

#include <mutex>
#include <string>
#include <memory>

#include <SQLiteCpp/SQLiteCpp.h>

#include "tasks/task_struct.hpp"
#include "utils.hpp"

namespace phi::tasks {

/*
  Going with the model that has reusable SQLite::Statements
    because the queries themselves are trivial and have few
    parameters, whereas phi::database::Database has many functions
    each with unique queries.
*/
class TaskMaster {
  private:
    mutable std::mutex mtx;

    std::unique_ptr<SQLite::Database> db;

    std::string number_tasks_query;
    std::unique_ptr<SQLite::Statement> get_first_query;
    std::unique_ptr<SQLite::Statement> get_next_task_query;
    std::unique_ptr<SQLite::Statement> delete_task_query;
    std::unique_ptr<SQLite::Statement> add_task_query;

    int next_id;

    // private overload for loadNextTask()
    std::unique_lock<std::mutex> resetQueue(std::unique_lock<std::mutex>&& lock);

  public:
    task_t current_task{};

    /*
      erc: 0 if none, 1 if invalid JSON encountered for first task
    */
    TaskMaster(bool is_phi, int& erc);

    void resetQueue();

    /*
      erc: 0 if none, 1 if no more tasks, 2 if invalid JSON encountered
    */
    bool loadNextTask(bool prev_scs, int& erc);

    /**/

    /*
      gives no indication of success or failure
    */
    void addTask(const task_t& task);

    std::unique_lock<std::mutex> deleteTask(std::unique_lock<std::mutex>&& lock);
};

}  // namespace phi::tasks

#endif /* TASKMASTER_HPP */