=========
AC3Filter
=========

AC3Filter is high quality DirectShow audio decoder and processor filter
focused on flexible audio controls during movie playback.

Main features:
* Decode AC3/DTS/MPEG Audio Layer I/II formats
* MPEG1/2 PES demultiplex (for DVD support)
* PES/LPCM format support
* AVI/AC3, AVI/DTS, WAV/AC3 and WAV/DTS support
* AC3/DTS/MPEG Audio SPDIF passthrough support
* AC3 encode to output any multichannel source to SPDIF
* Detailed input bitstream format information
* Automatic Gain Control
* Dynamic Range Compression
* Per-channel delays
* Online output configuraion change
* Upmix from any stereo source to 3-6 channels
* Downmix to DolbySurround/ProLogic/ProLogic2
* Per-channel gain for all input/output channels
* Direct access to mixing matrix
* Input/output levels indication
  
More information you can find in files:
doc/ac3filter_rus.html (russian)
doc/ac3filter_eng.html (english)
doc/ac3filter_ita.html (italian)

Copyright (c) 2002-2004 by Alexander Vigovsky (xvalex@mail.ru)


License:
========

Distributed under GNU General Public License version 2.
You may find it in GNU_eng.txt at english language and GNU_rus.txt at russian 
language. Russain language version is for information purpose only and english 
version have priority with all variant reading.

This application may solely be used for demonstration and educational purposes. 
Any other use may be prohibited by law in some coutries. The author has no 
liability regarding this application whatsoever. This application may be 
distributed freely unless prohibited by law.

This product distributed in hope it may be useful, but without any warranty; 
without even the implied warranty of merchantability or fitness for a 
particular purpose and compliance with any standards. I do not guarantee 
any support, bug correction, repair of lost data, I am not responsible 
for broken hardware and lost working time. And I am not responsible for 
legality of reproduced with this program multimedia production.


Compilation:
============

To compile this project at least VC++ 6.0 is required with PlatformSDK or 
DirectX SDK installed (at least one of them).

Include path should have this lines first:

...\...SDK\Samples\Multimedia\DirectShow\BaseClasses
...\...SDK\include

Required libraries are strmbase.lib (release) and strmbasd.lib (debug) in
libraries path. If it is not shipped with SDK you can compile this project:
...\...SDK\Samples\Multimedia\DirectShow\BaseClasses\baseclasses.dsw
(read MSDN about how to setup environment to use DirectShow Base Classes)

It also require VALib library, available at 
http://sourceforge.net/project/showfiles.php?group_id=66022

AC3Filter project has dependency on the valib library project so 
by default directories should be configured as follows:

...\AC3Filter     - ac3filter project
...\VALib
...\VALib\lib     - valib library project
...\VALib\valib   - valib include & source files

You may simply checkout both modules from CVS to the same root folder.
CVS root: :pserver:anonymous@cvs.sourceforge.net:/cvsroot/ac3filter
Modules:  AC3Filter, VALib

If you want to alter default layout:
1) compile valib.lib
2) specify valib.lib in ac3filter's linked libraries
3) specify include path to the root of valib includes


Links:
======

http://ac3filter.sourceforge.net             - Project home page
http://sourceforge.net/projects/ac3filter    - Sourceforge project page
http://sourceforge.net/forum/?group_id=66022 - Forum


Donate:
=======



Contact Author: 
===============

mailto:xvalex@mail.ru?Subject=AC3Filter

Please write only in English or Russian. Subject should have 'AC3Filter' 
word otherwise it may be accidentally deleted with tons of other junk mail.
