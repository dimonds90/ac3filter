#ifndef __CSSscramble_h_
#define __CSSscramble_h_

extern void CSSdisckey(unsigned char *dkey,unsigned char *pkey);
extern void CSStitlekey(unsigned char *tkey,unsigned char *dkey);
extern void CSSdescramble(unsigned char *sector,unsigned char *tkey);

#endif
