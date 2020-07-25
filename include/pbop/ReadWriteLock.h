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

#ifndef LIB_PBOP_READ_WRITE_LOCK
#define LIB_PBOP_READ_WRITE_LOCK

namespace pbop
{

  /// <summary>
  /// Protect a section of code against concurrent access.
  /// This class is used in situations where there are multiple readers but few writers.
  /// It allows locking for read or write access.
  /// All readers do not block each others. Multiple readers can use the same sections of code.
  /// When a write lock is requested, the class blocks the process until all readers unlocks then
  /// the writer lock is granted. In other words, writer locks have priority over read locks.
  /// </summary>
  class ReadWriteLock
  {
  private:
    struct PImpl;
    PImpl * impl_;

  public:
    ReadWriteLock();
    ~ReadWriteLock();
  private:
    ReadWriteLock(const ReadWriteLock & copy); //disable copy constructor.
    ReadWriteLock & operator =(const ReadWriteLock & other); //disable assignment operator.
  public:

    /// <summary>
    /// Enter the reading critical section. Blocks other writing threads but not other reading thread until a UnlockRead() call is processed.
    /// </summary>
    void LockRead();

    /// <summary>
    /// Leave the reading critical section.
    /// </summary>
    void UnlockRead();

    /// <summary>
    /// Enter the writing critical section. Blocks other reading and writing threads until a UnlockWrite() call is processed.
    /// </summary>
    void LockWrite();

    /// <summary>
    /// Leave the writing critical section.
    /// </summary>
    void UnlockWrite();
  };

}; //namespace pbop

#endif //LIB_PBOP_READ_WRITE_LOCK