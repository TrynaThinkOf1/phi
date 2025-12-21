/*

 PHI
 2025/12/13

 Phi C++ Project
 include/phi/do_setup.hpp

 Zevi Berlin

*/

#ifndef DO_SETUP_HPP
#define DO_SETUP_HPP

#include <iostream>
#include <memory>
#include <string>

#include "termcolor/termcolor.hpp"

#include "phi/database/Database.hpp"
#include "phi/encryption/Encryptor.hpp"

bool do_setup(const std::shared_ptr<phi::database::Database>& DATABASE,
              const std::shared_ptr<phi::encryption::Encryptor>& ENCRYPTOR);

#endif /* DO_SETUP_HPP */
