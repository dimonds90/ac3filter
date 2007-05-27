#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "ac3filter_intl.h"
/*
LangCat::LangCat()
{
}

LangCat::LangCat(const char *path)
{
  set_path(path);
}

LangCat::~LangCat()
{
  reset();
}

int
LangCat::set_path(const char *path)
{
  char file[MAX_PATH];
  WIN32_FIND_DATA fd;
  HANDLE fh;
  int count;

  reset();

  if (!path) return 0;
  if (path[0] == 0) return 0;

  count = 0;
  sprintf(file, "%s\\*", path);
  fh = FindFirstFile(file, &fd);
  if (fh != INVALID_HANDLE_VALUE)
    do
    {
      if (GetFileAttributes(fd.cFileName) && FILE_ATTRIBUTE_DIRECTORY)
      {
        if (lang_from_iso(fd.cFileName))
          count++;
      }
    } while (FindNextFile(fh, &fd));

  langs = new char[MAX_LANG * count];
  if (!langs) return 0;
  memset(langs, 0, sizeof(const char *) * count);
}
*/
// Global language code
// Only 2 or 3-character codes allowed
static const int lang_size = 4;
static char lang[lang_size] = "\0";
extern "C" int _nl_msg_cat_cntr;

void set_lang(const char *code, const char *package, const char *path)
{
#ifdef ENABLE_NLS
  if (package)
  {
    if (path)
      bindtextdomain(package, path);
    textdomain(package);
  }

  if (code)
  {
    if (code[0] == 0) // set_lang("") sets to english
    {
      char lang_env[256];
      sprintf(lang_env, "LANGUAGE=en", lang);
      putenv(lang_env);
      ++_nl_msg_cat_cntr;
    }
    else if (lang_from_iso_code(code)) // verify the language code provided
    {
      strncpy(lang, code, lang_size);
      lang[lang_size-1] = 0;
      char lang_env[256];
      sprintf(lang_env, "LANGUAGE=%s", lang);
      putenv(lang_env);
      ++_nl_msg_cat_cntr;
    }
  }
#endif
}

const char *get_lang()
{
  if (lang[0] == 0) { return 0; }
  return lang;
}


int find_iso6392(const char *_code)
{
  char code[3];
  if (_code == 0) return -1;
  if (_code[0] == 0) return -1;
  if (_code[1] == 0) return -1;
  if (_code[2] == 0) return -1;
  if (_code[3] != 0) return -1;
  code[0] = (char) tolower(_code[0]);
  code[1] = (char) tolower(_code[1]);
  code[2] = (char) tolower(_code[2]);

  int i = 0;
  while (iso_langs[i].name)
  {
    if (iso_langs[i].iso6392)
      if (code[0] == iso_langs[i].iso6392[0] && 
          code[1] == iso_langs[i].iso6392[1] &&
          code[2] == iso_langs[i].iso6392[2])
        return i;
    i++;
  }
  return -1;
}

int find_iso6391(const char *_code)
{
  char code[2];
  if (_code == 0) return -1;
  if (_code[0] == 0) return -1;
  if (_code[1] == 0) return -1;
  if (_code[2] != 0) return -1;
  code[0] = (char) tolower(_code[0]);
  code[1] = (char) tolower(_code[1]);

  int i = 0;
  while (iso_langs[i].name)
  {
    if (iso_langs[i].iso6391)
      if (code[0] == iso_langs[i].iso6391[0] && 
          code[1] == iso_langs[i].iso6391[1])
        return i;
    i++;
  }
  return -1;
}

int find_iso_code(const char *_code)
{
  if (_code == 0) return 0;
  if (_code[0] == 0) return 0;
  if (_code[1] == 0) return 0;
  if (_code[2] == 0) return find_iso6391(_code);
  if (_code[3] == 0) return find_iso6392(_code);
  return 0;
}

const char *lang_from_iso6392(const char *_code)
{
  int i = find_iso6392(_code);
  return i == -1? 0: iso_langs[i].name;
}

const char *lang_from_iso6391(const char *_code)
{
  int i = find_iso6391(_code);
  return i == -1? 0: iso_langs[i].name;
}

const char *lang_from_iso_code(const char *_code)
{
  int i = find_iso_code(_code);
  return i == -1? 0: iso_langs[i].name;
}

const iso_lang_s iso_langs[] = 
{
  { "English", "eng", "en" }, 

  { "Abkhazian", "abk", "ab" }, 
  { "Achinese", "ace", 0 }, 
  { "Acoli", "ach", 0 }, 
  { "Adangme", "ada", 0 }, 
  { "Adygei", "ady", 0 }, 
  { "Adyghe", "ady", 0 }, 
  { "Afar", "aar", "aa" }, 
  { "Afrihili", "afh", 0 }, 
  { "Afrikaans", "afr", "af" }, 
  { "Afro-Asiatic (Other)", "afa", 0 }, 
  { "Ainu", "ain", 0 }, 
  { "Akan", "aka", "ak" }, 
  { "Akkadian", "akk", 0 }, 
  { "Albanian", "alb", "sq" }, 
  { "Albanian", "sqi", "sq" }, 
  { "Alemannic", "gsw", 0 }, 
  { "Aleut", "ale", 0 }, 
  { "Algonquian languages", "alg", 0 }, 
  { "Altaic (Other)", "tut", 0 }, 
  { "Amharic", "amh", "am" }, 
  { "Angika", "anp", 0 }, 
  { "Apache languages", "apa", 0 }, 
  { "Arabic", "ara", "ar" }, 
  { "Aragonese", "arg", "an" }, 
  { "Aramaic", "arc", 0 }, 
  { "Arapaho", "arp", 0 }, 
  { "Arawak", "arw", 0 }, 
  { "Armenian", "arm", "hy" }, 
  { "Armenian", "hye", "hy" }, 
  { "Aromanian", "rup", 0 }, 
  { "Artificial (Other)", "art", 0 }, 
  { "Arumanian", "rup", 0 }, 
  { "Assamese", "asm", "as" }, 
  { "Asturian", "ast", 0 }, 
  { "Athapascan languages", "ath", 0 }, 
  { "Australian languages", "aus", 0 }, 
  { "Austronesian (Other)", "map", 0 }, 
  { "Avaric", "ava", "av" }, 
  { "Avestan", "ave", "ae" }, 
  { "Awadhi", "awa", 0 }, 
  { "Aymara", "aym", "ay" }, 
  { "Azerbaijani", "aze", "az" }, 
  { "Bable", "ast", 0 }, 
  { "Balinese", "ban", 0 }, 
  { "Baltic (Other)", "bat", 0 }, 
  { "Baluchi", "bal", 0 }, 
  { "Bambara", "bam", "bm" }, 
  { "Bamileke languages", "bai", 0 }, 
  { "Banda languages", "bad", 0 }, 
  { "Bantu (Other)", "bnt", 0 }, 
  { "Basa", "bas", 0 }, 
  { "Bashkir", "bak", "ba" }, 
  { "Basque", "baq", "eu" }, 
  { "Basque", "eus", "eu" }, 
  { "Batak languages", "btk", 0 }, 
  { "Beja", "bej", 0 }, 
  { "Belarusian", "bel", "be" }, 
  { "Bemba", "bem", 0 }, 
  { "Bengali", "ben", "bn" }, 
  { "Berber (Other)", "ber", 0 }, 
  { "Bhojpuri", "bho", 0 }, 
  { "Bihari", "bih", "bh" }, 
  { "Bikol", "bik", 0 }, 
  { "Bilin", "byn", 0 }, 
  { "Bini", "bin", 0 }, 
  { "Bislama", "bis", "bi" }, 
  { "Blin", "byn", 0 }, 
  { "Bokmal, Norwegian", "nob", "nb" }, 
  { "Bosnian", "bos", "bs" }, 
  { "Braj", "bra", 0 }, 
  { "Breton", "bre", "br" }, 
  { "Buginese", "bug", 0 }, 
  { "Bulgarian", "bul", "bg" }, 
  { "Buriat", "bua", 0 }, 
  { "Burmese", "bur", "my" }, 
  { "Burmese", "mya", "my" }, 
  { "Caddo", "cad", 0 }, 
  { "Castilian", "spa", "es" }, 
  { "Catalan", "cat", "ca" }, 
  { "Caucasian (Other)", "cau", 0 }, 
  { "Cebuano", "ceb", 0 }, 
  { "Celtic (Other)", "cel", 0 }, 
  { "Central American Indian (Other)", "cai", 0 }, 
  { "Central Khmer", "khm", "km" }, 
  { "Chagatai", "chg", 0 }, 
  { "Chamic languages", "cmc", 0 }, 
  { "Chamorro", "cha", "ch" }, 
  { "Chechen", "che", "ce" }, 
  { "Cherokee", "chr", 0 }, 
  { "Chewa", "nya", "ny" }, 
  { "Cheyenne", "chy", 0 }, 
  { "Chibcha", "chb", 0 }, 
  { "Chichewa", "nya", "ny" }, 
  { "Chinese", "chi", "zh" }, 
  { "Chinese", "zho", "zh" }, 
  { "Chinook jargon", "chn", 0 }, 
  { "Chipewyan", "chp", 0 }, 
  { "Choctaw", "cho", 0 }, 
  { "Chuang", "zha", "za" }, 
  { "Church Slavic", "chu", "cu" }, 
  { "Church Slavonic", "chu", "cu" }, 
  { "Chuukese", "chk", 0 }, 
  { "Chuvash", "chv", "cv" }, 
  { "Classical Nepal Bhasa", "nwc", 0 }, 
  { "Classical Newari", "nwc", 0 }, 
  { "Classical Syriac", "syc", 0 }, 
  { "Cook Islands Maori", "rar", 0 }, 
  { "Coptic", "cop", 0 }, 
  { "Cornish", "cor", "kw" }, 
  { "Corsican", "cos", "co" }, 
  { "Cree", "cre", "cr" }, 
  { "Creek", "mus", 0 }, 
  { "Creoles and pidgins (Other)", "crp", 0 }, 
  { "Creoles and pidgins, English based (Other)", "cpe", 0 }, 
  { "Creoles and pidgins, French-based (Other)", "cpf", 0 }, 
  { "Creoles and pidgins, Portuguese-based (Other)", "cpp", 0 }, 
  { "Crimean Tatar", "crh", 0 }, 
  { "Crimean Turkish", "crh", 0 }, 
  { "Croatian", "scr", "hr" }, 
  { "Croatian", "hrv", "hr" }, 
  { "Cushitic (Other)", "cus", 0 }, 
  { "Czech", "cze", "cs" }, 
  { "Czech", "ces", "cs" }, 
  { "Dakota", "dak", 0 }, 
  { "Danish", "dan", "da" }, 
  { "Dargwa", "dar", 0 }, 
  { "Delaware", "del", 0 }, 
  { "Dhivehi", "div", "dv" }, 
  { "Dimili", "zza", 0 }, 
  { "Dimli", "zza", 0 }, 
  { "Dinka", "din", 0 }, 
  { "Divehi", "div", "dv" }, 
  { "Dogri", "doi", 0 }, 
  { "Dogrib", "dgr", 0 }, 
  { "Dravidian (Other)", "dra", 0 }, 
  { "Duala", "dua", 0 }, 
  { "Dutch", "dut", "nl" }, 
  { "Dutch", "nld", "nl" }, 
  { "Dutch, Middle (ca.1050-1350)", "dum", 0 }, 
  { "Dyula", "dyu", 0 }, 
  { "Dzongkha", "dzo", "dz" }, 
  { "Eastern Frisian", "frs", 0 }, 
  { "Edo", "bin", 0 }, 
  { "Efik", "efi", 0 }, 
  { "Egyptian (Ancient)", "egy", 0 }, 
  { "Ekajuk", "eka", 0 }, 
  { "Elamite", "elx", 0 }, 
  { "English, Middle (1100-1500)", "enm", 0 }, 
  { "English, Old (ca.450-1100)", "ang", 0 }, 
  { "Erzya", "myv", 0 }, 
  { "Esperanto", "epo", "eo" }, 
  { "Estonian", "est", "et" }, 
  { "Ewe", "ewe", "ee" }, 
  { "Ewondo", "ewo", 0 }, 
  { "Fang", "fan", 0 }, 
  { "Fanti", "fat", 0 }, 
  { "Faroese", "fao", "fo" }, 
  { "Fijian", "fij", "fj" }, 
  { "Filipino", "fil", 0 }, 
  { "Finnish", "fin", "fi" }, 
  { "Finno-Ugrian (Other)", "fiu", 0 }, 
  { "Flemish", "dut", "nl" }, 
  { "Flemish", "nld", "nl" }, 
  { "Fon", "fon", 0 }, 
  { "French", "fre", "fr" }, 
  { "French", "fra", "fr" }, 
  { "French, Middle (ca.1400-1600)", "frm", 0 }, 
  { "French, Old (842-ca.1400)", "fro", 0 }, 
  { "Friulian", "fur", 0 }, 
  { "Fulah", "ful", "ff" }, 
  { "Ga", "gaa", 0 }, 
  { "Gaelic", "gla", "gd" }, 
  { "Galibi Carib", "car", 0 }, 
  { "Galician", "glg", "gl" }, 
  { "Ganda", "lug", "lg" }, 
  { "Gayo", "gay", 0 }, 
  { "Gbaya", "gba", 0 }, 
  { "Geez", "gez", 0 }, 
  { "Georgian", "geo", "ka" }, 
  { "Georgian", "kat", "ka" }, 
  { "German", "ger", "de" }, 
  { "German", "deu", "de" }, 
  { "German, Low", "nds", 0 }, 
  { "German, Middle High (ca.1050-1500)", "gmh", 0 }, 
  { "German, Old High (ca.750-1050)", "goh", 0 }, 
  { "Germanic (Other)", "gem", 0 }, 
  { "Gikuyu", "kik", "ki" }, 
  { "Gilbertese", "gil", 0 }, 
  { "Gondi", "gon", 0 }, 
  { "Gorontalo", "gor", 0 }, 
  { "Gothic", "got", 0 }, 
  { "Grebo", "grb", 0 }, 
  { "Greek, Ancient (to 1453)", "grc", 0 }, 
  { "Greek, Modern (1453-)", "gre", "el" }, 
  { "Greek, Modern (1453-)", "ell", "el" }, 
  { "Greenlandic", "kal", "kl" }, 
  { "Guarani", "grn", "gn" }, 
  { "Gujarati", "guj", "gu" }, 
  { "Gwich'in", "gwi", 0 }, 
  { "Haida", "hai", 0 }, 
  { "Haitian", "hat", "ht" }, 
  { "Haitian Creole", "hat", "ht" }, 
  { "Hausa", "hau", "ha" }, 
  { "Hawaiian", "haw", 0 }, 
  { "Hebrew", "heb", "he" }, 
  { "Herero", "her", "hz" }, 
  { "Hiligaynon", "hil", 0 }, 
  { "Himachali", "him", 0 }, 
  { "Hindi", "hin", "hi" }, 
  { "Hiri Motu", "hmo", "ho" }, 
  { "Hittite", "hit", 0 }, 
  { "Hmong", "hmn", 0 }, 
  { "Hungarian", "hun", "hu" }, 
  { "Hupa", "hup", 0 }, 
  { "Iban", "iba", 0 }, 
  { "Icelandic", "ice", "is" }, 
  { "Icelandic", "isl", "is" }, 
  { "Ido", "ido", "io" }, 
  { "Igbo", "ibo", "ig" }, 
  { "Ijo languages", "ijo", 0 }, 
  { "Iloko", "ilo", 0 }, 
  { "Inari Sami", "smn", 0 }, 
  { "Indic (Other)", "inc", 0 }, 
  { "Indo-European (Other)", "ine", 0 }, 
  { "Indonesian", "ind", "id" }, 
  { "Ingush", "inh", 0 }, 
  { "Interlingua (International Auxiliary Language Association)", "ina", "ia" }, 
  { "Interlingue", "ile", "ie" }, 
  { "Inuktitut", "iku", "iu" }, 
  { "Inupiaq", "ipk", "ik" }, 
  { "Iranian (Other)", "ira", 0 }, 
  { "Irish", "gle", "ga" }, 
  { "Irish, Middle (900-1200)", "mga", 0 }, 
  { "Irish, Old (to 900)", "sga", 0 }, 
  { "Iroquoian languages", "iro", 0 }, 
  { "Italian", "ita", "it" }, 
  { "Japanese", "jpn", "ja" }, 
  { "Javanese", "jav", "jv" }, 
  { "Jingpho", "kac", 0 }, 
  { "Judeo-Arabic", "jrb", 0 }, 
  { "Judeo-Persian", "jpr", 0 }, 
  { "Kabardian", "kbd", 0 }, 
  { "Kabyle", "kab", 0 }, 
  { "Kachin", "kac", 0 }, 
  { "Kalaallisut", "kal", "kl" }, 
  { "Kalmyk", "xal", 0 }, 
  { "Kamba", "kam", 0 }, 
  { "Kannada", "kan", "kn" }, 
  { "Kanuri", "kau", "kr" }, 
  { "Kara-Kalpak", "kaa", 0 }, 
  { "Karachay-Balkar", "krc", 0 }, 
  { "Karelian", "krl", 0 }, 
  { "Karen languages", "kar", 0 }, 
  { "Kashmiri", "kas", "ks" }, 
  { "Kashubian", "csb", 0 }, 
  { "Kawi", "kaw", 0 }, 
  { "Kazakh", "kaz", "kk" }, 
  { "Khasi", "kha", 0 }, 
  { "Khoisan (Other)", "khi", 0 }, 
  { "Khotanese", "kho", 0 }, 
  { "Kikuyu", "kik", "ki" }, 
  { "Kimbundu", "kmb", 0 }, 
  { "Kinyarwanda", "kin", "rw" }, 
  { "Kirdki", "zza", 0 }, 
  { "Kirghiz", "kir", "ky" }, 
  { "Kirmanjki", "zza", 0 }, 
  { "Klingon", "tlh", 0 }, 
  { "Komi", "kom", "kv" }, 
  { "Kongo", "kon", "kg" }, 
  { "Konkani", "kok", 0 }, 
  { "Korean", "kor", "ko" }, 
  { "Kosraean", "kos", 0 }, 
  { "Kpelle", "kpe", 0 }, 
  { "Kru languages", "kro", 0 }, 
  { "Kuanyama", "kua", "kj" }, 
  { "Kumyk", "kum", 0 }, 
  { "Kurdish", "kur", "ku" }, 
  { "Kurukh", "kru", 0 }, 
  { "Kutenai", "kut", 0 }, 
  { "Kwanyama", "kua", "kj" }, 
  { "Kyrgyz", "kir", "ky" }, 
  { "Ladino", "lad", 0 }, 
  { "Lahnda", "lah", 0 }, 
  { "Lamba", "lam", 0 }, 
  { "Land Dayak languages", "day", 0 }, 
  { "Lao", "lao", "lo" }, 
  { "Latin", "lat", "la" }, 
  { "Latvian", "lav", "lv" }, 
  { "Letzeburgesch", "ltz", "lb" }, 
  { "Lezghian", "lez", 0 }, 
  { "Limburgan", "lim", "li" }, 
  { "Limburger", "lim", "li" }, 
  { "Limburgish", "lim", "li" }, 
  { "Lingala", "lin", "ln" }, 
  { "Lithuanian", "lit", "lt" }, 
  { "Lojban", "jbo", 0 }, 
  { "Low German", "nds", 0 }, 
  { "Low Saxon", "nds", 0 }, 
  { "Lower Sorbian", "dsb", 0 }, 
  { "Lozi", "loz", 0 }, 
  { "Luba-Katanga", "lub", "lu" }, 
  { "Luba-Lulua", "lua", 0 }, 
  { "Luiseno", "lui", 0 }, 
  { "Lule Sami", "smj", 0 }, 
  { "Lunda", "lun", 0 }, 
  { "Luo (Kenya and Tanzania)", "luo", 0 }, 
  { "Lushai", "lus", 0 }, 
  { "Luxembourgish", "ltz", "lb" }, 
  { "Macedo-Romanian", "rup", 0 }, 
  { "Macedonian", "mac", "mk" }, 
  { "Macedonian", "mkd", "mk" }, 
  { "Madurese", "mad", 0 }, 
  { "Magahi", "mag", 0 }, 
  { "Maithili", "mai", 0 }, 
  { "Makasar", "mak", 0 }, 
  { "Malagasy", "mlg", "mg" }, 
  { "Malay", "may", "ms" }, 
  { "Malay", "msa", "ms" }, 
  { "Malayalam", "mal", "ml" }, 
  { "Maldivian", "div", "dv" }, 
  { "Maltese", "mlt", "mt" }, 
  { "Manchu", "mnc", 0 }, 
  { "Mandar", "mdr", 0 }, 
  { "Mandingo", "man", 0 }, 
  { "Manipuri", "mni", 0 }, 
  { "Manobo languages", "mno", 0 }, 
  { "Manx", "glv", "gv" }, 
  { "Maori", "mao", "mi" }, 
  { "Maori", "mri", "mi" }, 
  { "Mapuche", "arn", 0 }, 
  { "Mapudungun", "arn", 0 }, 
  { "Marathi", "mar", "mr" }, 
  { "Mari", "chm", 0 }, 
  { "Marshallese", "mah", "mh" }, 
  { "Marwari", "mwr", 0 }, 
  { "Masai", "mas", 0 }, 
  { "Mayan languages", "myn", 0 }, 
  { "Mende", "men", 0 }, 
  { "Mi'kmaq", "mic", 0 }, 
  { "Micmac", "mic", 0 }, 
  { "Minangkabau", "min", 0 }, 
  { "Mirandese", "mwl", 0 }, 
  { "Miscellaneous languages", "mis", 0 }, 
  { "Mohawk", "moh", 0 }, 
  { "Moksha", "mdf", 0 }, 
  { "Moldavian", "mol", "mo" }, 
  { "Mon-Khmer (Other)", "mkh", 0 }, 
  { "Mongo", "lol", 0 }, 
  { "Mongolian", "mon", "mn" }, 
  { "Mossi", "mos", 0 }, 
  { "Multiple languages", "mul", 0 }, 
  { "Munda languages", "mun", 0 }, 
  { "N'Ko", "nqo", 0 }, 
  { "Nahuatl languages", "nah", 0 }, 
  { "Nauru", "nau", "na" }, 
  { "Navaho", "nav", "nv" }, 
  { "Navajo", "nav", "nv" }, 
  { "Ndebele, North", "nde", "nd" }, 
  { "Ndebele, South", "nbl", "nr" }, 
  { "Ndonga", "ndo", "ng" }, 
  { "Neapolitan", "nap", 0 }, 
  { "Nepal Bhasa", "new", 0 }, 
  { "Nepali", "nep", "ne" }, 
  { "Newari", "new", 0 }, 
  { "Nias", "nia", 0 }, 
  { "Niger-Kordofanian (Other)", "nic", 0 }, 
  { "Nilo-Saharan (Other)", "ssa", 0 }, 
  { "Niuean", "niu", 0 }, 
  { "No linguistic content", "zxx", 0 }, 
  { "Nogai", "nog", 0 }, 
  { "Norse, Old", "non", 0 }, 
  { "North American Indian", "nai", 0 }, 
  { "North Ndebele", "nde", "nd" }, 
  { "Northern Frisian", "frr", 0 }, 
  { "Northern Sami", "sme", "se" }, 
  { "Northern Sotho", "nso", 0 }, 
  { "Norwegian", "nor", "no" }, 
  { "Norwegian Bokmal", "nob", "nb" }, 
  { "Norwegian Nynorsk", "nno", "nn" }, 
  { "Nubian languages", "nub", 0 }, 
  { "Nyamwezi", "nym", 0 }, 
  { "Nyanja", "nya", "ny" }, 
  { "Nyankole", "nyn", 0 }, 
  { "Nynorsk, Norwegian", "nno", "nn" }, 
  { "Nyoro", "nyo", 0 }, 
  { "Nzima", "nzi", 0 }, 
  { "Occitan (post 1500)", "oci", "oc" }, 
  { "Oirat", "xal", 0 }, 
  { "Ojibwa", "oji", "oj" }, 
  { "Old Bulgarian", "chu", "cu" }, 
  { "Old Church Slavonic", "chu", "cu" }, 
  { "Old Newari", "nwc", 0 }, 
  { "Old Slavonic", "chu", "cu" }, 
  { "Oriya", "ori", "or" }, 
  { "Oromo", "orm", "om" }, 
  { "Osage", "osa", 0 }, 
  { "Ossetian", "oss", "os" }, 
  { "Ossetic", "oss", "os" }, 
  { "Otomian languages", "oto", 0 }, 
  { "Pahlavi", "pal", 0 }, 
  { "Palauan", "pau", 0 }, 
  { "Pali", "pli", "pi" }, 
  { "Pampanga", "pam", 0 }, 
  { "Pangasinan", "pag", 0 }, 
  { "Panjabi", "pan", "pa" }, 
  { "Papiamento", "pap", 0 }, 
  { "Papuan (Other)", "paa", 0 }, 
  { "Pedi", "nso", 0 }, 
  { "Persian", "per", "fa" }, 
  { "Persian", "fas", "fa" }, 
  { "Persian, Old (ca.600-400 B.C.)", "peo", 0 }, 
  { "Philippine (Other)", "phi", 0 }, 
  { "Phoenician", "phn", 0 }, 
  { "Pilipino", "fil", 0 }, 
  { "Pohnpeian", "pon", 0 }, 
  { "Polish", "pol", "pl" }, 
  { "Portuguese", "por", "pt" }, 
  { "Prakrit languages", "pra", 0 }, 
  { "Provencal", "oci", "oc" }, 
  { "Provencal, Old (to 1500)", "pro", 0 }, 
  { "Punjabi", "pan", "pa" }, 
  { "Pushto", "pus", "ps" }, 
  { "Quechua", "que", "qu" }, 
  { "Rajasthani", "raj", 0 }, 
  { "Rapanui", "rap", 0 }, 
  { "Rarotongan", "rar", 0 }, 
  { "Reserved for local use", "qaa-qtz", 0 }, 
  { "Romance (Other)", "roa", 0 }, 
  { "Romanian", "rum", "ro" }, 
  { "Romanian", "ron", "ro" }, 
  { "Romansh", "roh", "rm" }, 
  { "Romany", "rom", 0 }, 
  { "Rundi", "run", "rn" }, 
  { "Russian", "rus", "ru" }, 
  { "Salishan languages", "sal", 0 }, 
  { "Samaritan Aramaic", "sam", 0 }, 
  { "Sami languages (Other)", "smi", 0 }, 
  { "Samoan", "smo", "sm" }, 
  { "Sandawe", "sad", 0 }, 
  { "Sango", "sag", "sg" }, 
  { "Sanskrit", "san", "sa" }, 
  { "Santali", "sat", 0 }, 
  { "Sardinian", "srd", "sc" }, 
  { "Sasak", "sas", 0 }, 
  { "Saxon, Low", "nds", 0 }, 
  { "Scots", "sco", 0 }, 
  { "Scottish Gaelic", "gla", "gd" }, 
  { "Selkup", "sel", 0 }, 
  { "Semitic (Other)", "sem", 0 }, 
  { "Sepedi", "nso", 0 }, 
  { "Serbian", "scc", "sr" }, 
  { "Serbian", "srp", "sr" }, 
  { "Serer", "srr", 0 }, 
  { "Shan", "shn", 0 }, 
  { "Shona", "sna", "sn" }, 
  { "Sichuan Yi", "iii", "ii" }, 
  { "Sicilian", "scn", 0 }, 
  { "Sidamo", "sid", 0 }, 
  { "Sign Languages", "sgn", 0 }, 
  { "Siksika", "bla", 0 }, 
  { "Sindhi", "snd", "sd" }, 
  { "Sinhala", "sin", "si" }, 
  { "Sinhalese", "sin", "si" }, 
  { "Sino-Tibetan (Other)", "sit", 0 }, 
  { "Siouan languages", "sio", 0 }, 
  { "Skolt Sami", "sms", 0 }, 
  { "Slave (Athapascan)", "den", 0 }, 
  { "Slavic (Other)", "sla", 0 }, 
  { "Slovak", "slo", "sk" }, 
  { "Slovak", "slk", "sk" }, 
  { "Slovenian", "slv", "sl" }, 
  { "Sogdian", "sog", 0 }, 
  { "Somali", "som", "so" }, 
  { "Songhai languages", "son", 0 }, 
  { "Soninke", "snk", 0 }, 
  { "Sorbian languages", "wen", 0 }, 
  { "Sotho, Northern", "nso", 0 }, 
  { "Sotho, Southern", "sot", "st" }, 
  { "South American Indian (Other)", "sai", 0 }, 
  { "South Ndebele", "nbl", "nr" }, 
  { "Southern Altai", "alt", 0 }, 
  { "Southern Sami", "sma", 0 }, 
  { "Spanish", "spa", "es" }, 
  { "Sranan Tongo", "srn", 0 }, 
  { "Sukuma", "suk", 0 }, 
  { "Sumerian", "sux", 0 }, 
  { "Sundanese", "sun", "su" }, 
  { "Susu", "sus", 0 }, 
  { "Swahili", "swa", "sw" }, 
  { "Swati", "ssw", "ss" }, 
  { "Swedish", "swe", "sv" }, 
  { "Swiss German", "gsw", 0 }, 
  { "Syriac", "syr", 0 }, 
  { "Tagalog", "tgl", "tl" }, 
  { "Tahitian", "tah", "ty" }, 
  { "Tai (Other)", "tai", 0 }, 
  { "Tajik", "tgk", "tg" }, 
  { "Tamashek", "tmh", 0 }, 
  { "Tamil", "tam", "ta" }, 
  { "Tatar", "tat", "tt" }, 
  { "Telugu", "tel", "te" }, 
  { "Tereno", "ter", 0 }, 
  { "Tetum", "tet", 0 }, 
  { "Thai", "tha", "th" }, 
  { "Tibetan", "tib", "bo" }, 
  { "Tibetan", "bod", "bo" }, 
  { "Tigre", "tig", 0 }, 
  { "Tigrinya", "tir", "ti" }, 
  { "Timne", "tem", 0 }, 
  { "Tiv", "tiv", 0 }, 
  { "tlhIngan-Hol", "tlh", 0 }, 
  { "Tlingit", "tli", 0 }, 
  { "Tok Pisin", "tpi", 0 }, 
  { "Tokelau", "tkl", 0 }, 
  { "Tonga (Nyasa)", "tog", 0 }, 
  { "Tonga (Tonga Islands)", "ton", "to" }, 
  { "Tsimshian", "tsi", 0 }, 
  { "Tsonga", "tso", "ts" }, 
  { "Tswana", "tsn", "tn" }, 
  { "Tumbuka", "tum", 0 }, 
  { "Tupi languages", "tup", 0 }, 
  { "Turkish", "tur", "tr" }, 
  { "Turkish, Ottoman (1500-1928)", "ota", 0 }, 
  { "Turkmen", "tuk", "tk" }, 
  { "Tuvalu", "tvl", 0 }, 
  { "Tuvinian", "tyv", 0 }, 
  { "Twi", "twi", "tw" }, 
  { "Udmurt", "udm", 0 }, 
  { "Ugaritic", "uga", 0 }, 
  { "Uighur", "uig", "ug" }, 
  { "Ukrainian", "ukr", "uk" }, 
  { "Umbundu", "umb", 0 }, 
  { "Undetermined", "und", 0 }, 
  { "Upper Sorbian", "hsb", 0 }, 
  { "Urdu", "urd", "ur" }, 
  { "Uyghur", "uig", "ug" }, 
  { "Uzbek", "uzb", "uz" }, 
  { "Vai", "vai", 0 }, 
  { "Valencian", "cat", "ca" }, 
  { "Venda", "ven", "ve" }, 
  { "Vietnamese", "vie", "vi" }, 
  { "Volapuk", "vol", "vo" }, 
  { "Votic", "vot", 0 }, 
  { "Wakashan languages", "wak", 0 }, 
  { "Walamo", "wal", 0 }, 
  { "Walloon", "wln", "wa" }, 
  { "Waray", "war", 0 }, 
  { "Washo", "was", 0 }, 
  { "Welsh", "wel", "cy" }, 
  { "Welsh", "cym", "cy" }, 
  { "Western Frisian", "fry", "fy" }, 
  { "Wolof", "wol", "wo" }, 
  { "Xhosa", "xho", "xh" }, 
  { "Yakut", "sah", 0 }, 
  { "Yao", "yao", 0 }, 
  { "Yapese", "yap", 0 }, 
  { "Yiddish", "yid", "yi" }, 
  { "Yoruba", "yor", "yo" }, 
  { "Yupik languages", "ypk", 0 }, 
  { "Zande languages", "znd", 0 }, 
  { "Zapotec", "zap", 0 }, 
  { "Zaza", "zza", 0 }, 
  { "Zazaki", "zza", 0 }, 
  { "Zenaga", "zen", 0 }, 
  { "Zhuang", "zha", "za" }, 
  { "Zulu", "zul", "zu" }, 
  { "Zuni", "zun", 0 }, 
  { 0, 0, 0 }
};
