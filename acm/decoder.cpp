#include "decoder.h"
#include "dbglog.h"

#define BUFFER_SIZE 4096

StreamDecoder::StreamDecoder()
{
  is_open = false;
}

StreamDecoder::~StreamDecoder()
{
  if (is_open) close();
}

bool
StreamDecoder::open(Speakers _in_spk, Speakers _out_spk)
{
  if (is_open) close();

  if (!dec.open(_in_spk) || !dec.proc.set_user(_out_spk))
    return false;

  in_spk = _in_spk; 
  out_spk = _out_spk;

  dec.proc.set_input_order(std_order);
  dec.proc.set_output_order(win_order);
  dec.reset();

  buf.allocate(BUFFER_SIZE);

  is_open = true;
  return true;
}

void
StreamDecoder::close()
{
  is_open = false;
}
 
void
StreamDecoder::reset()
{
  chunk.clear();
  dec.reset();
}

bool 
StreamDecoder::decode(
  uint8_t *src, size_t src_len,
  uint8_t *dst, size_t dst_len,
  size_t *src_gone, size_t *dst_gone)
{
  if ((!src && src_len) || (!dst && dst_len) || !src_gone || !dst_gone)
  {
    dbglog("StreamDecoder::decode(): null parameter");
    return false;
  }

  *src_gone = 0;
  *dst_gone = 0;
  bool process = true;
  Chunk src_chunk(src, src_len);

  try
  {
    while (dst_len && process)
      if (!chunk.is_empty())
      {
        size_t size = MIN(chunk.size, dst_len);
        memcpy(dst, chunk.rawdata, size);
        chunk.drop_rawdata(size);
        dst += size;
        dst_len -= size;
        *dst_gone += size;
      }
      else
      {
        process = dec.process(src_chunk, chunk);
        if (!process)
          chunk.clear();
      }
  }
  catch (ValibException &e)
  {
    dbglog(boost::diagnostic_information(e).c_str());
    reset();
  }

  *src_gone = src_chunk.size? src_chunk.rawdata - src: src_len;
  return true;
}
