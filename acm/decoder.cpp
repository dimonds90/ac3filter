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

  if (!dec.set_input(_in_spk) || !dec.set_user(_out_spk))
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
  chunk.set_empty(out_spk);
  dec.reset();
}

bool 
StreamDecoder::decode(
  uint8_t *src, size_t src_len,
  uint8_t *dst, size_t dst_len,
  size_t *src_gone, size_t *dst_gone)
{
  size_t size;
  while (src_len && dst_len)
    if (dec.is_empty() && chunk.is_empty())
    {
      size = MIN(buf.size(), src_len);
      memcpy(buf.data(), src, size);
      chunk.set_rawdata(in_spk, buf.data(), size);
      if (!dec.process(&chunk))
      {
        dbglog("error in process()");
        reset();
        return false;
      }

      chunk.set_empty(out_spk);
      src += size;
      src_len -= size;
      *src_gone += size;
      continue;
    }
    else if (chunk.is_empty())
    {
      if (!dec.get_chunk(&chunk))
      {
        dbglog("error in get_chunk()");
        reset();
        return false;
      }
      continue;
    }
    else
    {
      size = MIN(chunk.size, dst_len);
      memcpy(dst, chunk.rawdata, size);
      chunk.drop(size);
      dst += size;
      dst_len -= size;
      *dst_gone += size;
      continue;
    }
  // while (src_len && dst_len)
  return true;
}
