/*

 PHI
 2025/11/11

 Phi C++ Project
 include/phid/PipelineManager.hpp

 Zevi Berlin

*/

#ifndef PIPELINEMANAGER_HPP
#define PIPELINEMANAGER_HPP

#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <exception>
#include <map>

#include <termcolor/termcolor.hpp>

#include "phid/Pipeline.hpp"
#include "Logger.hpp"

namespace tmc = termcolor;

namespace phid {

constexpr short LONGEST_DELIM = 7;
constexpr short SHORTEST_DELIM = 6;

const std::map<const std::string, const std::string> MSG_TYPE{
  {"go", "</GOA>"},  // go ahead
  {"no", "</NOG>"},  // no go

  {"text", "</TXT>"},   // text response
  {"num", "</NUM>"},    // numerical response
  {"error", "</ERR>"},  // error response

  {"ccon", "</CCON>"},  // create contact
  {"rcon", "</RCON>"},  // retrieve contact
  {"acon", "</ACON>"},  // alter contact
  {"dcon", "</DCON>"},  // delete contact

  {"cmsg", "</CMSG>"},  // create message
  {"rmsg", "</RMSG>"},  // retrieve message
};

/***/

class PipelineManager {
  private:
    std::shared_ptr<phi::Logger> log;

    std::unique_ptr<phid::Pipeline> pipeline;

    std::string msg_type;     // store the most recent recv msg type
    std::string msg_content;  // store the most recent recv msg content

    static void stripDelim(std::string& msg) {
      std::string delim = msg.substr(msg.length() - SHORTEST_DELIM);
      if (delim.at(0) != '<') {
        msg.resize(msg.length() - LONGEST_DELIM);
        return;
      }

      msg.resize(msg.length() - SHORTEST_DELIM);
    }

  public:
    PipelineManager(bool& scs, const std::shared_ptr<phi::Logger>& log);

    /***/

    bool isConnected() {
      return this->pipeline->isConnected();
    }

    std::string getMsgType() const {
      return this->msg_type;
    }

    std::string getMsgContent() const {
      return this->msg_content;
    }

    /***/

    bool readForMsg();

    bool sendResponse(const std::string& msg_type, const std::string& msg_content);
};

}  // namespace phid

#endif /* PIPELINEMANAGER_HPP */
