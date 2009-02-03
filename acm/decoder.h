#ifndef AC3FILTER_ACM_DECODER_H
#define AC3FILTER_ACM_DECODER_H

#include "filters\decoder_graph.h"

class StreamDecoder
{
protected:
  Speakers in_spk;
  Speakers out_spk;

  bool is_open;
  Chunk chunk;
  DecoderGraph dec;
  Rawdata buf;

public:
  StreamDecoder();
  ~StreamDecoder();

  bool open(Speakers _in_spk, Speakers _out_spk);
  void close();
  void reset();

  bool decode(
    uint8_t *src, size_t src_len, 
    uint8_t *dst, size_t dst_len, 
    size_t *src_gone, size_t *dst_gone);
};

#endif
