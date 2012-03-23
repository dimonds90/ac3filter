#ifndef __CSSauth_H__
#define __CSSauth_H__

extern void CSSkey1(int varient, byte const *challenge, byte *key);
extern void CSSkey2(int varient, byte const *challenge, byte *key);
extern void CSSbuskey(int varient, byte const *challenge, byte *key);

#endif