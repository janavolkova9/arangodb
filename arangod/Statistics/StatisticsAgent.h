////////////////////////////////////////////////////////////////////////////////
/// @brief statistics agents
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
/// @author Copyright 2014-2016, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2012-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_STATISTICS_STATISTICS_AGENT_H
#define ARANGODB_STATISTICS_STATISTICS_AGENT_H 1

#include "Basics/Common.h"

#include "Statistics/statistics.h"

// -----------------------------------------------------------------------------
// --SECTION--                                             class StatisticsAgent
// -----------------------------------------------------------------------------

namespace triagens {
namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// @brief request statistics agent
////////////////////////////////////////////////////////////////////////////////

template <typename STAT, typename FUNC>
class StatisticsAgent {
  StatisticsAgent(StatisticsAgent const&) = delete;
  StatisticsAgent& operator=(StatisticsAgent const&) = delete;

  // ---------------------------------------------------------------------------
  // --SECTION--                                    constructors and destructors
  // ---------------------------------------------------------------------------

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief constructs a new agent
  //////////////////////////////////////////////////////////////////////////////

  StatisticsAgent() : _statistics(nullptr), _lastReadStart(0.0) {}

  //////////////////////////////////////////////////////////////////////////////
  /// @brief destructs an agent
  //////////////////////////////////////////////////////////////////////////////

  ~StatisticsAgent() {
    if (_statistics != nullptr) {
      FUNC::release(_statistics);
    }
  }

  // ---------------------------------------------------------------------------
  // --SECTION--                                                  public methods
  // ---------------------------------------------------------------------------

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief acquires a new statistics block
  //////////////////////////////////////////////////////////////////////////////

  STAT* acquire() {
    if (_statistics != nullptr) {
      return _statistics;
    }

    _lastReadStart = 0.0;
    return _statistics = FUNC::acquire();
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief releases a statistics block
  //////////////////////////////////////////////////////////////////////////////

  void release() {
    if (_statistics != nullptr) {
      FUNC::release(_statistics);
      _statistics = nullptr;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief transfers statistics information to another agent
  //////////////////////////////////////////////////////////////////////////////

  void transfer(StatisticsAgent* agent) {
    agent->replace(_statistics);
    _statistics = nullptr;
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief transfers statistics information
  //////////////////////////////////////////////////////////////////////////////

  STAT* transfer() {
    STAT* statistics = _statistics;
    _statistics = nullptr;

    return statistics;
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the time elapsed since read started
  //////////////////////////////////////////////////////////////////////////////

  double elapsedSinceReadStart() {
    if (_lastReadStart != 0.0) {
      return TRI_StatisticsTime() - _lastReadStart;
    }

    return 0.0;
  }

  // ---------------------------------------------------------------------------
  // --SECTION--                                                public variables
  // ---------------------------------------------------------------------------

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief statistics
  //////////////////////////////////////////////////////////////////////////////

  STAT* _statistics;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief last read
  //////////////////////////////////////////////////////////////////////////////

  double _lastReadStart;

  // ---------------------------------------------------------------------------
  // --SECTION--                                                  public methods
  // ---------------------------------------------------------------------------

 protected:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief replaces a statistics block
  //////////////////////////////////////////////////////////////////////////////

  void replace(STAT* statistics) {
    if (_statistics != nullptr) {
      FUNC::release(_statistics);
    }

    _statistics = statistics;
  }
};
}
}

// -----------------------------------------------------------------------------
// --SECTION--                                      class RequestStatisticsAgent
// -----------------------------------------------------------------------------

namespace triagens {
namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// @brief request statistics description
////////////////////////////////////////////////////////////////////////////////

struct RequestStatisticsAgentDesc {
  static TRI_request_statistics_t* acquire() {
    return TRI_AcquireRequestStatistics();
  }

  static void release(TRI_request_statistics_t* stat) {
    TRI_ReleaseRequestStatistics(stat);
  }
};

////////////////////////////////////////////////////////////////////////////////
/// @brief request statistics agent
////////////////////////////////////////////////////////////////////////////////

typedef StatisticsAgent<TRI_request_statistics_t, RequestStatisticsAgentDesc>
    RequestStatisticsAgent;
}
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the request type
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetRequestType(
    triagens::rest::RequestStatisticsAgent* a,
    triagens::rest::HttpRequest::HttpRequestType b) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_requestType = b;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @Brief sets the async flag
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetAsync(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_async = true;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the read start
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetReadStart(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_lastReadStart = a->_statistics->_readStart = TRI_StatisticsTime();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the read end
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetReadEnd(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_readEnd = TRI_StatisticsTime();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the write start
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetWriteStart(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_writeStart = TRI_StatisticsTime();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the write end
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetWriteEnd(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_writeEnd = TRI_StatisticsTime();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the queue start
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetQueueStart(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_queueStart = TRI_StatisticsTime();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the queue end
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetQueueEnd(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_queueEnd = TRI_StatisticsTime();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the request start
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetRequestStart(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_requestStart = TRI_StatisticsTime();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the request end
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetRequestEnd(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_requestEnd = TRI_StatisticsTime();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets execution error
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetExecuteError(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_executeError = true;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets ignore flag
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentSetIgnore(
    triagens::rest::RequestStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_ignore = true;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief adds bytes received
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentAddReceivedBytes(
    triagens::rest::RequestStatisticsAgent* a, size_t b) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_receivedBytes += (b);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief adds bytes sent
////////////////////////////////////////////////////////////////////////////////

inline void RequestStatisticsAgentAddSentBytes(
    triagens::rest::RequestStatisticsAgent* a, size_t b) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_sentBytes += (b);
    }
  }
}

// -----------------------------------------------------------------------------
// --SECTION--                                   class ConnectionStatisticsAgent
// -----------------------------------------------------------------------------

namespace triagens {
namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// @brief connection statistics description
////////////////////////////////////////////////////////////////////////////////

struct ConnectionStatisticsAgentDesc {
  static TRI_connection_statistics_t* acquire() {
    return TRI_AcquireConnectionStatistics();
  }

  static void release(TRI_connection_statistics_t* stat) {
    TRI_ReleaseConnectionStatistics(stat);
  }
};

////////////////////////////////////////////////////////////////////////////////
/// @brief connection statistics agent
////////////////////////////////////////////////////////////////////////////////

typedef StatisticsAgent<TRI_connection_statistics_t,
                        ConnectionStatisticsAgentDesc>
    ConnectionStatisticsAgent;
}
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the connection type
////////////////////////////////////////////////////////////////////////////////

inline void ConnectionStatisticsAgentSetHttp(
    triagens::rest::ConnectionStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_http = true;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the connection start
////////////////////////////////////////////////////////////////////////////////

inline void ConnectionStatisticsAgentSetStart(
    triagens::rest::ConnectionStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_connStart = TRI_StatisticsTime();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the connection end
////////////////////////////////////////////////////////////////////////////////

inline void ConnectionStatisticsAgentSetEnd(
    triagens::rest::ConnectionStatisticsAgent* a) {
  if (TRI_ENABLE_STATISTICS) {
    if (a->_statistics != nullptr) {
      a->_statistics->_connEnd = TRI_StatisticsTime();
    }
  }
}

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------
