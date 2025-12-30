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

class TaskMaster {
  private:
    mutable std::mutex mtx;

    std::unique_ptr<SQLite::Database> db;

    std::string recv_task_table;
    std::string send_task_table;

    int next_id;

  public:
    task_t current_task{};

    TaskMaster(bool is_phi);
};

}  // namespace phi::tasks

#endif /* TASKMASTER_HPP */