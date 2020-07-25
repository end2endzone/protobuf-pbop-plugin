/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#ifndef LIB_PBOP_EVENTS
#define LIB_PBOP_EVENTS

#include "pbop/Types.h"
#include "pbop/Status.h"

namespace pbop
{

  /// <summary>
  /// Generic base class for all events.
  /// </summary>
  class Event
  {
  protected:
	  virtual ~Event() {};
  };

  /// <summary>
  /// Identify an event that is published when the server starting up.
  /// </summary>
  class EventStartup : public Event
  {
  public:
    EventStartup() {}
    virtual ~EventStartup() {}
  };

  /// <summary>
  /// Identify an event that is published when the server is shutting down.
  /// </summary>
  class EventShutdown : public Event
  {
  public:
    EventShutdown() {}
    virtual ~EventShutdown() {}
  };

  /// <summary>
  /// Identify an event that is published when the server starts listening for incomming connections.
  /// </summary>
  class EventListening : public Event
  {
  public:
    EventListening() {}
    virtual ~EventListening() {}
  };

  /// <summary>
  /// Base class for all events related to a connection.
  /// </summary>
  class EventConnectionBase : public Event
  {
  public:
    EventConnectionBase() {}
    virtual ~EventConnectionBase() {}

    /// <summary>
    /// Set the connection id related to this event.
    /// </summary>
    /// <param name="connection_id">The connection id associated to a connection.</param>
    void SetConnectionId(connection_id_t connection_id);

    /// <summary>
    /// Get the connection id related to this event.
    /// </summary>
    /// <returns>Returns the connection id related to this event.</returns>
    connection_id_t GetConnectionId() const;

  protected:
    connection_id_t connection_id_;
  };

  /// <summary>
  /// Identify an event that is published when a client has connected.
  /// </summary>
  class EventConnection : public EventConnectionBase
  {
  public:
    EventConnection() {}
    virtual ~EventConnection() {}
  };

  /// <summary>
  /// Identify an event that is published when a client session is created.
  /// </summary>
  class EventClientCreate : public EventConnectionBase
  {
  public:
    EventClientCreate() {}
    virtual ~EventClientCreate() {}
  };

  /// <summary>
  /// Identify an event that is published when a client session is disconnected.
  /// </summary>
  class EventClientDisconnected : public EventConnectionBase
  {
  public:
    EventClientDisconnected() {}
    virtual ~EventClientDisconnected() {}
  };

  /// <summary>
  /// Identify an event that is published when a client session is destroyed.
  /// </summary>
  class EventClientDestroy : public EventConnectionBase
  {
  public:
    EventClientDestroy() {}
    virtual ~EventClientDestroy() {}
  };

  /// <summary>
  /// Identify an event that is published when a client session has encountered an error.
  /// The error status is stored in the event.
  /// </summary>
  class EventClientError : public EventConnectionBase
  {
  public:
    EventClientError() {}
    virtual ~EventClientError() {}

    /// <summary>
    /// Set the status result related to this event.
    /// </summary>
    /// <param name="status">The status associated to this event.</param>
    void SetStatus(const Status & status);

    /// <summary>
    /// Get the status result related to this event.
    /// </summary>
    /// <returns>Returns the status result related to this event.</returns>
    const Status & GetStatus() const;
  protected:
    Status status_;
  };

}; //namespace pbop

#endif //LIB_PBOP_EVENTS