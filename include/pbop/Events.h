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

#ifndef LIB_PROTOBUF_PIPE_PLUGIN_EVENTS
#define LIB_PROTOBUF_PIPE_PLUGIN_EVENTS

#include "pbop/Types.h"
#include "pbop/Status.h"

namespace pbop
{

  class Event
  {
  protected:
	  virtual ~Event() {};
  };

  class EventStartup : public Event
  {
  public:
    EventStartup() {}
    virtual ~EventStartup() {}
  };

  class EventShutdown : public Event
  {
  public:
    EventShutdown() {}
    virtual ~EventShutdown() {}
  };

  class EventListening : public Event
  {
  public:
    EventListening() {}
    virtual ~EventListening() {}
  };

  class EventConnectionBase : public Event
  {
  public:
    EventConnectionBase() {}
    virtual ~EventConnectionBase() {}

    void SetConnectionId(connection_id_t connection_id);
    connection_id_t GetConnectionId() const;
  protected:
    connection_id_t connection_id_;
  };

  class EventConnection : public EventConnectionBase
  {
  public:
    EventConnection() {}
    virtual ~EventConnection() {}
  };

  class EventClientCreate : public EventConnectionBase
  {
  public:
    EventClientCreate() {}
    virtual ~EventClientCreate() {}
  };

  class EventClientDisconnected : public EventConnectionBase
  {
  public:
    EventClientDisconnected() {}
    virtual ~EventClientDisconnected() {}
  };

  class EventClientDestroy : public EventConnectionBase
  {
  public:
    EventClientDestroy() {}
    virtual ~EventClientDestroy() {}
  };

  class EventClientError : public EventConnectionBase
  {
  public:
    EventClientError() {}
    virtual ~EventClientError() {}

    void SetStatus(const Status & status);
    const Status & GetStatus() const;
  protected:
    Status status_;
  };

}; //namespace pbop

#endif //LIB_PROTOBUF_PIPE_PLUGIN_EVENTS