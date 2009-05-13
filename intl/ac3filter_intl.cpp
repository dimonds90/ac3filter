#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ac3filter_intl.h"
#include "libintl.h"

#define array_size(a) (sizeof(a) / sizeof(a[0]))

///////////////////////////////////////////////////////////////////////////////
// gettext wrapper

const char *gettext_wrapper(const char *s)
{
  return gettext(s);
}

///////////////////////////////////////////////////////////////////////////////
// Language selection

// Global language code
static char lang[LANG_LEN] = "\0";
extern "C" int _nl_msg_cat_cntr;

void set_lang(const char *code, const char *package, const char *path)
{
#ifndef DISABLE_NLS
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
      lang[0] = 0;
      char lang_env[256];
      sprintf(lang_env, "LANGUAGE=en", lang);
      _putenv(lang_env);
      ++_nl_msg_cat_cntr;
    }
    else
    {
      strncpy(lang, code, LANG_LEN);
      lang[LANG_LEN-1] = 0;
      char lang_env[LANG_LEN + 10];
      sprintf(lang_env, "LANGUAGE=%s", lang);
      _putenv(lang_env);
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

///////////////////////////////////////////////////////////////////////////////
// Tables

struct iso_lang_s
{ 
  const char *name;
  const char *iso6392;
  const char *iso6391;
};

struct iso_country_s
{
  const char *name;
  const char *alpha3;
  const char *alpha2;
  int code;
};

static const iso_lang_s iso_langs[] = 
{
  { "English", "eng", "en" },
  { "Afar", "aar", "aa" },
  { "Abkhazian", "abk", "ab" },
  { "Achinese", "ace", 0 },
  { "Acoli", "ach", 0 },
  { "Adangme", "ada", 0 },
  { "Adyghe", "ady", 0 },
  { "Adygei", "ady", 0 },
  { "Afro-Asiatic (Other)", "afa", 0 },
  { "Afrihili", "afh", 0 },
  { "Afrikaans", "afr", "af" },
  { "Ainu", "ain", 0 },
  { "Akan", "aka", "ak" },
  { "Akkadian", "akk", 0 },
  { "Albanian", "alb", "sq" },
  { "Aleut", "ale", 0 },
  { "Algonquian languages", "alg", 0 },
  { "Southern Altai", "alt", 0 },
  { "Amharic", "amh", "am" },
  { "English, Old (ca.450-1100)", "ang", 0 },
  { "Angika", "anp", 0 },
  { "Apache languages", "apa", 0 },
  { "Arabic", "ara", "ar" },
  { "Official Aramaic (700-300 BCE)", "arc", 0 },
  { "Imperial Aramaic (700-300 BCE)", "arc", 0 },
  { "Aragonese", "arg", "an" },
  { "Armenian", "arm", "hy" },
  { "Mapudungun", "arn", 0 },
  { "Mapuche", "arn", 0 },
  { "Arapaho", "arp", 0 },
  { "Artificial (Other)", "art", 0 },
  { "Arawak", "arw", 0 },
  { "Assamese", "asm", "as" },
  { "Asturian", "ast", 0 },
  { "Bable", "ast", 0 },
  { "Athapascan languages", "ath", 0 },
  { "Australian languages", "aus", 0 },
  { "Avaric", "ava", "av" },
  { "Avestan", "ave", "ae" },
  { "Awadhi", "awa", 0 },
  { "Aymara", "aym", "ay" },
  { "Azerbaijani", "aze", "az" },
  { "Banda languages", "bad", 0 },
  { "Bamileke languages", "bai", 0 },
  { "Bashkir", "bak", "ba" },
  { "Baluchi", "bal", 0 },
  { "Bambara", "bam", "bm" },
  { "Balinese", "ban", 0 },
  { "Basque", "baq", "eu" },
  { "Basa", "bas", 0 },
  { "Baltic (Other)", "bat", 0 },
  { "Beja", "bej", 0 },
  { "Belarusian", "bel", "be" },
  { "Bemba", "bem", 0 },
  { "Bengali", "ben", "bn" },
  { "Berber (Other)", "ber", 0 },
  { "Bhojpuri", "bho", 0 },
  { "Bihari", "bih", "bh" },
  { "Bikol", "bik", 0 },
  { "Bini", "bin", 0 },
  { "Edo", "bin", 0 },
  { "Bislama", "bis", "bi" },
  { "Siksika", "bla", 0 },
  { "Bantu (Other)", "bnt", 0 },
  { "Tibetan", "bod", "bo" },
  { "Bosnian", "bos", "bs" },
  { "Braj", "bra", 0 },
  { "Breton", "bre", "br" },
  { "Batak languages", "btk", 0 },
  { "Buriat", "bua", 0 },
  { "Buginese", "bug", 0 },
  { "Bulgarian", "bul", "bg" },
  { "Burmese", "bur", "my" },
  { "Blin", "byn", 0 },
  { "Bilin", "byn", 0 },
  { "Caddo", "cad", 0 },
  { "Central American Indian (Other)", "cai", 0 },
  { "Galibi Carib", "car", 0 },
  { "Catalan", "cat", "ca" },
  { "Valencian", "cat", "ca" },
  { "Caucasian (Other)", "cau", 0 },
  { "Cebuano", "ceb", 0 },
  { "Celtic (Other)", "cel", 0 },
  { "Czech", "ces", "cs" },
  { "Chamorro", "cha", "ch" },
  { "Chibcha", "chb", 0 },
  { "Chechen", "che", "ce" },
  { "Chagatai", "chg", 0 },
  { "Chinese", "chi", "zh" },
  { "Chuukese", "chk", 0 },
  { "Mari", "chm", 0 },
  { "Chinook jargon", "chn", 0 },
  { "Choctaw", "cho", 0 },
  { "Chipewyan", "chp", 0 },
  { "Cherokee", "chr", 0 },
  { "Church Slavic", "chu", "cu" },
  { "Old Slavonic", "chu", "cu" },
  { "Church Slavonic", "chu", "cu" },
  { "Old Bulgarian", "chu", "cu" },
  { "Old Church Slavonic", "chu", "cu" },
  { "Chuvash", "chv", "cv" },
  { "Cheyenne", "chy", 0 },
  { "Chamic languages", "cmc", 0 },
  { "Coptic", "cop", 0 },
  { "Cornish", "cor", "kw" },
  { "Corsican", "cos", "co" },
  { "Creoles and pidgins, English based (Other)", "cpe", 0 },
  { "Creoles and pidgins, French-based (Other)", "cpf", 0 },
  { "Creoles and pidgins, Portuguese-based (Other)", "cpp", 0 },
  { "Cree", "cre", "cr" },
  { "Crimean Tatar", "crh", 0 },
  { "Crimean Turkish", "crh", 0 },
  { "Creoles and pidgins (Other)", "crp", 0 },
  { "Kashubian", "csb", 0 },
  { "Cushitic (Other)", "cus", 0 },
  { "Welsh", "cym", "cy" },
  { "Czech", "cze", "cs" },
  { "Dakota", "dak", 0 },
  { "Danish", "dan", "da" },
  { "Dargwa", "dar", 0 },
  { "Land Dayak languages", "day", 0 },
  { "Delaware", "del", 0 },
  { "Slave (Athapascan)", "den", 0 },
  { "German", "deu", "de" },
  { "Dogrib", "dgr", 0 },
  { "Dinka", "din", 0 },
  { "Divehi", "div", "dv" },
  { "Dhivehi", "div", "dv" },
  { "Maldivian", "div", "dv" },
  { "Dogri", "doi", 0 },
  { "Dravidian (Other)", "dra", 0 },
  { "Lower Sorbian", "dsb", 0 },
  { "Duala", "dua", 0 },
  { "Dutch, Middle (ca.1050-1350)", "dum", 0 },
  { "Dutch", "dut", "nl" },
  { "Flemish", "dut", "nl" },
  { "Dyula", "dyu", 0 },
  { "Dzongkha", "dzo", "dz" },
  { "Efik", "efi", 0 },
  { "Egyptian (Ancient)", "egy", 0 },
  { "Ekajuk", "eka", 0 },
  { "Greek", "ell", "el" },
  { "Elamite", "elx", 0 },
  { "English, Middle (1100-1500)", "enm", 0 },
  { "Esperanto", "epo", "eo" },
  { "Estonian", "est", "et" },
  { "Basque", "eus", "eu" },
  { "Ewe", "ewe", "ee" },
  { "Ewondo", "ewo", 0 },
  { "Fang", "fan", 0 },
  { "Faroese", "fao", "fo" },
  { "Persian", "fas", "fa" },
  { "Fanti", "fat", 0 },
  { "Fijian", "fij", "fj" },
  { "Filipino", "fil", 0 },
  { "Pilipino", "fil", 0 },
  { "Finnish", "fin", "fi" },
  { "Finno-Ugrian (Other)", "fiu", 0 },
  { "Fon", "fon", 0 },
  { "French", "fra", "fr" },
  { "French", "fre", "fr" },
  { "French, Middle (ca.1400-1600)", "frm", 0 },
  { "French, Old (842-ca.1400)", "fro", 0 },
  { "Northern Frisian", "frr", 0 },
  { "Eastern Frisian", "frs", 0 },
  { "Western Frisian", "fry", "fy" },
  { "Fulah", "ful", "ff" },
  { "Friulian", "fur", 0 },
  { "Ga", "gaa", 0 },
  { "Gayo", "gay", 0 },
  { "Gbaya", "gba", 0 },
  { "Germanic (Other)", "gem", 0 },
  { "Georgian", "geo", "ka" },
  { "German", "ger", "de" },
  { "Geez", "gez", 0 },
  { "Gilbertese", "gil", 0 },
  { "Gaelic", "gla", "gd" },
  { "Scottish Gaelic", "gla", "gd" },
  { "Irish", "gle", "ga" },
  { "Galician", "glg", "gl" },
  { "Manx", "glv", "gv" },
  { "German, Middle High (ca.1050-1500)", "gmh", 0 },
  { "German, Old High (ca.750-1050)", "goh", 0 },
  { "Gondi", "gon", 0 },
  { "Gorontalo", "gor", 0 },
  { "Gothic", "got", 0 },
  { "Grebo", "grb", 0 },
  { "Greek, Ancient (to 1453)", "grc", 0 },
  { "Greek", "gre", "el" },
  { "Guarani", "grn", "gn" },
  { "Swiss German", "gsw", 0 },
  { "Alemannic", "gsw", 0 },
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
  { "Hittite", "hit", 0 },
  { "Hmong", "hmn", 0 },
  { "Hiri Motu", "hmo", "ho" },
  { "Croatian", "hrv", "hr" },
  { "Upper Sorbian", "hsb", 0 },
  { "Hungarian", "hun", "hu" },
  { "Hupa", "hup", 0 },
  { "Armenian", "hye", "hy" },
  { "Iban", "iba", 0 },
  { "Igbo", "ibo", "ig" },
  { "Icelandic", "ice", "is" },
  { "Ido", "ido", "io" },
  { "Sichuan Yi", "iii", "ii" },
  { "Ijo languages", "ijo", 0 },
  { "Inuktitut", "iku", "iu" },
  { "Interlingue", "ile", "ie" },
  { "Iloko", "ilo", 0 },
  { "Interlingua", "ina", "ia" },
  { "Indic (Other)", "inc", 0 },
  { "Indonesian", "ind", "id" },
  { "Indo-European (Other)", "ine", 0 },
  { "Ingush", "inh", 0 },
  { "Inupiaq", "ipk", "ik" },
  { "Iranian (Other)", "ira", 0 },
  { "Iroquoian languages", "iro", 0 },
  { "Icelandic", "isl", "is" },
  { "Italian", "ita", "it" },
  { "Javanese", "jav", "jv" },
  { "Lojban", "jbo", 0 },
  { "Japanese", "jpn", "ja" },
  { "Judeo-Persian", "jpr", 0 },
  { "Judeo-Arabic", "jrb", 0 },
  { "Kara-Kalpak", "kaa", 0 },
  { "Kabyle", "kab", 0 },
  { "Kachin", "kac", 0 },
  { "Jingpho", "kac", 0 },
  { "Kalaallisut", "kal", "kl" },
  { "Greenlandic", "kal", "kl" },
  { "Kamba", "kam", 0 },
  { "Kannada", "kan", "kn" },
  { "Karen languages", "kar", 0 },
  { "Kashmiri", "kas", "ks" },
  { "Georgian", "kat", "ka" },
  { "Kanuri", "kau", "kr" },
  { "Kawi", "kaw", 0 },
  { "Kazakh", "kaz", "kk" },
  { "Kabardian", "kbd", 0 },
  { "Khasi", "kha", 0 },
  { "Khoisan (Other)", "khi", 0 },
  { "Central Khmer", "khm", "km" },
  { "Khotanese", "kho", 0 },
  { "Kikuyu", "kik", "ki" },
  { "Gikuyu", "kik", "ki" },
  { "Kinyarwanda", "kin", "rw" },
  { "Kirghiz", "kir", "ky" },
  { "Kyrgyz", "kir", "ky" },
  { "Kimbundu", "kmb", 0 },
  { "Konkani", "kok", 0 },
  { "Komi", "kom", "kv" },
  { "Kongo", "kon", "kg" },
  { "Korean", "kor", "ko" },
  { "Kosraean", "kos", 0 },
  { "Kpelle", "kpe", 0 },
  { "Karachay-Balkar", "krc", 0 },
  { "Karelian", "krl", 0 },
  { "Kru languages", "kro", 0 },
  { "Kurukh", "kru", 0 },
  { "Kuanyama", "kua", "kj" },
  { "Kwanyama", "kua", "kj" },
  { "Kumyk", "kum", 0 },
  { "Kurdish", "kur", "ku" },
  { "Kutenai", "kut", 0 },
  { "Ladino", "lad", 0 },
  { "Lahnda", "lah", 0 },
  { "Lamba", "lam", 0 },
  { "Lao", "lao", "lo" },
  { "Latin", "lat", "la" },
  { "Latvian", "lav", "lv" },
  { "Lezghian", "lez", 0 },
  { "Limburgan", "lim", "li" },
  { "Limburger", "lim", "li" },
  { "Limburgish", "lim", "li" },
  { "Lingala", "lin", "ln" },
  { "Lithuanian", "lit", "lt" },
  { "Mongo", "lol", 0 },
  { "Lozi", "loz", 0 },
  { "Luxembourgish", "ltz", "lb" },
  { "Letzeburgesch", "ltz", "lb" },
  { "Luba-Lulua", "lua", 0 },
  { "Luba-Katanga", "lub", "lu" },
  { "Ganda", "lug", "lg" },
  { "Luiseno", "lui", 0 },
  { "Lunda", "lun", 0 },
  { "Luo (Kenya and Tanzania)", "luo", 0 },
  { "Lushai", "lus", 0 },
  { "Macedonian", "mac", "mk" },
  { "Madurese", "mad", 0 },
  { "Magahi", "mag", 0 },
  { "Marshallese", "mah", "mh" },
  { "Maithili", "mai", 0 },
  { "Makasar", "mak", 0 },
  { "Malayalam", "mal", "ml" },
  { "Mandingo", "man", 0 },
  { "Maori", "mao", "mi" },
  { "Austronesian (Other)", "map", 0 },
  { "Marathi", "mar", "mr" },
  { "Masai", "mas", 0 },
  { "Malay", "may", "ms" },
  { "Moksha", "mdf", 0 },
  { "Mandar", "mdr", 0 },
  { "Mende", "men", 0 },
  { "Irish, Middle (900-1200)", "mga", 0 },
  { "Mi'kmaq", "mic", 0 },
  { "Micmac", "mic", 0 },
  { "Minangkabau", "min", 0 },
  { "Miscellaneous languages", "mis", 0 },
  { "Macedonian", "mkd", "mk" },
  { "Mon-Khmer (Other)", "mkh", 0 },
  { "Malagasy", "mlg", "mg" },
  { "Maltese", "mlt", "mt" },
  { "Manchu", "mnc", 0 },
  { "Manipuri", "mni", 0 },
  { "Manobo languages", "mno", 0 },
  { "Mohawk", "moh", 0 },
  { "Moldavian", "mol", "mo" },
  { "Mongolian", "mon", "mn" },
  { "Mossi", "mos", 0 },
  { "Maori", "mri", "mi" },
  { "Malay", "msa", "ms" },
  { "Multiple languages", "mul", 0 },
  { "Munda languages", "mun", 0 },
  { "Creek", "mus", 0 },
  { "Mirandese", "mwl", 0 },
  { "Marwari", "mwr", 0 },
  { "Burmese", "mya", "my" },
  { "Mayan languages", "myn", 0 },
  { "Erzya", "myv", 0 },
  { "Nahuatl languages", "nah", 0 },
  { "North American Indian", "nai", 0 },
  { "Neapolitan", "nap", 0 },
  { "Nauru", "nau", "na" },
  { "Navajo", "nav", "nv" },
  { "Navaho", "nav", "nv" },
  { "Ndebele, South", "nbl", "nr" },
  { "South Ndebele", "nbl", "nr" },
  { "Ndebele, North", "nde", "nd" },
  { "North Ndebele", "nde", "nd" },
  { "Ndonga", "ndo", "ng" },
  { "Low German", "nds", 0 },
  { "Low Saxon", "nds", 0 },
  { "German, Low", "nds", 0 },
  { "Saxon, Low", "nds", 0 },
  { "Nepali", "nep", "ne" },
  { "Nepal Bhasa", "new", 0 },
  { "Newari", "new", 0 },
  { "Nias", "nia", 0 },
  { "Niger-Kordofanian (Other)", "nic", 0 },
  { "Niuean", "niu", 0 },
  { "Dutch", "nld", "nl" },
  { "Flemish", "nld", "nl" },
  { "Norwegian Nynorsk", "nno", "nn" },
  { "Nynorsk, Norwegian", "nno", "nn" },
  { "Bokmal, Norwegian", "nob", "nb" },
  { "Norwegian Bokmal", "nob", "nb" },
  { "Nogai", "nog", 0 },
  { "Norse, Old", "non", 0 },
  { "Norwegian", "nor", "no" },
  { "N'Ko", "nqo", 0 },
  { "Pedi", "nso", 0 },
  { "Sepedi", "nso", 0 },
  { "Northern Sotho", "nso", 0 },
  { "Nubian languages", "nub", 0 },
  { "Classical Newari", "nwc", 0 },
  { "Old Newari", "nwc", 0 },
  { "Classical Nepal Bhasa", "nwc", 0 },
  { "Chichewa", "nya", "ny" },
  { "Chewa", "nya", "ny" },
  { "Nyanja", "nya", "ny" },
  { "Nyamwezi", "nym", 0 },
  { "Nyankole", "nyn", 0 },
  { "Nyoro", "nyo", 0 },
  { "Nzima", "nzi", 0 },
  { "Occitan (post 1500)", "oci", "oc" },
  { "Provencal", "oci", "oc" },
  { "Ojibwa", "oji", "oj" },
  { "Oriya", "ori", "or" },
  { "Oromo", "orm", "om" },
  { "Osage", "osa", 0 },
  { "Ossetian", "oss", "os" },
  { "Ossetic", "oss", "os" },
  { "Turkish, Ottoman (1500-1928)", "ota", 0 },
  { "Otomian languages", "oto", 0 },
  { "Papuan (Other)", "paa", 0 },
  { "Pangasinan", "pag", 0 },
  { "Pahlavi", "pal", 0 },
  { "Pampanga", "pam", 0 },
  { "Panjabi", "pan", "pa" },
  { "Punjabi", "pan", "pa" },
  { "Papiamento", "pap", 0 },
  { "Palauan", "pau", 0 },
  { "Persian, Old (ca.600-400 B.C.)", "peo", 0 },
  { "Persian", "per", "fa" },
  { "Philippine (Other)", "phi", 0 },
  { "Phoenician", "phn", 0 },
  { "Pali", "pli", "pi" },
  { "Polish", "pol", "pl" },
  { "Pohnpeian", "pon", 0 },
  { "Portuguese", "por", "pt" },
  { "Prakrit languages", "pra", 0 },
  { "Provencal, Old (to 1500)", "pro", 0 },
  { "Pushto", "pus", "ps" },
  { "Reserved for local use", "qaa-qtz", 0 },
  { "Quechua", "que", "qu" },
  { "Rajasthani", "raj", 0 },
  { "Rapanui", "rap", 0 },
  { "Rarotongan", "rar", 0 },
  { "Cook Islands Maori", "rar", 0 },
  { "Romance (Other)", "roa", 0 },
  { "Romansh", "roh", "rm" },
  { "Romany", "rom", 0 },
  { "Romanian", "ron", "ro" },
  { "Romanian", "rum", "ro" },
  { "Rundi", "run", "rn" },
  { "Aromanian", "rup", 0 },
  { "Arumanian", "rup", 0 },
  { "Macedo-Romanian", "rup", 0 },
  { "Russian", "rus", "ru" },
  { "Sandawe", "sad", 0 },
  { "Sango", "sag", "sg" },
  { "Yakut", "sah", 0 },
  { "South American Indian (Other)", "sai", 0 },
  { "Salishan languages", "sal", 0 },
  { "Samaritan Aramaic", "sam", 0 },
  { "Sanskrit", "san", "sa" },
  { "Sasak", "sas", 0 },
  { "Santali", "sat", 0 },
  { "Serbian", "scc", "sr" },
  { "Sicilian", "scn", 0 },
  { "Scots", "sco", 0 },
  { "Croatian", "scr", "hr" },
  { "Selkup", "sel", 0 },
  { "Semitic (Other)", "sem", 0 },
  { "Irish, Old (to 900)", "sga", 0 },
  { "Sign Languages", "sgn", 0 },
  { "Shan", "shn", 0 },
  { "Sidamo", "sid", 0 },
  { "Sinhala", "sin", "si" },
  { "Sinhalese", "sin", "si" },
  { "Siouan languages", "sio", 0 },
  { "Sino-Tibetan (Other)", "sit", 0 },
  { "Slavic (Other)", "sla", 0 },
  { "Slovak", "slk", "sk" },
  { "Slovak", "slo", "sk" },
  { "Slovenian", "slv", "sl" },
  { "Southern Sami", "sma", 0 },
  { "Northern Sami", "sme", "se" },
  { "Sami languages (Other)", "smi", 0 },
  { "Lule Sami", "smj", 0 },
  { "Inari Sami", "smn", 0 },
  { "Samoan", "smo", "sm" },
  { "Skolt Sami", "sms", 0 },
  { "Shona", "sna", "sn" },
  { "Sindhi", "snd", "sd" },
  { "Soninke", "snk", 0 },
  { "Sogdian", "sog", 0 },
  { "Somali", "som", "so" },
  { "Songhai languages", "son", 0 },
  { "Sotho, Southern", "sot", "st" },
  { "Spanish", "spa", "es" },
  { "Castilian", "spa", "es" },
  { "Albanian", "sqi", "sq" },
  { "Sardinian", "srd", "sc" },
  { "Sranan Tongo", "srn", 0 },
  { "Serbian", "srp", "sr" },
  { "Serer", "srr", 0 },
  { "Nilo-Saharan (Other)", "ssa", 0 },
  { "Swati", "ssw", "ss" },
  { "Sukuma", "suk", 0 },
  { "Sundanese", "sun", "su" },
  { "Susu", "sus", 0 },
  { "Sumerian", "sux", 0 },
  { "Swahili", "swa", "sw" },
  { "Swedish", "swe", "sv" },
  { "Classical Syriac", "syc", 0 },
  { "Syriac", "syr", 0 },
  { "Tahitian", "tah", "ty" },
  { "Tai (Other)", "tai", 0 },
  { "Tamil", "tam", "ta" },
  { "Tatar", "tat", "tt" },
  { "Telugu", "tel", "te" },
  { "Timne", "tem", 0 },
  { "Tereno", "ter", 0 },
  { "Tetum", "tet", 0 },
  { "Tajik", "tgk", "tg" },
  { "Tagalog", "tgl", "tl" },
  { "Thai", "tha", "th" },
  { "Tibetan", "tib", "bo" },
  { "Tigre", "tig", 0 },
  { "Tigrinya", "tir", "ti" },
  { "Tiv", "tiv", 0 },
  { "Tokelau", "tkl", 0 },
  { "Klingon", "tlh", 0 },
  { "tlhIngan-Hol", "tlh", 0 },
  { "Tlingit", "tli", 0 },
  { "Tamashek", "tmh", 0 },
  { "Tonga (Nyasa)", "tog", 0 },
  { "Tonga (Tonga Islands)", "ton", "to" },
  { "Tok Pisin", "tpi", 0 },
  { "Tsimshian", "tsi", 0 },
  { "Tswana", "tsn", "tn" },
  { "Tsonga", "tso", "ts" },
  { "Turkmen", "tuk", "tk" },
  { "Tumbuka", "tum", 0 },
  { "Tupi languages", "tup", 0 },
  { "Turkish", "tur", "tr" },
  { "Altaic (Other)", "tut", 0 },
  { "Tuvalu", "tvl", 0 },
  { "Twi", "twi", "tw" },
  { "Tuvinian", "tyv", 0 },
  { "Udmurt", "udm", 0 },
  { "Ugaritic", "uga", 0 },
  { "Uighur", "uig", "ug" },
  { "Uyghur", "uig", "ug" },
  { "Ukrainian", "ukr", "uk" },
  { "Umbundu", "umb", 0 },
  { "Undetermined", "und", 0 },
  { "Urdu", "urd", "ur" },
  { "Uzbek", "uzb", "uz" },
  { "Vai", "vai", 0 },
  { "Venda", "ven", "ve" },
  { "Vietnamese", "vie", "vi" },
  { "Volapuk", "vol", "vo" },
  { "Votic", "vot", 0 },
  { "Wakashan languages", "wak", 0 },
  { "Walamo", "wal", 0 },
  { "Waray", "war", 0 },
  { "Washo", "was", 0 },
  { "Welsh", "wel", "cy" },
  { "Sorbian languages", "wen", 0 },
  { "Walloon", "wln", "wa" },
  { "Wolof", "wol", "wo" },
  { "Kalmyk", "xal", 0 },
  { "Oirat", "xal", 0 },
  { "Xhosa", "xho", "xh" },
  { "Yao", "yao", 0 },
  { "Yapese", "yap", 0 },
  { "Yiddish", "yid", "yi" },
  { "Yoruba", "yor", "yo" },
  { "Yupik languages", "ypk", 0 },
  { "Zapotec", "zap", 0 },
  { "Zenaga", "zen", 0 },
  { "Zhuang", "zha", "za" },
  { "Chuang", "zha", "za" },
  { "Chinese", "zho", "zh" },
  { "Zande languages", "znd", 0 },
  { "Zulu", "zul", "zu" },
  { "Zuni", "zun", 0 },
  { "No linguistic content", "zxx", 0 },
  { "Zaza", "zza", 0 },
  { "Dimili", "zza", 0 },
  { "Dimli", "zza", 0 },
  { "Kirdki", "zza", 0 },
  { "Kirmanjki", "zza", 0 },
  { "Zazaki", "zza", 0 },
  { 0, 0, 0 }
};

static const iso_country_s iso_countries[] =
{
  { "Afghanistan", "AFG", "AF", 4 },
  { "Aland Islands", "ALA", "AX", 248 },
  { "Albania", "ALB", "AL", 8 },
  { "Algeria", "DZA", "DZ", 12 },
  { "American Samoa", "ASM", "AS", 16 },
  { "Andorra", "AND", "AD", 20 },
  { "Angola", "AGO", "AO", 24 },
  { "Anguilla", "AIA", "AI", 660 },
  { "Antarctica", "ATA", "AQ", 10 },
  { "Antigua and Barbuda", "ATG", "AG", 28 },
  { "Argentina", "ARG", "AR", 32 },
  { "Armenia", "ARM", "AM", 51 },
  { "Aruba", "ABW", "AW", 533 },
  { "Australia", "AUS", "AU", 36 },
  { "Austria", "AUT", "AT", 40 },
  { "Azerbaijan", "AZE", "AZ", 31 },
  { "Bahamas", "BHS", "BS", 44 },
  { "Bahrain", "BHR", "BH", 48 },
  { "Bangladesh", "BGD", "BD", 50 },
  { "Barbados", "BRB", "BB", 52 },
  { "Belarus", "BLR", "BY", 112 },
  { "Belgium", "BEL", "BE", 56 },
  { "Belize", "BLZ", "BZ", 84 },
  { "Benin", "BEN", "BJ", 204 },
  { "Bermuda", "BMU", "BM", 60 },
  { "Bhutan", "BTN", "BT", 64 },
  { "Bolivia", "BOL", "BO", 68 },
  { "Bosnia and Herzegovina", "BIH", "BA", 70 },
  { "Botswana", "BWA", "BW", 72 },
  { "Bouvet Island", "BVT", "BV", 74 },
  { "Brazil", "BRA", "BR", 76 },
  { "British Indian Ocean Territory", "IOT", "IO", 86 },
  { "Brunei Darussalam", "BRN", "BN", 96 },
  { "Bulgaria", "BGR", "BG", 100 },
  { "Burkina Faso", "BFA", "BF", 854 },
  { "Burundi", "BDI", "BI", 108 },
  { "Cambodia", "KHM", "KH", 116 },
  { "Cameroon", "CMR", "CM", 120 },
  { "Canada", "CAN", "CA", 124 },
  { "Cape Verde", "CPV", "CV", 132 },
  { "Cayman Islands", "CYM", "KY", 136 },
  { "Central African Republic", "CAF", "CF", 140 },
  { "Chad", "TCD", "TD", 148 },
  { "Chile", "CHL", "CL", 152 },
  { "China", "CHN", "CN", 156 },
  { "Christmas Island", "CXR", "CX", 162 },
  { "Cocos (Keeling) Islands", "CCK", "CC", 166 },
  { "Colombia", "COL", "CO", 170 },
  { "Comoros", "COM", "KM", 174 },
  { "Congo", "COG", "CG", 178 },
  { "Congo, Democratic Republic of the", "COD", "CD", 180 },
  { "Cook Islands", "COK", "CK", 184 },
  { "Costa Rica", "CRI", "CR", 188 },
  { "Cote d'Ivoire", "CIV", "CI", 384 },
  { "Croatia", "HRV", "HR", 191 },
  { "Cuba", "CUB", "CU", 192 },
  { "Cyprus", "CYP", "CY", 196 },
  { "Czech Republic", "CZE", "CZ", 203 },
  { "Denmark", "DNK", "DK", 208 },
  { "Djibouti", "DJI", "DJ", 262 },
  { "Dominica", "DMA", "DM", 212 },
  { "Dominican Republic", "DOM", "DO", 214 },
  { "Ecuador", "ECU", "EC", 218 },
  { "Egypt", "EGY", "EG", 818 },
  { "El Salvador", "SLV", "SV", 222 },
  { "Equatorial Guinea", "GNQ", "GQ", 226 },
  { "Eritrea", "ERI", "ER", 232 },
  { "Estonia", "EST", "EE", 233 },
  { "Ethiopia", "ETH", "ET", 231 },
  { "Falkland Islands (Malvinas)", "FLK", "FK", 238 },
  { "Faroe Islands", "FRO", "FO", 234 },
  { "Fiji", "FJI", "FJ", 242 },
  { "Finland", "FIN", "FI", 246 },
  { "France", "FRA", "FR", 250 },
  { "French Guiana", "GUF", "GF", 254 },
  { "French Polynesia", "PYF", "PF", 258 },
  { "French Southern Territories", "ATF", "TF", 260 },
  { "Gabon", "GAB", "GA", 266 },
  { "Gambia", "GMB", "GM", 270 },
  { "Georgia", "GEO", "GE", 268 },
  { "Germany", "DEU", "DE", 276 },
  { "Ghana", "GHA", "GH", 288 },
  { "Gibraltar", "GIB", "GI", 292 },
  { "Greece", "GRC", "GR", 300 },
  { "Greenland", "GRL", "GL", 304 },
  { "Grenada", "GRD", "GD", 308 },
  { "Guadeloupe", "GLP", "GP", 312 },
  { "Guam", "GUM", "GU", 316 },
  { "Guatemala", "GTM", "GT", 320 },
  { "Guernsey", "GGY", "GG", 831 },
  { "Guinea", "GIN", "GN", 324 },
  { "Guinea-Bissau", "GNB", "GW", 624 },
  { "Guyana", "GUY", "GY", 328 },
  { "Haiti", "HTI", "HT", 332 },
  { "Heard Island and McDonald Islands", "HMD", "HM", 334 },
  { "Holy See (Vatican City State)", "VAT", "VA", 336 },
  { "Honduras", "HND", "HN", 340 },
  { "Hong Kong", "HKG", "HK", 344 },
  { "Hungary", "HUN", "HU", 348 },
  { "Iceland", "ISL", "IS", 352 },
  { "India", "IND", "IN", 356 },
  { "Indonesia", "IDN", "ID", 360 },
  { "Iran, Islamic Republic of", "IRN", "IR", 364 },
  { "Iraq", "IRQ", "IQ", 368 },
  { "Ireland", "IRL", "IE", 372 },
  { "Isle of Man", "IMN", "IM", 833 },
  { "Israel", "ISR", "IL", 376 },
  { "Italy", "ITA", "IT", 380 },
  { "Jamaica", "JAM", "JM", 388 },
  { "Japan", "JPN", "JP", 392 },
  { "Jersey", "JEY", "JE", 832 },
  { "Jordan", "JOR", "JO", 400 },
  { "Kazakhstan", "KAZ", "KZ", 398 },
  { "Kenya", "KEN", "KE", 404 },
  { "Kiribati", "KIR", "KI", 296 },
  { "Korea, Democratic People's Republic of", "PRK", "KP", 408 },
  { "Korea, Republic of", "KOR", "KR", 410 },
  { "Kuwait", "KWT", "KW", 414 },
  { "Kyrgyzstan", "KGZ", "KG", 417 },
  { "Lao People's Democratic Republic", "LAO", "LA", 418 },
  { "Latvia", "LVA", "LV", 428 },
  { "Lebanon", "LBN", "LB", 422 },
  { "Lesotho", "LSO", "LS", 426 },
  { "Liberia", "LBR", "LR", 430 },
  { "Libyan Arab Jamahiriya", "LBY", "LY", 434 },
  { "Liechtenstein", "LIE", "LI", 438 },
  { "Lithuania", "LTU", "LT", 440 },
  { "Luxembourg", "LUX", "LU", 442 },
  { "Macao", "MAC", "MO", 446 },
  { "Macedonia, the former Yugoslav Republic of", "MKD", "MK", 807 },
  { "Madagascar", "MDG", "MG", 450 },
  { "Malawi", "MWI", "MW", 454 },
  { "Malaysia", "MYS", "MY", 458 },
  { "Maldives", "MDV", "MV", 462 },
  { "Mali", "MLI", "ML", 466 },
  { "Malta", "MLT", "MT", 470 },
  { "Marshall Islands", "MHL", "MH", 584 },
  { "Martinique", "MTQ", "MQ", 474 },
  { "Mauritania", "MRT", "MR", 478 },
  { "Mauritius", "MUS", "MU", 480 },
  { "Mayotte", "MYT", "YT", 175 },
  { "Mexico", "MEX", "MX", 484 },
  { "Micronesia, Federated States of", "FSM", "FM", 583 },
  { "Moldova", "MDA", "MD", 498 },
  { "Monaco", "MCO", "MC", 492 },
  { "Mongolia", "MNG", "MN", 496 },
  { "Montenegro", "MNE", "ME", 499 },
  { "Montserrat", "MSR", "MS", 500 },
  { "Morocco", "MAR", "MA", 504 },
  { "Mozambique", "MOZ", "MZ", 508 },
  { "Myanmar", "MMR", "MM", 104 },
  { "Namibia", "NAM", "NA", 516 },
  { "Nauru", "NRU", "NR", 520 },
  { "Nepal", "NPL", "NP", 524 },
  { "Netherlands", "NLD", "NL", 528 },
  { "Netherlands Antilles", "ANT", "AN", 530 },
  { "New Caledonia", "NCL", "NC", 540 },
  { "New Zealand", "NZL", "NZ", 554 },
  { "Nicaragua", "NIC", "NI", 558 },
  { "Niger", "NER", "NE", 562 },
  { "Nigeria", "NGA", "NG", 566 },
  { "Niue", "NIU", "NU", 570 },
  { "Norfolk Island", "NFK", "NF", 574 },
  { "Northern Mariana Islands", "MNP", "MP", 580 },
  { "Norway", "NOR", "NO", 578 },
  { "Oman", "OMN", "OM", 512 },
  { "Pakistan", "PAK", "PK", 586 },
  { "Palau", "PLW", "PW", 585 },
  { "Palestinian Territory, Occupied", "PSE", "PS", 275 },
  { "Panama", "PAN", "PA", 591 },
  { "Papua New Guinea", "PNG", "PG", 598 },
  { "Paraguay", "PRY", "PY", 600 },
  { "Peru", "PER", "PE", 604 },
  { "Philippines", "PHL", "PH", 608 },
  { "Pitcairn", "PCN", "PN", 612 },
  { "Poland", "POL", "PL", 616 },
  { "Portugal", "PRT", "PT", 620 },
  { "Puerto Rico", "PRI", "PR", 630 },
  { "Qatar", "QAT", "QA", 634 },
  { "Reunion", "REU", "RE", 638 },
  { "Romania", "ROU", "RO", 642 },
  { "Russian Federation", "RUS", "RU", 643 },
  { "Rwanda", "RWA", "RW", 646 },
  { "Saint Barthelemy", "BLM", "BL", 652 },
  { "Saint Helena", "SHN", "SH", 654 },
  { "Saint Kitts and Nevis", "KNA", "KN", 659 },
  { "Saint Lucia", "LCA", "LC", 662 },
  { "Saint Martin (French part)", "MAF", "MF", 663 },
  { "Saint Pierre and Miquelon", "SPM", "PM", 666 },
  { "Saint Vincent and the Grenadines", "VCT", "VC", 670 },
  { "Samoa", "WSM", "WS", 882 },
  { "San Marino", "SMR", "SM", 674 },
  { "Sao Tome and Principe", "STP", "ST", 678 },
  { "Saudi Arabia", "SAU", "SA", 682 },
  { "Senegal", "SEN", "SN", 686 },
  { "Serbia[5]", "SRB", "RS", 688 },
  { "Seychelles", "SYC", "SC", 690 },
  { "Sierra Leone", "SLE", "SL", 694 },
  { "Singapore", "SGP", "SG", 702 },
  { "Slovakia", "SVK", "SK", 703 },
  { "Slovenia", "SVN", "SI", 705 },
  { "Solomon Islands", "SLB", "SB", 90 },
  { "Somalia", "SOM", "SO", 706 },
  { "South Africa", "ZAF", "ZA", 710 },
  { "South Georgia and the South Sandwich Islands", "SGS", "GS", 239 },
  { "Spain", "ESP", "ES", 724 },
  { "Sri Lanka", "LKA", "LK", 144 },
  { "Sudan", "SDN", "SD", 736 },
  { "Suriname", "SUR", "SR", 740 },
  { "Svalbard and Jan Mayen", "SJM", "SJ", 744 },
  { "Swaziland", "SWZ", "SZ", 748 },
  { "Sweden", "SWE", "SE", 752 },
  { "Switzerland", "CHE", "CH", 756 },
  { "Syrian Arab Republic", "SYR", "SY", 760 },
  { "Taiwan", "TWN", "TW", 158 },
  { "Tajikistan", "TJK", "TJ", 762 },
  { "Tanzania, United Republic of", "TZA", "TZ", 834 },
  { "Thailand", "THA", "TH", 764 },
  { "Timor-Leste", "TLS", "TL", 626 },
  { "Togo", "TGO", "TG", 768 },
  { "Tokelau", "TKL", "TK", 772 },
  { "Tonga", "TON", "TO", 776 },
  { "Trinidad and Tobago", "TTO", "TT", 780 },
  { "Tunisia", "TUN", "TN", 788 },
  { "Turkey", "TUR", "TR", 792 },
  { "Turkmenistan", "TKM", "TM", 795 },
  { "Turks and Caicos Islands", "TCA", "TC", 796 },
  { "Tuvalu", "TUV", "TV", 798 },
  { "Uganda", "UGA", "UG", 800 },
  { "Ukraine", "UKR", "UA", 804 },
  { "United Arab Emirates", "ARE", "AE", 784 },
  { "United Kingdom", "GBR", "GB", 826 },
  { "United States", "USA", "US", 840 },
  { "United States Minor Outlying Islands", "UMI", "UM", 581 },
  { "Uruguay", "URY", "UY", 858 },
  { "Uzbekistan", "UZB", "UZ", 860 },
  { "Vanuatu", "VUT", "VU", 548 },
  { "Venezuela", "VEN", "VE", 862 },
  { "Viet Nam", "VNM", "VN", 704 },
  { "Virgin Islands, British", "VGB", "VG", 92 },
  { "Virgin Islands, U.S.", "VIR", "VI", 850 },
  { "Wallis and Futuna", "WLF", "WF", 876 },
  { "Western Sahara", "ESH", "EH", 732 },
  { "Yemen", "YEM", "YE", 887 },
  { "Zambia", "ZMB", "ZM", 894 },
  { "Zimbabwe", "ZWE", "ZW", 716 },
  { 0, 0, 0, 0 }
};

///////////////////////////////////////////////////////////////////////////////
// Language codes

int find_iso6391(char a1, char a2)
{
  a1 = (char)tolower(a1);
  a2 = (char)tolower(a2);

  int i = 0;
  while (iso_langs[i].name)
  {
    if (iso_langs[i].iso6391)
      if (a1 == iso_langs[i].iso6391[0] && 
          a2 == iso_langs[i].iso6391[1])
        return i;
    i++;
  }
  return -1;
}

int find_iso6392(char a1, char a2, char a3)
{
  a1 = (char)tolower(a1);
  a2 = (char)tolower(a2);
  a3 = (char)tolower(a3);

  int i = 0;
  while (iso_langs[i].name)
  {
    if (iso_langs[i].iso6392)
      if (a1 == iso_langs[i].iso6392[0] && 
          a2 == iso_langs[i].iso6392[1] &&
          a3 == iso_langs[i].iso6392[2])
        return i;
    i++;
  }
  return -1;
}

int lang_index(const char *_code)
{
  if (_code == 0) return -1;
  if (_code[0] == 0) return -1;
  if (_code[1] == 0) return -1;
  if (_code[2] == 0) return find_iso6391(_code[0], _code[1]);
  if (_code[3] == 0) return find_iso6392(_code[0], _code[1], _code[2]);
  return -1;
}

const char *lang_by_code(const char *_code)
{
  int i = lang_index(_code);
  return i == -1? 0: iso_langs[i].name;
}

const char *lang_by_index(int index)
{
  if (index >= 0 && index < array_size(iso_langs))
    return iso_langs[index].name;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Country codes

int find_country_alpha3(char a1, char a2, char a3)
{
  a1 = (char) toupper(a1);
  a2 = (char) toupper(a2);
  a3 = (char) toupper(a3);

  int i = 0;
  while (iso_countries[i].name)
  {
    if (iso_countries[i].alpha3)
      if (a1 == iso_countries[i].alpha3[0] && 
          a2 == iso_countries[i].alpha3[1] &&
          a3 == iso_countries[i].alpha3[2])
        return i;
    i++;
  }
  return -1;
}

int find_country_alpha2(char a1, char a2)
{
  a1 = (char) toupper(a1);
  a2 = (char) toupper(a2);

  int i = 0;
  while (iso_countries[i].name)
  {
    if (iso_countries[i].alpha2)
      if (a1 == iso_countries[i].alpha2[0] && 
          a2 == iso_countries[i].alpha2[1])
        return i;
    i++;
  }
  return -1;
}

int country_index(const char *_code)
{
  if (_code == 0) return -1;
  if (_code[0] == 0) return -1;
  if (_code[1] == 0) return -1;
  if (_code[2] == 0) return find_country_alpha2(_code[0], _code[1]);
  if (_code[3] == 0) return find_country_alpha3(_code[0], _code[1], _code[2]);
  return -1;
}

const char *country_by_code(const char *_code)
{
  int i = country_index(_code);
  return i == -1? 0: iso_countries[i].name;
}

const char *country_by_index(int index)
{
  if (index >= 0 && index < array_size(iso_countries))
    return iso_countries[index].name;
  return 0;
}
