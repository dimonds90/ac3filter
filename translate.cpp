#include "translate.h"

#pragma warning(disable: 4786) // the debugger cannot debug code with symbols longer than 255 characters

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

using namespace std;

void compress_str(string &str)
{
  // replace tabs to spaces and compress white space
  for (int i = 0, j = 0; i < str.length(); i++)
  {
    if (str[i] == ' ' || str[i] == '\t')
    {
      if (j && str[j-1] != ' ')
        str[j++] = ' ';
    }
    else
      str[j++] = str[i];
  }
  str.reserve(j);
}

class Translator
{
protected:
  typedef map<string, int> map_si;
  typedef map<int, string> map_is;

  map_si controls; // map identifier name => control id
  map_is trans;    // map control id => translation string

public:
  Translator()
  {};

  Translator(const char *_controls, const char *_trans)
  {
    add_controls(_controls);
    add_trans(_trans);
  };

  bool add_controls(const char *_filename)
  {
    ifstream f(_filename);
    if (!f.is_open())
      return false;

    string str;
    string define;
    string str_id;
    int    int_id;

    while (!f.eof())
    {
      // read file
      getline(f, str, '\n');
      compress_str(str);

      // read tokens
      int_id = 0;
      stringstream sstr(str);
      sstr >> define >> str_id >> int_id;
      if (define != "#define" || int_id == 0)
        continue;

      // update ids map
      controls[str_id] = int_id;
    }

    return true;
  };

  bool add_trans(const char *_filename)
  {
    ifstream f(_filename);
    if (!f.is_open())
      return false;

    string str;
    string strid;
    string strtrans;
    int    intid;

    while (!f.eof())
    {
      getline(f, str, '\n');

      // read tokens
      stringstream sstr(str);
      getline(sstr, strid, '=');
      getline(sstr, strtrans, '\n');

      // find control identifier by name
      compress_str(strid);
      map_si::iterator i = controls.find(strid);
      if (i == controls.end())
        continue;

      // update translation map
      intid = i->second;
      trans[intid] = strtrans;
      //cout << """" << strid << """=" << strtrans << "\n";
    }

    return true;
  }

  void translate_controls(HWND hwnd)
  {
    map_is::iterator i;
    int id;
    const char *str;
    for (i = trans.begin(); i != trans.end(); i++)
    {
      id = i->first;
      str = i->second.c_str();
      SetDlgItemText(hwnd, id, str);
    }
  }

  void translate_title(HWND hwnd, int title_id)
  {
    if (hwnd == 0 || title_id == 0) return;
    map_is::iterator i = trans.find(title_id);
    if (i == trans.end())
      return;

    const char *title = i->second.c_str();
    SetWindowText(hwnd, title);
  }

};

void translate(HWND hwnd, const char *ids, const char *trans, int window_id)
{
  Translator t(ids, trans);
  t.translate_controls(hwnd);
  t.translate_title(hwnd, window_id);
}
