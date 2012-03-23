#include "rot.h"

ROTEntry::ROTEntry()
{
  rot_id = 0;
}

ROTEntry::~ROTEntry()
{
  if (rot_id)
    register_graph(0);
}


bool
ROTEntry::register_graph(IFilterGraph *graph)
{
  // get running objects table
  IRunningObjectTable *rot;
  if FAILED(GetRunningObjectTable(0, &rot)) 
    return false;

  // unregister old graph if registred
  if (rot_id) 
  {
    rot->Revoke(rot_id);
    rot_id = 0;
  }

  if (!graph)
  {
    rot->Release();
    return true;
  }

  // register graph
  WCHAR str[256];
  IMoniker *moniker;

  wsprintfW(str, L"FilterGraph %08x pid %08x", (DWORD_PTR)graph, GetCurrentProcessId());
  if SUCCEEDED(CreateItemMoniker(L"!", str, &moniker)) 
  {
    if FAILED(rot->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, (IUnknown *)graph, moniker, &rot_id))
      rot_id = 0;
    moniker->Release();
  }
  rot->Release();

  return rot_id != 0;
}

void
ROTEntry::unregister_graph()
{
  register_graph(0);
}
