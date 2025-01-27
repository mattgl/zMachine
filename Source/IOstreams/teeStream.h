#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <functional>

class teeBuffer : public std::streambuf
{
public:
  teeBuffer();

  void addBuffer(std::streambuf* buf);
  virtual int overflow(int c);

private:
  std::vector<std::streambuf*>    m_buffers;
};

class teeStream : public std::ostream
{
public:
  teeStream();
  void tee(std::ostream& out);

private:
  teeBuffer m_buffer;
};
