// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include "keeper.hpp"

#include <cstdlib>
#include <iostream>
#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "../common/membership.hpp"
#include "../common/signals.hpp"
#include "../common/util.hpp"
#include "server_util.hpp"

namespace jubatus {
namespace server {
namespace framework {

__thread msgpack::rpc::session_pool* private_session_pool_ = NULL;
__thread keeper::async_task_loop*
  keeper::async_task_loop::private_async_task_loop_ = NULL;

// NOTE: '__thread' is gcc-extension. We should re-implement with
//       pthread TLS?

keeper::keeper(const keeper_argv& a)
    : keeper_common(a),
      jubatus::server::common::mprpc::rpc_server(a.timeout) {
}

keeper::~keeper() {
}

namespace {

void stop_rpc_server(msgpack::rpc::server& serv) {
  LOG(INFO) << "stopping RPC server";
  serv.end();
}

}  // anonymous namespace

int keeper::run() {
  try {
    this->instance_.listen(a_.bind_address, a_.port);
    LOG(INFO) << "start listening at port " << a_.port;
    this->instance_.start(a_.threadnum);

    // RPC server started, then register group membership
    register_keeper(*zk_, a_.type, a_.eth, a_.port);
    LOG(INFO) << "registered group membership";

    LOG(INFO) << common::util::get_program_name() << " RPC server startup";

    common::util::set_action_on_term(
        pfi::lang::bind(&stop_rpc_server, pfi::lang::ref(this->instance_)));

    this->instance_.join();

    return 0;
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
  } catch (const mp::system_error& e) {
    if (e.code == EADDRINUSE) {
      LOG(FATAL) << "server failed to start: any process using port " << a_.port
          << "?";
    } else {
      LOG(FATAL) << "server failed to start: " << e.what();
    }
  } catch (const std::exception& e) {
    LOG(FATAL) << "server failed to start:" << e.what();
  }
  return -1;
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
