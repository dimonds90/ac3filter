/* 
 *	Copyright (C) Gabest - December 2002
 *
 *  CDeCSSInputPin is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  CDeCSSInputPin is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */

#include <streams.h>
#include <dvdmedia.h>
#include "DeCSSInputPin.h"

#include "CSSauth.h"
#include "CSSscramble.h"


//
// CDeCSSInputPin
//

static BYTE g_PlayerKeys[][6] = 
{
	// from mplayer:
	{0x01, 0xaf, 0xe3, 0x12, 0x80},
	{0x12, 0x11, 0xca, 0x04, 0x3b},
	{0x14, 0x0c, 0x9e, 0xd0, 0x09},
	{0x14, 0x71, 0x35, 0xba, 0xe2},
	{0x1a, 0xa4, 0x33, 0x21, 0xa6},
	{0x26, 0xec, 0xc4, 0xa7, 0x4e},
	{0x2c, 0xb2, 0xc1, 0x09, 0xee},
	{0x2f, 0x25, 0x9e, 0x96, 0xdd},
	{0x33, 0x2f, 0x49, 0x6c, 0xe0},
	{0x35, 0x5b, 0xc1, 0x31, 0x0f},
	{0x36, 0x67, 0xb2, 0xe3, 0x85},
	{0x39, 0x3d, 0xf1, 0xf1, 0xbd},
	{0x3b, 0x31, 0x34, 0x0d, 0x91},
	{0x45, 0xed, 0x28, 0xeb, 0xd3},
	{0x48, 0xb7, 0x6c, 0xce, 0x69},
	{0x4b, 0x65, 0x0d, 0xc1, 0xee},
	{0x4c, 0xbb, 0xf5, 0x5b, 0x23},
	{0x51, 0x67, 0x67, 0xc5, 0xe0},
	{0x53, 0x94, 0xe1, 0x75, 0xbf},
	{0x57, 0x2c, 0x8b, 0x31, 0xae},
	{0x63, 0xdb, 0x4c, 0x5b, 0x4a},
	{0x7b, 0x1e, 0x5e, 0x2b, 0x57},
	{0x85, 0xf3, 0x85, 0xa0, 0xe0},
	{0xab, 0x1e, 0xe7, 0x7b, 0x72},
	{0xab, 0x36, 0xe3, 0xeb, 0x76},
	{0xb1, 0xb8, 0xf9, 0x38, 0x03},
	{0xb8, 0x5d, 0xd8, 0x53, 0xbd},
	{0xbf, 0x92, 0xc3, 0xb0, 0xe2},
	{0xcf, 0x1a, 0xb2, 0xf8, 0x0a},
	{0xec, 0xa0, 0xcf, 0xb3, 0xff},
	{0xfc, 0x95, 0xa9, 0x87, 0x35}
	// TODO: find more player keys
};

#define g_nPlayerKeys (sizeof(g_PlayerKeys)/sizeof(g_PlayerKeys[0]))

CDeCSSInputPin::CDeCSSInputPin(CTransformFilter* pFilter, HRESULT* phr)
	: CTransformInputPin(NAME("CDeCSSInputPin"), pFilter, phr, L"In")
{
	m_varient = -1;
	memset(m_Challenge, 0, sizeof(m_Challenge));
	memset(m_KeyCheck, 0, sizeof(m_KeyCheck));
	memset(m_DiscKey, 0, sizeof(m_DiscKey));
	memset(m_TitleKey, 0, sizeof(m_TitleKey));
}

STDMETHODIMP CDeCSSInputPin::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	return
		riid == __uuidof(IKsPropertySet) ? GetInterface((IKsPropertySet*)this, ppv) :
		CTransformInputPin::NonDelegatingQueryInterface(riid, ppv);
}

// IMemInputPin

STDMETHODIMP CDeCSSInputPin::Receive(IMediaSample* pSample)
{
	if(m_mt.majortype == MEDIATYPE_DVD_ENCRYPTED_PACK
	&& pSample->GetActualDataLength() == 2048)
	{
		BYTE* pBuffer = NULL;
		if(SUCCEEDED(pSample->GetPointer(&pBuffer)) && (pBuffer[0x14]&0x10))
		{
			CSSdescramble(pBuffer, m_TitleKey);
			pBuffer[0x14] &= ~0x10;

			IMediaSample2 *pMS2;
			if(SUCCEEDED(pSample->QueryInterface(IID_IMediaSample2, (void**) &pMS2)))
			{
				AM_SAMPLE2_PROPERTIES props;
				memset(&props, 0, sizeof(props));
				if(SUCCEEDED(pMS2->GetProperties(sizeof(props), (BYTE*)&props))
				&& (props.dwTypeSpecificFlags & AM_UseNewCSSKey))
				{
					props.dwTypeSpecificFlags &= ~AM_UseNewCSSKey; // no idea if it's useful to clear but can't hurt since we are outputting decrypted PES always
					pMS2->SetProperties(sizeof(props), (BYTE*)&props);
				}
        pMS2->Release();
			}
		}
	}

	return(CTransformInputPin::Receive(pSample));
}

// IKsPropertySet

STDMETHODIMP CDeCSSInputPin::Set(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength)
{
	int i;
	if(PropSet != AM_KSPROPSETID_CopyProt)
		return E_NOTIMPL;

	switch(Id)
	{
	case AM_PROPERTY_COPY_MACROVISION:
		break;
	case AM_PROPERTY_DVDCOPY_CHLG_KEY: // 3. auth: receive drive nonce word, also store and encrypt the buskey made up of the two nonce words
		{
			AM_DVDCOPY_CHLGKEY* pChlgKey = (AM_DVDCOPY_CHLGKEY*)pPropertyData;
			for(i = 0; i < 10; i++)
				m_Challenge[i] = pChlgKey->ChlgKey[9-i];

			CSSkey2(m_varient, m_Challenge, &m_Key[5]);

			CSSbuskey(m_varient, m_Key, m_KeyCheck);
		}
		break;
	case AM_PROPERTY_DVDCOPY_DISC_KEY: // 5. receive the disckey
		{
			AM_DVDCOPY_DISCKEY* pDiscKey = (AM_DVDCOPY_DISCKEY*)pPropertyData; // pDiscKey->DiscKey holds the disckey encrypted with itself and the 408 disckeys encrypted with the playerkeys

			bool fSuccess = false;

			for(int j = 0; j < g_nPlayerKeys; j++)
			{
				for(int k = 1; k < 409; k++)
				{
					BYTE DiscKey[6];
					for(i = 0; i < 5; i++)
						DiscKey[i] = pDiscKey->DiscKey[k*5+i] ^ m_KeyCheck[4-i];
					DiscKey[5] = 0;

					CSSdisckey(DiscKey, g_PlayerKeys[j]);

					BYTE Hash[6];
					for(i = 0; i < 5; i++)
						Hash[i] = pDiscKey->DiscKey[i] ^ m_KeyCheck[4-i];
					Hash[5] = 0;

					CSSdisckey(Hash, DiscKey);

					if(!memcmp(Hash, DiscKey, 6))
					{
						memcpy(m_DiscKey, DiscKey, 6);
						j = g_nPlayerKeys;
						fSuccess = true;
						break;
					}
				}
			}

			if(!fSuccess)
				return E_FAIL;
		}
		break;
	case AM_PROPERTY_DVDCOPY_DVD_KEY1: // 2. auth: receive our drive-encrypted nonce word and decrypt it for verification
		{
			AM_DVDCOPY_BUSKEY* pKey1 = (AM_DVDCOPY_BUSKEY*)pPropertyData;
			for(i = 0; i < 5; i++)
				m_Key[i] =  pKey1->BusKey[4-i];

			m_varient = -1;

			for(i = 31; i >= 0; i--)
			{
				CSSkey1(i, m_Challenge, m_KeyCheck);

				if(memcmp(m_KeyCheck, &m_Key[0], 5) == 0)
					m_varient = i;
			}
		}
		break;
	case AM_PROPERTY_DVDCOPY_REGION:
		break;
	case AM_PROPERTY_DVDCOPY_SET_COPY_STATE:
		break;
	case AM_PROPERTY_DVDCOPY_TITLE_KEY: // 6. receive the title key and decrypt it with the disc key
		{
			AM_DVDCOPY_TITLEKEY* pTitleKey = (AM_DVDCOPY_TITLEKEY*)pPropertyData;
			for(i = 0; i < 5; i++)
				m_TitleKey[i] = pTitleKey->TitleKey[i] ^ m_KeyCheck[4-i];
			m_TitleKey[5] = 0;
			CSStitlekey(m_TitleKey, m_DiscKey);
		}
		break;
	default:
		return E_PROP_ID_UNSUPPORTED;
	}

	return S_OK;
}

STDMETHODIMP CDeCSSInputPin::Get(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength, ULONG* pBytesReturned)
{
	if(PropSet != AM_KSPROPSETID_CopyProt)
		return E_NOTIMPL;

	switch(Id)
	{
	case AM_PROPERTY_DVDCOPY_CHLG_KEY: // 1. auth: send our nonce word
		{
			AM_DVDCOPY_CHLGKEY* pChlgKey = (AM_DVDCOPY_CHLGKEY*)pPropertyData;
			for(int i = 0; i < 10; i++)
				pChlgKey->ChlgKey[i] = 9 - (m_Challenge[i] = i);
		}
		break;
	case AM_PROPERTY_DVDCOPY_DEC_KEY2: // 4. auth: send back the encrypted drive nonce word to finish the authentication
		{
			AM_DVDCOPY_BUSKEY* pKey2 = (AM_DVDCOPY_BUSKEY*)pPropertyData;
			for(int i = 0; i < 5; i++)
				pKey2->BusKey[4-i] = m_Key[5+i];
		}
		break;
	case AM_PROPERTY_DVDCOPY_REGION:
		{
			DVD_REGION* pRegion = (DVD_REGION*)pPropertyData;
			pRegion->RegionData = 0;
			pRegion->SystemRegion = 0;
			*pBytesReturned = sizeof(DVD_REGION);
		}
		break;
	case AM_PROPERTY_DVDCOPY_SET_COPY_STATE:
		{
			AM_DVDCOPY_SET_COPY_STATE* pState = (AM_DVDCOPY_SET_COPY_STATE*)pPropertyData;
			pState->DVDCopyState = AM_DVDCOPYSTATE_AUTHENTICATION_REQUIRED;
			*pBytesReturned = sizeof(AM_DVDCOPY_SET_COPY_STATE);
		}
		break;
	default:
		return E_PROP_ID_UNSUPPORTED;
	}

	return S_OK;
}

STDMETHODIMP CDeCSSInputPin::QuerySupported(REFGUID PropSet, ULONG Id, ULONG* pTypeSupport)
{
	if(PropSet != AM_KSPROPSETID_CopyProt)
		return E_NOTIMPL;

	switch(Id)
	{
	case AM_PROPERTY_COPY_MACROVISION:
		*pTypeSupport = KSPROPERTY_SUPPORT_SET;
		break;
	case AM_PROPERTY_DVDCOPY_CHLG_KEY:
		*pTypeSupport = KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET;
		break;
	case AM_PROPERTY_DVDCOPY_DEC_KEY2:
		*pTypeSupport = KSPROPERTY_SUPPORT_GET;
		break;
	case AM_PROPERTY_DVDCOPY_DISC_KEY:
		*pTypeSupport = KSPROPERTY_SUPPORT_SET;
		break;
	case AM_PROPERTY_DVDCOPY_DVD_KEY1:
		*pTypeSupport = KSPROPERTY_SUPPORT_SET;
		break;
	case AM_PROPERTY_DVDCOPY_REGION:
		*pTypeSupport = KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET;
		break;
	case AM_PROPERTY_DVDCOPY_SET_COPY_STATE:
		*pTypeSupport = KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET;
		break;
	case AM_PROPERTY_DVDCOPY_TITLE_KEY:
		*pTypeSupport = KSPROPERTY_SUPPORT_SET;
		break;
	default:
		return E_PROP_ID_UNSUPPORTED;
	}
	
	return S_OK;
}
