/*

 PHI
 2026/01/01

 Phi C++ Project
 src/phi/Manager.cpp

 Zevi Berlin

*/

#include "phi/ui/Manager.hpp"

#include <string>
#include <memory>
#include <vector>
#include <thread>

#include <sys/ioctl.h>
#include <unistd.h>

#include <ftxui/screen/screen.hpp>

#include "phi/database/Database.hpp"
#include "phi/database/structs.hpp"
#include "tasks/TaskMaster.hpp"
#include "tasks/task_struct.hpp"
#include "phi/encryption/Encryptor.hpp"
#include "phi/encryption/secrets.hpp"
#include "phi/ui/color_defs.hpp"

phi::ui::Manager::Manager(std::shared_ptr<phi::database::Database> database,
                          std::shared_ptr<phi::encryption::Encryptor> encryptor,
                          std::shared_ptr<phi::tasks::TaskMaster> taskmaster)
    : DATABASE(std::move(database)),
      ENCRYPTOR(std::move(encryptor)),
      TASKMASTER(std::move(taskmaster)) {
  this->screen.Print();
}

/**/

bool phi::ui::Manager::loginPage() {
  return true;
}