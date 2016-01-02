////////////////////////////////////////////////////////////////////////////////
/// @brief general server job
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Achim Brandt
/// @author Copyright 2014-2016, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2009-2014, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "HttpServerJob.h"

#include "Basics/WorkMonitor.h"
#include "Basics/logging.h"
#include "Dispatcher/DispatcherQueue.h"
#include "HttpServer/AsyncJobManager.h"
#include "HttpServer/HttpCommTask.h"
#include "HttpServer/HttpHandler.h"
#include "HttpServer/HttpServer.h"
#include "Scheduler/Scheduler.h"

using namespace arangodb;
using namespace triagens::rest;
using namespace std;

// -----------------------------------------------------------------------------
// --SECTION--                                               class HttpServerJob
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief constructs a new server job
////////////////////////////////////////////////////////////////////////////////

HttpServerJob::HttpServerJob(HttpServer* server,
                             WorkItem::uptr<HttpHandler>& handler,
                             bool isAsync)
  : Job("HttpServerJob"), _server(server), _isAsync(isAsync) {
  _handler.swap(handler);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief destructs a server job
////////////////////////////////////////////////////////////////////////////////

HttpServerJob::~HttpServerJob() { WorkMonitor::releaseHandler(_handler); }

// -----------------------------------------------------------------------------
// --SECTION--                                                       Job methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

size_t HttpServerJob::queue() const { return _handler->queue(); }

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

void HttpServerJob::work() {
  TRI_ASSERT(_handler != nullptr);

  LOG_TRACE("beginning job %p", (void*)this);

  // start working with handler
  RequestStatisticsAgent::transfer(_handler.get());

  {
    HandlerWorkStack work(_handler, false);
    _handler->executeFull();
  }

  LOG_TRACE("finished job %p", (void*)this);

  if (! _isAsync) {
    std::unique_ptr<TaskData> data(new TaskData());
    data->_taskId = _handler->taskId();
    data->_loop = _handler->eventLoop();
    data->_type = TaskData::TASK_DATA_RESPONSE;
    data->_response = _handler->stealResponse();

    Scheduler::SCHEDULER->signalTask(data);
  }

  // the handler is no longer needed
  WorkMonitor::releaseHandler(_handler);
  _handler = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

bool HttpServerJob::cancel() { return _handler->cancel(); }

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

void HttpServerJob::cleanup(DispatcherQueue* queue) {
  if (_isAsync) {
    _server->jobManager()->finishAsyncJob(this);
  }

  queue->removeJob(this);
  delete this;
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

void HttpServerJob::handleError(triagens::basics::Exception const& ex) {
  _handler->handleError(ex);
}

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------
