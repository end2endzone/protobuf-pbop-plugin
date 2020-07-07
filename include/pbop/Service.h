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

#ifndef LIB_PBOP_SERVICE
#define LIB_PBOP_SERVICE

#include "pbop/Status.h"

#include <string>

namespace pbop
{

  class Service
  {
  private:
    Service(const Service & copy); //disable copy constructor.
    Service & operator =(const Service & other); //disable assignment operator.

  public:
    Service() {}
    virtual ~Service() {}

    /// <summary>
    /// Provides the package name of the service.
    /// </summary>
    /// <returns>Provides the package name of the service. Returns NULL if no package is specified.</returns>
    virtual const char * GetPackageName() const = 0;

    /// <summary>
    /// Provides the service name of the service.
    /// </summary>
    /// <returns>Provides the service name of the service.</returns>
    virtual const char * GetServiceName() const = 0;

    /// <summary>
    /// Provides an array of the service method names.
    /// The array contains `const char *` elements. The last element of the array is a NULL element.
    /// </summary>
    /// <returns>Returns an array of the service method names.</returns>
    virtual const char ** GetFunctionIdentifiers() const = 0;

    /// <summary>
    /// Invoke a function of the service and wait for the output serialized message.
    /// The function is identified with an index based on the functions array returned by GetFunctionIdentifiers().
    /// </summary>
    /// <param name="index">The index in GetFunctionIdentifiers() of the service method to process this message.</param>
    /// <param name="name">The serialized input message for the service method. The arguments of the function call.</param>
    /// <param name="name">The serialized output message of the service method. The return type of the function call.</param>
    /// <returns>Returns a Status instance which code is set to STATUS_CODE_SUCCESS when the operation is successful.</returns>
    virtual Status InvokeMethod(const size_t & index, const std::string & input, std::string & output) = 0;
  };

}; //namespace pbop

#endif //LIB_PBOP_SERVICE