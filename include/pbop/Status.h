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

#ifndef LIB_PBOP_STATUS
#define LIB_PBOP_STATUS

#include <string>

#include <google/protobuf/message.h>

namespace pbop
{

  /// <summary>
  /// Status code enumeration for the Status class.
  /// </summary>
  enum StatusCode
  {
    STATUS_CODE_SUCCESS,              // Not an error. The operation completed successfully.
    STATUS_CODE_UNKNOWN,              // An unexpected error has occurred.
    STATUS_CODE_SERIALIZE_ERROR,      // An error occured while serializing a message into a string.
    STATUS_CODE_DESERIALIZE_ERROR,    // An error occured while deserializing a message from a string.
    STATUS_CODE_TIMED_OUT,            // The operation has timed out.
    STATUS_CODE_CANCELLED,            // The operation was cancelled by the server.
    STATUS_CODE_NOT_IMPLEMENTED,      // The requested service method is not implemented in the server.
    STATUS_CODE_INVALID_ARGUMENT,     // The specified message argument is invalid.
    STATUS_CODE_OUT_OF_RANGE,         // A value in the request message is out of range.
    STATUS_CODE_OUT_OF_MEMORY,        // Not enough memory to process the operation.
    STATUS_CODE_PIPE_ERROR,           // A pipe related error occured.
    STATUS_CODE_IMPLEMENTATION,       // An error in the server's specific service implementation occured.
  };

  class Status
  {
  public:
    Status();
    Status(const Status & other);
    Status(const StatusCode & code, const std::string & message);
    virtual ~Status();

    /// <summary>
    /// Set a code to this status.
    /// </summary>
    /// <param name="code">The new code value.</param>
    void SetCode(const StatusCode & code);

    /// <summary>
    /// Get the code assigned to this status.
    /// </summary>
    /// <returns>Returns the code assigned to this status.</returns>
    const StatusCode & GetCode() const;

    /// <summary>
    /// Set an error description string to this status.
    /// </summary>
    /// <param name="description">The new error description string.</param>
    void SetDescription(const std::string & description);

    /// <summary>
    /// Get the error description string assigned to this status.
    /// </summary>
    /// <returns>Returns the error description string assigned to this status.</returns>
    const std::string & GetDescription() const;

    /// <summary>
    /// Provides the error state of the status. Either success or any other error.
    /// </summary>
    /// <returns>Returns true if the code assigned to this status is STATUS_CODE_SUCCESS. Returns false otherwise.</returns>
    bool Success();

    friend void swap(Status & first, Status & second);
    Status & operator=(Status other);
    bool operator==(const Status & other);
    bool operator!=(const Status & other);

    /// <returns>Returns a Status instance which code is set to STATUS_CODE_SUCCESS.</returns>
    static const Status & OK;

    /// <summary>
    /// Provides a generic error description string for a given code.
    /// </summary>
    /// <param name="code">The code to get the error description.</param>
    /// <returns>Returns a string that matches a generic description for a status code.</returns>
    static const char * ToString(const StatusCode & code);

    /// <summary>
    /// Factory class for creating Status with similar error message.
    /// </summary>
    class Factory
    {
    public:
      /// <summary>
      /// Create a Status when an Out of Memory error is found.
      /// </summary>
      /// <param name="function">The name of the function that encountered the out of memory error.</param>
      /// <returns>Returns a Status instance which code is set to STATUS_CODE_OUT_OF_MEMORY.</returns>
      static Status OutOfMemory(const char * function);

      /// <summary>
      /// Create a Status when an serialization error is found.
      /// </summary>
      /// <param name="function">The name of the function that encountered the out of memory error.</param>
      /// <param name="message">The protobuf message that was serialized.</param>
      /// <returns>Returns a Status instance which code is set to STATUS_CODE_SERIALIZE_ERROR.</returns>
      static Status Serialization(const char * function, const ::google::protobuf::Message & message);

      /// <summary>
      /// Create a Status when an deserialization error is found.
      /// </summary>
      /// <param name="function">The name of the function that encountered the out of memory error.</param>
      /// <param name="message">The protobuf message that was deserialized.</param>
      /// <returns>Returns a Status instance which code is set to STATUS_CODE_DESERIALIZE_ERROR.</returns>
      static Status Deserialization(const char * function, const ::google::protobuf::Message & message);

      /// <summary>
      /// Create a Status when a protobuf field is missing.
      /// </summary>
      /// <param name="function">The name of the function that encountered the out of memory error.</param>
      /// <param name="field">The name of the field that is missing in the protobuf message.</param>
      /// <param name="message">The protobuf message that had a missing field.</param>
      /// <returns>Returns a Status instance which code is set to STATUS_CODE_DESERIALIZE_ERROR.</returns>
      static Status MissingField(const char * function, const char * field, const ::google::protobuf::Message & message);

      /// <summary>
      /// Create a Status when a function is not implemented.
      /// </summary>
      /// <param name="function">The name of the function that encountered the out of memory error.</param>
      /// <returns>Returns a Status instance which code is set to STATUS_CODE_NOT_IMPLEMENTED.</returns>
      static Status NotImplemented(const char * function);
    };

  private:
    StatusCode code_;
    std::string description_;
  };

}; //namespace pbop

#endif //LIB_PBOP_STATUS