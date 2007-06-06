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
  DataBuf buf;

public:
  StreamDecoder();
  ~StreamDecoder();

  bool open(Speakers _in_spk, Speakers _out_spk);
  void close();
  void reset();

  bool decode(
    uint8_t *src, unsigned src_len, 
    uint8_t *dst, unsigned dst_len, 
    unsigned *src_gone, unsigned *dst_gone);
};

#endif
