#include "pch.h"
#include "muStream.h"

namespace mu {

MemoryStreamBuf::MemoryStreamBuf()
{
    resize(default_bufsize);
}

void MemoryStreamBuf::reset()
{
    auto *p = buffer.data();
    auto *e = p + buffer.size();
    this->setp(p, p, e);
    this->setg(p, p, e);
}

void MemoryStreamBuf::resize(size_t n)
{
    buffer.resize(n);
    reset();
}

void MemoryStreamBuf::swap(RawVector<char>& buf)
{
    buffer.swap(buf);
    reset();
}

std::ios::pos_type MemoryStreamBuf::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode /*mode*/)
{
    auto *p = buffer.data();
    auto *e = p + buffer.size();
    if (dir == std::ios::beg)
        this->setg(p, buffer.data() + off, e);
    if (dir == std::ios::cur)
        this->setg(p, this->gptr() + off, e);
    if (dir == std::ios::end)
        this->setg(p, e - off, e);
    return uint64_t(this->gptr() - this->eback());
}

std::ios::pos_type MemoryStreamBuf::seekpos(pos_type pos, std::ios_base::openmode /*mode*/)
{
    auto *p = buffer.data();
    auto *e = p + buffer.size();
    this->setp(p, p + pos, e);
    return wcount;
}

int MemoryStreamBuf::overflow(int c)
{
    size_t pos = size_t(this->pptr() - this->pbase());

    // expand buffer and reset pptr
    resize(buffer.empty() ? default_bufsize : buffer.size() * 2);

    // recover pptr offset
    this->pbump((int)pos);

    // put overflowed 1 byte
    *this->pptr() = (char)c;
    this->pbump(1);

    return c;
}

int MemoryStreamBuf::underflow()
{
    return traits_type::eof();
}

int MemoryStreamBuf::sync()
{
    rcount = uint64_t(this->gptr() - this->eback());
    wcount = uint64_t(this->pptr() - this->pbase());
    if (wcount > 0)
        buffer.resize(wcount);
    return 0;
}

MemoryStream::MemoryStream() : std::iostream(&m_buf) {}
void MemoryStream::reset() { m_buf.reset(); }
void MemoryStream::resize(size_t n) { m_buf.resize(n); }
void MemoryStream::swap(RawVector<char>& buf) { m_buf.swap(buf); }
const RawVector<char>& MemoryStream::getBuffer() const { return m_buf.buffer; }
uint64_t MemoryStream::getWCount() const { return m_buf.wcount; }
uint64_t MemoryStream::getRCount() const { return m_buf.rcount; }


static RawVector<char> s_dummy_buf;

CounterStreamBuf::CounterStreamBuf()
{
    if (s_dummy_buf.empty())
        s_dummy_buf.resize(default_bufsize);
    this->setp(s_dummy_buf.data(), s_dummy_buf.data() + s_dummy_buf.size());
}

int CounterStreamBuf::overflow(int c)
{
    m_size += uint64_t(this->pptr() - this->pbase()) + 1;
    this->setp(s_dummy_buf.data(), s_dummy_buf.data() + s_dummy_buf.size());
    return c;
}

int CounterStreamBuf::sync()
{
    m_size += uint64_t(this->pptr() - this->pbase());
    this->setp(s_dummy_buf.data(), s_dummy_buf.data() + s_dummy_buf.size());
    return 0;
}

void CounterStreamBuf::reset()
{
    m_size = 0;
    this->setp(s_dummy_buf.data(), s_dummy_buf.data() + s_dummy_buf.size());
}

CounterStream::CounterStream() : std::ostream(&m_buf) {}
uint64_t CounterStream::size() const { return m_buf.m_size; }
void CounterStream::reset() { m_buf.reset(); }

} // namespace mu
