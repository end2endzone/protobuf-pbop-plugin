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

#ifndef LIB_PBOP_MUTEX
#define LIB_PBOP_MUTEX

namespace pbop
{

  /// <summary>
  /// Protect a section of code against concurrent access.
  /// </summary>
  class Mutex
  {
  private:
    struct PImpl;
    PImpl * impl_;

  public:
    Mutex();
    ~Mutex();
  private:
    Mutex(const Mutex & copy); //disable copy constructor.
    Mutex & operator =(const Mutex & other); //disable assignment operator.
  public:

    /// <summary>
    /// Acquire the mutex. Blocks other threads until a Unlock() call is processed.
    /// </summary>
    void Lock();

    /// <summary>
    /// Release the mutex.
    /// </summary>
    void Unlock();
  };

}; //namespace pbop

#endif //LIB_PBOP_MUTEX