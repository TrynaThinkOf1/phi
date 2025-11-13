/*

 PHI
 2025/11/11

 Phi C++ Project
 src/phid/PipelineManager.cpp

 Zevi Berlin

*/

#include "phid/PipelineManager.hpp"

#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <exception>
#include <unordered_map>

#include "phid/Pipeline.hpp"
#include "Logger.hpp"

#define STDWAIT std::chrono::milliseconds(150)
#define CHUNK 1024

namespace tmc = termcolor;

phid::PipelineManager::PipelineManager(bool& scs, const std::shared_ptr<phi::Logger>& logp)
    : log(logp) {
  int erc = -1;

  try {
    this->pipeline = std::make_unique<phid::Pipeline>(erc);
    log->log("info", "Pipeline successfully created");

    scs = true;
  } catch (std::exception e) {
    switch (erc) {
      case 0:
        log->log("critical", "Failed to create pipeline to frontend. EC: 2B1");
        break;

      case 1:
        log->log("critical", "Failed to bind pipeline socket to frontend. EC: 2B2");
        break;

      case 2:
        log->log("critical", "Failed to listen on pipeline socket to frontend. EC: 2B3");
        break;

      default:
        log->log("critical", "Unknown error occurred while creating pipeline to frontend. EC: 2B0");
        break;
    }

    scs = false;
  }
}

/***/

bool phid::PipelineManager::readForMsg() {
  if (!this->pipeline->isConnected()) return false;  // NOLINT

  std::string delim = "";

  while (delim.empty()) {
    if (this->pipeline->isConnected()) {
      std::this_thread::sleep_for(STDWAIT);
      delim = this->pipeline->read(LONGEST_DELIM);  // NOLINT -- 7 is max delim size
    } else {
      return false;
    }
  }

  if (delim == "</EOT>") return false;  // NOLINT

  std::string type;

  for (const auto& itr : phid::MSG_TYPE) {
    if (itr.second == delim) {
      type = itr.first;
      break;
    }
  }

  if (type.empty()) {
    if (this->pipeline->isConnected()) {
      this->pipeline->send(phid::MSG_TYPE.at("no"));
    }

    return false;
  }

  if (this->pipeline->isConnected()) {
    this->pipeline->send(phid::MSG_TYPE.at("go"));
    std::this_thread::sleep_for(STDWAIT);
  }

  std::string content = " ";

  while (content.at(content.length() - 1) != '>' || content.at(content.length() - 2) == '\\') {
    if (this->pipeline->isConnected()) {
      std::this_thread::sleep_for(STDWAIT);
      content += this->pipeline->read(CHUNK);
    } else {
      return false;
    }
  }

  this->msg_type = type;

  this->stripDelim(content);
  this->msg_content = content;

  return true;
}

/**/

bool phid::PipelineManager::sendResponse(const std::string& msg_type,
                                         const std::string& msg_content) {
  std::string delim;

  auto itr = phid::MSG_TYPE.find(msg_type);
  if (itr != phid::MSG_TYPE.end()) [[likely]] {
    delim = itr->second;
  } else [[unlikely]] {  // this is super unlikely and really only a developer helper
    this->log->log("error", "Invalid message type for pipeline: " + msg_type);
    this->log->~Logger();  // so that it cleans up before exception thrown
    throw std::runtime_error("Invalid message type for pipeline: " + msg_type);
  }

  if (!this->pipeline->isConnected()) return false;  // NOLINT

  this->pipeline->send(delim);
  std::string recv_msg = "      ";  // reserve 6 chars for </GOA>

  while (recv_msg != phid::MSG_TYPE.at("go")) {
    if (!this->pipeline->isConnected() || recv_msg == phid::MSG_TYPE.at("no")) [[unlikely]] {
      return false;
    } else [[likely]] {
      std::this_thread::sleep_for(STDWAIT);
      recv_msg = this->pipeline->read(SHORTEST_DELIM);
    }
  }

  if (this->pipeline->isConnected()) {
    this->pipeline->send(msg_content);
    this->pipeline->send(delim);
  }

  return true;
}