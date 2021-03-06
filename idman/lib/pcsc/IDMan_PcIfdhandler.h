/*
 * Copyright (c) 2007, 2008 University of Tsukuba
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Tsukuba nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Copyright (c) 1999-2003 David Corcoran <corcoran@linuxnet.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * Changes to this license can be made only by the copyright author with 
 * explicit written consent.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * ID?????????CCID???????????????IFD?????????????????? ????????????\n
 * Athena Smartcard Solutions???????????? ASE-III(USB)?????????????????????????????????????????????
 * \file IDMan_PcIfdhandler.h
 */

#ifndef _ifd_handler_h_
#define _ifd_handler_h_

#include "IDMan_PcPcsclite.h"

/*
 * IFD Handler???????????????????????????
 */

/// ????????????????????????????????????????????????
  typedef struct _SCARD_IO_HEADER {
	DWORD Protocol;		///< ???????????????\n ATR_PROTOCOL_TYPE_T0 or ATR_PROTOCOL_TYPE_T1
	DWORD Length;		///< (?????????)
  } SCARD_IO_HEADER, *PSCARD_IO_HEADER;


/*
 * IFD Handler??????????????????
 */
#define TAG_IFD_ATR					0x0303	///<????????????: ATR(Answer-to-Reset)???????????????
#define TAG_IFD_SIMULTANEOUS_ACCESS	0x0FAF	///<????????????:?????????????????????????????????????????????
#define TAG_IFD_SLOTS_NUMBER		0x0FAE	///<????????????:?????????????????????????????????
#define TAG_IFD_THREAD_SAFE			0x0FAD	///<????????????:??????????????????????????????????????????
#define TAG_IFD_SLOT_THREAD_SAFE	0x0FAC	///<????????????:????????????????????????????????????????????????

#define IFD_POWER_UP			500		///<??????????????????????????????ATR?????????
#define IFD_POWER_DOWN			501		///<????????????????????????????????????
#define IFD_RESET				502		///<????????????????????????

#define IFD_NEGOTIATE_PTS1		1	///<PTS1?????????????????????
#define IFD_NEGOTIATE_PTS2		2	///<PTS2?????????????????????
#define IFD_NEGOTIATE_PTS3		4	///<PTS3?????????????????????


/*
 * IFD Handler?????????????????????
 */
#define IFD_SUCCESS					0		///<????????????
#define IFD_ERROR_TAG				600		///<????????????????????????????????????
#define IFD_ERROR_SET_FAILURE		601
#define IFD_ERROR_VALUE_READ_ONLY	602
#define IFD_ERROR_PTS_FAILURE		605		///<???????????????????????????
#define IFD_ERROR_NOT_SUPPORTED	606
#define IFD_PROTOCOL_NOT_SUPPORTED	607		///<??????????????????????????????????????????
#define IFD_ERROR_POWER_ACTION		608		///<?????????????????????
#define IFD_ERROR_SWALLOW			609
#define IFD_ERROR_EJECT				610
#define IFD_ERROR_CONFISCATE		611
#define IFD_COMMUNICATION_ERROR		612		///<???????????????
#define IFD_RESPONSE_TIMEOUT		613		///<????????????????????????
#define IFD_NOT_SUPPORTED			614		///<???????????????
#define IFD_ICC_PRESENT				615		///<???????????????????????????????????????
#define IFD_ICC_NOT_PRESENT			616		///<???????????????
#define IFD_NO_SUCH_DEVICE			617		///<??????????????????


//????????????PCSC/CCID??????????????????????????????CCID?????????????????????
#define IFD_ICC_PRESENT_POWER_ON	630		///<???????????????????????????????????????


/*
 * IFD??????????????????(IFD handler Ver 2.0)
 */

  RESPONSECODE IFDHCreateChannel( DWORD, DWORD );
  RESPONSECODE IFDHCloseChannel( DWORD );
  RESPONSECODE IFDHGetCapabilities( DWORD, DWORD, PDWORD, PUCHAR );
  RESPONSECODE IFDHSetProtocolParameters( DWORD, DWORD, UCHAR, UCHAR, UCHAR, UCHAR );
  RESPONSECODE IFDHPowerICC( DWORD, DWORD, PUCHAR, PDWORD );
  RESPONSECODE IFDHTransmitToICC( DWORD, SCARD_IO_HEADER, PUCHAR, 
				   DWORD, PUCHAR, PDWORD, PSCARD_IO_HEADER );
  RESPONSECODE IFDHControl( DWORD, PUCHAR, DWORD, PUCHAR, PDWORD );
  RESPONSECODE IFDHICCPresence( DWORD );



#endif /* _ifd_hander_h_ */
