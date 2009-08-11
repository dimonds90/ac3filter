#ifndef ROT_H
#define ROT_H

#include <streams.h>

class ROTEntry
{
protected:
  DWORD rot_id;

public:
  ROTEntry();
  ~ROTEntry();

  bool register_graph(IFilterGraph *graph);
  void unregister_graph();
};

#endif
