/*
  Good log should be:
  * small enough for fast reporting
  * very detailed to help with debugging

  AC3Filter can make tens and hundreds megabytes trace logs. Of course, it is
  unacceptable for end user. But low-detailed logs may not help at all.\
  
  As a tradeoff TWO log files are produced:
  * low-detailed (log_event and lower).
  * high-detailed (not filtered), but limited in size.
*/

#include <list>
#include <deque>
#include <fstream>
#include <iomanip>

#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include <DbgHelp.h>

#include "../BugTrap/BugTrap.h"
#include "../ac3filter_ver.h"
#include "buffer.h"
#include "registry.h"
#include "error_reporting.h"
#include "guids.h"

static const size_t log_size = 1000; // entries per log
static const size_t audio_data_size = 200000; // cache size in bytes

LogMem event_log(log_size);
LogMem trace_log(log_size);

///////////////////////////////////////////////////////////////////////////////

class AudioLogImpl;
static std::list<AudioLogImpl *> streams;

class AudioLogImpl
{
protected:
  Speakers spk;

  // Cyclic data buffer
  Rawdata buf;
  size_t data_size;
  size_t pos;

  // Chunk log
  std::deque<Chunk> chunks;
  size_t chunk_data_size;

public:
  AudioLogImpl(Speakers stream_spk, size_t buf_size)
  {
    spk = stream_spk;

    buf.allocate(buf_size);
    data_size = 0;
    pos = 0;

    chunks.clear();
    chunk_data_size = 0;

    streams.push_back(this);
  }

  ~AudioLogImpl()
  {
    streams.remove(this);
  }

  void log(const Chunk &chunk)
  {
    if (chunk.size > buf.size())
    {
      pos = data_size = buf.size();
      memcpy(buf, chunk.rawdata, buf.size());
    }
    else if (pos + chunk.size >= buf.size())
    {
      size_t tail_size = buf.size() - pos;
      memcpy(buf + pos, chunk.rawdata, tail_size);
      memcpy(buf, chunk.rawdata + tail_size, chunk.size - tail_size);
      pos = chunk.size - tail_size;
      data_size = buf.size();
    }
    else
    {
      memcpy(buf + pos, chunk.rawdata, chunk.size);
      pos += chunk.size;
      data_size += chunk.size;
    }

    chunks.push_back(chunk);
    chunk_data_size += chunk.size;

    // Remove chunks gone from the buffer
    while (chunk_data_size > buf.size() + chunks[0].size)
    {
      chunk_data_size -= chunks[0].size;
      chunks.pop_front();
    }
  }

  void reset()
  {
    data_size = 0;
    pos = 0;
    chunks.clear();
    chunk_data_size = 0;
  }

  bool is_empty() const
  {
    return data_size == 0;
  }

  bool save_log(string log_file)
  {
    // write chunks log
    std::ofstream f(log_file.c_str(), std::ios_base::trunc);
    if (!f.is_open())
      return false;

    f << "Data format: " << spk.print() << nl;
    f << "File pos\tSize\tTimestamp\n";
    f << std::fixed << std::setprecision(3);

    int pos = (int)data_size - (int)chunk_data_size; // may be negative!
    for (size_t i = 0; i < chunks.size(); i++)
    {
      f << pos << "\t" << chunks[i].size;
      pos += (int)chunks[i].size;
      if (chunks[i].sync)
        f << "\t" << chunks[i].time;
      f << nl;
    }
    return true;
  }

  bool save_data(string data_file)
  {
    AutoFile f(data_file.c_str(), "wb");
    if (!f.is_open())
      return false;

    if (data_size == pos)
      f.write(buf, data_size);
    else
    {
      f.write(buf + pos, buf.size() - pos);
      f.write(buf, pos);
    }
    return true;
  }
};

class AudioLog::Impl : public AudioLogImpl
{
public:
  AudioLog::Impl(Speakers stream_spk, size_t buf_size):
  AudioLogImpl(stream_spk, buf_size)
  {}
};

AudioLog::AudioLog(): pimpl(0)
{}

AudioLog::~AudioLog()
{
  safe_delete(pimpl);
}

bool
AudioLog::open(Speakers spk)
{
  pimpl = new AudioLog::Impl(spk, audio_data_size);
  return true;
}

void
AudioLog::close()
{
  safe_delete(pimpl);
}

bool
AudioLog::is_open() const
{
  return pimpl != 0;
}

void
AudioLog::log(const Chunk &chunk)
{
  if (pimpl) pimpl->log(chunk);
}

void
AudioLog::reset()
{
  if (pimpl) pimpl->reset();
}

///////////////////////////////////////////////////////////////////////////////

static std::string temp_path()
{
  static std::string temp_str;
  if (temp_str.empty())
  {
    TCHAR temp[MAX_PATH];
    GetTempPath(MAX_PATH, temp);
    temp_str = std::string(temp);
  }
  return temp_str;
}

static std::string event_log_file_name()
{
  TCHAR file_name[MAX_PATH];
  GetTempPath(MAX_PATH, file_name);
  PathAppend(file_name, "ac3filter_event.log");
  return std::string(file_name);
}

static std::string trace_log_file_name()
{
  TCHAR file_name[MAX_PATH];
  GetTempPath(MAX_PATH, file_name);
  PathAppend(file_name, "ac3filter_trace.log");
  return std::string(file_name);
}

static void flush_log(LogMem &log, const string &file_name)
{
  static const string endl = "\n";
  AutoFile f(file_name.c_str(), "w");
  for (size_t i = 0; i < log.size(); i++)
  {
    string s = log[i].print() + endl;
    f.write(s.c_str(), s.size());
  }
}

static void CALLBACK pre_error(INT_PTR param)
{
  flush_log(event_log, event_log_file_name());
  flush_log(trace_log, trace_log_file_name());

  RegistryKey reg(REG_KEY);
  bool send_audio_data = false;
  reg.get_bool("send_audio_data", send_audio_data);
  if (send_audio_data)
  {
    char filename[MAX_PATH];
    int stream = 0;
    for (std::list<AudioLogImpl *>::iterator i = streams.begin(); i != streams.end(); i++)
      if (!(*i)->is_empty())
      {
        sprintf(filename, "%s\\ac3filter_%i.log", temp_path().c_str(), stream);
        (*i)->save_log(filename);
        BT_AddLogFile(filename);

        sprintf(filename, "%s\\ac3filter_%i.raw", temp_path().c_str(), stream);
        (*i)->save_data(filename);
        BT_AddLogFile(filename);

        stream++;
      }
  }
}

void init_logging()
{
  event_log.subscribe(&valib_log_dispatcher);
  trace_log.subscribe(&valib_log_dispatcher);
  event_log.set_max_log_level(log_event);
  trace_log.set_max_log_level(log_all);

  // Init BugTrap
  BT_SetAppName(_T(APP_NAME));
  BT_SetAppVersion(_T(AC3FILTER_VER));
  BT_SetSupportEMail(_T(SUPPORT_EMAIL));
  BT_SetFlags(BTF_DETAILEDMODE | BTF_EDITMAIL);
  BT_SetDumpType(MiniDumpNormal);
  BT_SetSupportServer(_T(BUG_TRAP_URL), 80);
  BT_SetSupportURL(_T(WEB_SITE_URL));
  BT_AddRegFile(_T("Settings.reg"), _T("HKEY_CURRENT_USER\\"REG_KEY));
  BT_AddLogFile(event_log_file_name().c_str());
  BT_AddLogFile(trace_log_file_name().c_str());
  BT_SetPreErrHandler(pre_error, 0);
  BT_SetModule(ac3filter_instance);

  BT_InstallSehFilter();
}

void uninit_logging()
{
  BT_UninstallSehFilter();

  bool save_logs = false;
  RegistryKey reg(REG_KEY);
  reg.get_bool("save_logs", save_logs);
  if (save_logs)
  {
    flush_log(event_log, event_log_file_name());
    flush_log(trace_log, trace_log_file_name());
  }
}
