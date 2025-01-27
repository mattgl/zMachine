#include "teeStream.h"

teeBuffer::teeBuffer() 

{
};

void teeBuffer::addBuffer(std::streambuf* buf)

{
  m_buffers.push_back(buf);
}

int teeBuffer::overflow(int c)

{
  std::for_each(m_buffers.begin(), m_buffers.end(), std::bind2nd(std::mem_fun(&std::streambuf::sputc), c));
  return c;
}

teeStream::teeStream(): std::ostream(NULL)

{
  std::ostream::rdbuf(&m_buffer);
}

void teeStream::tee(std::ostream& out)
{
  out.flush();
  m_buffer.addBuffer(out.rdbuf());
}
