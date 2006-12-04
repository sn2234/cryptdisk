; 
; Copyright (c) 2006, nobody
; All rights reserved.
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
; 
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
; 
; THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
; EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 

.data
	
	mds_log		db	000h, 000h, 001h, 010h, 002h, 020h, 011h, 0CCh, 003h, 0DCh, 021h, 0D7h, 012h, 07Dh, 0CDh, 030h
			db	004h, 040h, 0DDh, 077h, 022h, 099h, 0D8h, 08Dh, 013h, 091h, 07Eh, 0ECh, 0CEh, 0E7h, 031h, 019h
			db	005h, 029h, 041h, 04Ah, 0DEh, 0B6h, 078h, 0F7h, 023h, 026h, 09Ah, 0A1h, 0D9h, 0FCh, 08Eh, 03Dh
			db	014h, 0A4h, 092h, 050h, 07Fh, 087h, 0EDh, 06Bh, 0CFh, 09Dh, 0E8h, 0D3h, 032h, 062h, 01Ah, 0A9h
			db	006h, 0B9h, 02Ah, 058h, 042h, 0AFh, 04Bh, 072h, 0DFh, 0E1h, 0B7h, 0ADh, 079h, 0E3h, 0F8h, 05Eh
			db	024h, 0FAh, 027h, 0B4h, 09Bh, 060h, 0A2h, 085h, 0DAh, 07Bh, 0FDh, 01Eh, 08Fh, 0E5h, 03Eh, 097h
			db	015h, 02Ch, 0A5h, 039h, 093h, 05Ah, 051h, 0C2h, 080h, 008h, 088h, 066h, 0EEh, 0BBh, 06Ch, 0C6h
			db	0D0h, 04Dh, 09Eh, 047h, 0E9h, 074h, 0D4h, 00Dh, 033h, 044h, 063h, 036h, 01Bh, 0B1h, 0AAh, 055h
			db	007h, 065h, 0BAh, 0C5h, 02Bh, 038h, 059h, 0C1h, 043h, 035h, 0B0h, 054h, 04Ch, 046h, 073h, 00Ch
			db	0E0h, 0ACh, 0E2h, 05Dh, 0B8h, 057h, 0AEh, 071h, 07Ah, 01Dh, 0E4h, 096h, 0F9h, 0B3h, 05Fh, 084h
			db	025h, 0A0h, 0FBh, 03Ch, 028h, 049h, 0B5h, 0F6h, 09Ch, 0D2h, 061h, 0A8h, 0A3h, 04Fh, 086h, 06Ah
			db	0DBh, 0D6h, 07Ch, 02Fh, 0FEh, 00Fh, 01Fh, 0CBh, 090h, 0EBh, 0E6h, 018h, 03Fh, 076h, 098h, 08Ch
			db	016h, 08Ah, 02Dh, 0C9h, 0A6h, 068h, 03Ah, 0F4h, 094h, 082h, 05Bh, 06Fh, 052h, 00Ah, 0C3h, 0BFh
			db	081h, 06Eh, 009h, 0BEh, 089h, 0C8h, 067h, 0F3h, 0EFh, 0F0h, 0BCh, 0F1h, 06Dh, 0BDh, 0C7h, 0F2h
			db	0D1h, 0A7h, 04Eh, 069h, 09Fh, 03Bh, 048h, 0F5h, 0EAh, 017h, 075h, 08Bh, 0D5h, 02Eh, 00Eh, 0CAh
			db	034h, 053h, 045h, 00Bh, 064h, 0C4h, 037h, 0C0h, 01Ch, 095h, 0B2h, 083h, 0ABh, 05Ch, 056h, 070h

	mds_alog	db	001h, 002h, 004h, 008h, 010h, 020h, 040h, 080h, 069h, 0D2h, 0CDh, 0F3h, 08Fh, 077h, 0EEh, 0B5h
			db	003h, 006h, 00Ch, 018h, 030h, 060h, 0C0h, 0E9h, 0BBh, 01Fh, 03Eh, 07Ch, 0F8h, 099h, 05Bh, 0B6h
			db	005h, 00Ah, 014h, 028h, 050h, 0A0h, 029h, 052h, 0A4h, 021h, 042h, 084h, 061h, 0C2h, 0EDh, 0B3h
			db	00Fh, 01Eh, 03Ch, 078h, 0F0h, 089h, 07Bh, 0F6h, 085h, 063h, 0C6h, 0E5h, 0A3h, 02Fh, 05Eh, 0BCh
			db	011h, 022h, 044h, 088h, 079h, 0F2h, 08Dh, 073h, 0E6h, 0A5h, 023h, 046h, 08Ch, 071h, 0E2h, 0ADh
			db	033h, 066h, 0CCh, 0F1h, 08Bh, 07Fh, 0FEh, 095h, 043h, 086h, 065h, 0CAh, 0FDh, 093h, 04Fh, 09Eh
			db	055h, 0AAh, 03Dh, 07Ah, 0F4h, 081h, 06Bh, 0D6h, 0C5h, 0E3h, 0AFh, 037h, 06Eh, 0DCh, 0D1h, 0CBh
			db	0FFh, 097h, 047h, 08Eh, 075h, 0EAh, 0BDh, 013h, 026h, 04Ch, 098h, 059h, 0B2h, 00Dh, 01Ah, 034h
			db	068h, 0D0h, 0C9h, 0FBh, 09Fh, 057h, 0AEh, 035h, 06Ah, 0D4h, 0C1h, 0EBh, 0BFh, 017h, 02Eh, 05Ch
			db	0B8h, 019h, 032h, 064h, 0C8h, 0F9h, 09Bh, 05Fh, 0BEh, 015h, 02Ah, 054h, 0A8h, 039h, 072h, 0E4h
			db	0A1h, 02Bh, 056h, 0ACh, 031h, 062h, 0C4h, 0E1h, 0ABh, 03Fh, 07Eh, 0FCh, 091h, 04Bh, 096h, 045h
			db	08Ah, 07Dh, 0FAh, 09Dh, 053h, 0A6h, 025h, 04Ah, 094h, 041h, 082h, 06Dh, 0DAh, 0DDh, 0D3h, 0CFh
			db	0F7h, 087h, 067h, 0CEh, 0F5h, 083h, 06Fh, 0DEh, 0D5h, 0C3h, 0EFh, 0B7h, 007h, 00Eh, 01Ch, 038h
			db	070h, 0E0h, 0A9h, 03Bh, 076h, 0ECh, 0B1h, 00Bh, 016h, 02Ch, 058h, 0B0h, 009h, 012h, 024h, 048h
			db	090h, 049h, 092h, 04Dh, 09Ah, 05Dh, 0BAh, 01Dh, 03Ah, 074h, 0E8h, 0B9h, 01Bh, 036h, 06Ch, 0D8h
			db	0D9h, 0DBh, 0DFh, 0D7h, 0C7h, 0E7h, 0A7h, 027h, 04Eh, 09Ch, 051h, 0A2h, 02Dh, 05Ah, 0B4h, 000h

	rs_log		db	000h, 000h, 001h, 017h, 002h, 02Eh, 018h, 053h, 003h, 06Ah, 02Fh, 093h, 019h, 034h, 054h, 045h
			db	004h, 05Ch, 06Bh, 0B6h, 030h, 0A6h, 094h, 04Bh, 01Ah, 08Ch, 035h, 081h, 055h, 0AAh, 046h, 00Dh
			db	005h, 024h, 05Dh, 087h, 06Ch, 09Bh, 0B7h, 0C1h, 031h, 02Bh, 0A7h, 0A3h, 095h, 098h, 04Ch, 0CAh
			db	01Bh, 0E6h, 08Dh, 073h, 036h, 0CDh, 082h, 012h, 056h, 062h, 0ABh, 0F0h, 047h, 04Fh, 00Eh, 0BDh
			db	006h, 0D4h, 025h, 0D2h, 05Eh, 027h, 088h, 066h, 06Dh, 0D6h, 09Ch, 079h, 0B8h, 008h, 0C2h, 0DFh
			db	032h, 068h, 02Ch, 0FDh, 0A8h, 08Ah, 0A4h, 05Ah, 096h, 029h, 099h, 022h, 04Dh, 060h, 0CBh, 0E4h
			db	01Ch, 07Bh, 0E7h, 03Bh, 08Eh, 09Eh, 074h, 0F4h, 037h, 0D8h, 0CEh, 0F9h, 083h, 06Fh, 013h, 0B2h
			db	057h, 0E1h, 063h, 0DCh, 0ACh, 0C4h, 0F1h, 0AFh, 048h, 00Ah, 050h, 042h, 00Fh, 0BAh, 0BEh, 0C7h
			db	007h, 0DEh, 0D5h, 078h, 026h, 065h, 0D3h, 0D1h, 05Fh, 0E3h, 028h, 021h, 089h, 059h, 067h, 0FCh
			db	06Eh, 0B1h, 0D7h, 0F8h, 09Dh, 0F3h, 07Ah, 03Ah, 0B9h, 0C6h, 009h, 041h, 0C3h, 0AEh, 0E0h, 0DBh
			db	033h, 044h, 069h, 092h, 02Dh, 052h, 0FEh, 016h, 0A9h, 00Ch, 08Bh, 080h, 0A5h, 04Ah, 05Bh, 0B5h
			db	097h, 0C9h, 02Ah, 0A2h, 09Ah, 0C0h, 023h, 086h, 04Eh, 0BCh, 061h, 0EFh, 0CCh, 011h, 0E5h, 072h
			db	01Dh, 03Dh, 07Ch, 0EBh, 0E8h, 0E9h, 03Ch, 0EAh, 08Fh, 07Dh, 09Fh, 0ECh, 075h, 01Eh, 0F5h, 03Eh
			db	038h, 0F6h, 0D9h, 03Fh, 0CFh, 076h, 0FAh, 01Fh, 084h, 0A0h, 070h, 0EDh, 014h, 090h, 0B3h, 07Eh
			db	058h, 0FBh, 0E2h, 020h, 064h, 0D0h, 0DDh, 077h, 0ADh, 0DAh, 0C5h, 040h, 0F2h, 039h, 0B0h, 0F7h
			db	049h, 0B4h, 00Bh, 07Fh, 051h, 015h, 043h, 091h, 010h, 071h, 0BBh, 0EEh, 0BFh, 085h, 0C8h, 0A1h

	rs_alog		db	001h, 002h, 004h, 008h, 010h, 020h, 040h, 080h, 04Dh, 09Ah, 079h, 0F2h, 0A9h, 01Fh, 03Eh, 07Ch
			db	0F8h, 0BDh, 037h, 06Eh, 0DCh, 0F5h, 0A7h, 003h, 006h, 00Ch, 018h, 030h, 060h, 0C0h, 0CDh, 0D7h
			db	0E3h, 08Bh, 05Bh, 0B6h, 021h, 042h, 084h, 045h, 08Ah, 059h, 0B2h, 029h, 052h, 0A4h, 005h, 00Ah
			db	014h, 028h, 050h, 0A0h, 00Dh, 01Ah, 034h, 068h, 0D0h, 0EDh, 097h, 063h, 0C6h, 0C1h, 0CFh, 0D3h
			db	0EBh, 09Bh, 07Bh, 0F6h, 0A1h, 00Fh, 01Eh, 03Ch, 078h, 0F0h, 0ADh, 017h, 02Eh, 05Ch, 0B8h, 03Dh
			db	07Ah, 0F4h, 0A5h, 007h, 00Eh, 01Ch, 038h, 070h, 0E0h, 08Dh, 057h, 0AEh, 011h, 022h, 044h, 088h
			db	05Dh, 0BAh, 039h, 072h, 0E4h, 085h, 047h, 08Eh, 051h, 0A2h, 009h, 012h, 024h, 048h, 090h, 06Dh
			db	0DAh, 0F9h, 0BFh, 033h, 066h, 0CCh, 0D5h, 0E7h, 083h, 04Bh, 096h, 061h, 0C2h, 0C9h, 0DFh, 0F3h
			db	0ABh, 01Bh, 036h, 06Ch, 0D8h, 0FDh, 0B7h, 023h, 046h, 08Ch, 055h, 0AAh, 019h, 032h, 064h, 0C8h
			db	0DDh, 0F7h, 0A3h, 00Bh, 016h, 02Ch, 058h, 0B0h, 02Dh, 05Ah, 0B4h, 025h, 04Ah, 094h, 065h, 0CAh
			db	0D9h, 0FFh, 0B3h, 02Bh, 056h, 0ACh, 015h, 02Ah, 054h, 0A8h, 01Dh, 03Ah, 074h, 0E8h, 09Dh, 077h
			db	0EEh, 091h, 06Fh, 0DEh, 0F1h, 0AFh, 013h, 026h, 04Ch, 098h, 07Dh, 0FAh, 0B9h, 03Fh, 07Eh, 0FCh
			db	0B5h, 027h, 04Eh, 09Ch, 075h, 0EAh, 099h, 07Fh, 0FEh, 0B1h, 02Fh, 05Eh, 0BCh, 035h, 06Ah, 0D4h
			db	0E5h, 087h, 043h, 086h, 041h, 082h, 049h, 092h, 069h, 0D2h, 0E9h, 09Fh, 073h, 0E6h, 081h, 04Fh
			db	09Eh, 071h, 0E2h, 089h, 05Fh, 0BEh, 031h, 062h, 0C4h, 0C5h, 0C7h, 0C3h, 0CBh, 0DBh, 0FBh, 0BBh
			db	03Bh, 076h, 0ECh, 095h, 067h, 0CEh, 0D1h, 0EFh, 093h, 06Bh, 0D6h, 0E1h, 08Fh, 053h, 0A6h, 000h

	q0		db	0A9h, 067h, 0B3h, 0E8h, 004h, 0FDh, 0A3h, 076h, 09Ah, 092h, 080h, 078h, 0E4h, 0DDh, 0D1h, 038h
			db	00Dh, 0C6h, 035h, 098h, 018h, 0F7h, 0ECh, 06Ch, 043h, 075h, 037h, 026h, 0FAh, 013h, 094h, 048h
			db	0F2h, 0D0h, 08Bh, 030h, 084h, 054h, 0DFh, 023h, 019h, 05Bh, 03Dh, 059h, 0F3h, 0AEh, 0A2h, 082h
			db	063h, 001h, 083h, 02Eh, 0D9h, 051h, 09Bh, 07Ch, 0A6h, 0EBh, 0A5h, 0BEh, 016h, 00Ch, 0E3h, 061h
			db	0C0h, 08Ch, 03Ah, 0F5h, 073h, 02Ch, 025h, 00Bh, 0BBh, 04Eh, 089h, 06Bh, 053h, 06Ah, 0B4h, 0F1h
			db	0E1h, 0E6h, 0BDh, 045h, 0E2h, 0F4h, 0B6h, 066h, 0CCh, 095h, 003h, 056h, 0D4h, 01Ch, 01Eh, 0D7h
			db	0FBh, 0C3h, 08Eh, 0B5h, 0E9h, 0CFh, 0BFh, 0BAh, 0EAh, 077h, 039h, 0AFh, 033h, 0C9h, 062h, 071h
			db	081h, 079h, 009h, 0ADh, 024h, 0CDh, 0F9h, 0D8h, 0E5h, 0C5h, 0B9h, 04Dh, 044h, 008h, 086h, 0E7h
			db	0A1h, 01Dh, 0AAh, 0EDh, 006h, 070h, 0B2h, 0D2h, 041h, 07Bh, 0A0h, 011h, 031h, 0C2h, 027h, 090h
			db	020h, 0F6h, 060h, 0FFh, 096h, 05Ch, 0B1h, 0ABh, 09Eh, 09Ch, 052h, 01Bh, 05Fh, 093h, 00Ah, 0EFh
			db	091h, 085h, 049h, 0EEh, 02Dh, 04Fh, 08Fh, 03Bh, 047h, 087h, 06Dh, 046h, 0D6h, 03Eh, 069h, 064h
			db	02Ah, 0CEh, 0CBh, 02Fh, 0FCh, 097h, 005h, 07Ah, 0ACh, 07Fh, 0D5h, 01Ah, 04Bh, 00Eh, 0A7h, 05Ah
			db	028h, 014h, 03Fh, 029h, 088h, 03Ch, 04Ch, 002h, 0B8h, 0DAh, 0B0h, 017h, 055h, 01Fh, 08Ah, 07Dh
			db	057h, 0C7h, 08Dh, 074h, 0B7h, 0C4h, 09Fh, 072h, 07Eh, 015h, 022h, 012h, 058h, 007h, 099h, 034h
			db	06Eh, 050h, 0DEh, 068h, 065h, 0BCh, 0DBh, 0F8h, 0C8h, 0A8h, 02Bh, 040h, 0DCh, 0FEh, 032h, 0A4h
			db	0CAh, 010h, 021h, 0F0h, 0D3h, 05Dh, 00Fh, 000h, 06Fh, 09Dh, 036h, 042h, 04Ah, 05Eh, 0C1h, 0E0h

	q1		db	075h, 0F3h, 0C6h, 0F4h, 0DBh, 07Bh, 0FBh, 0C8h, 04Ah, 0D3h, 0E6h, 06Bh, 045h, 07Dh, 0E8h, 04Bh
			db	0D6h, 032h, 0D8h, 0FDh, 037h, 071h, 0F1h, 0E1h, 030h, 00Fh, 0F8h, 01Bh, 087h, 0FAh, 006h, 03Fh
			db	05Eh, 0BAh, 0AEh, 05Bh, 08Ah, 000h, 0BCh, 09Dh, 06Dh, 0C1h, 0B1h, 00Eh, 080h, 05Dh, 0D2h, 0D5h
			db	0A0h, 084h, 007h, 014h, 0B5h, 090h, 02Ch, 0A3h, 0B2h, 073h, 04Ch, 054h, 092h, 074h, 036h, 051h
			db	038h, 0B0h, 0BDh, 05Ah, 0FCh, 060h, 062h, 096h, 06Ch, 042h, 0F7h, 010h, 07Ch, 028h, 027h, 08Ch
			db	013h, 095h, 09Ch, 0C7h, 024h, 046h, 03Bh, 070h, 0CAh, 0E3h, 085h, 0CBh, 011h, 0D0h, 093h, 0B8h
			db	0A6h, 083h, 020h, 0FFh, 09Fh, 077h, 0C3h, 0CCh, 003h, 06Fh, 008h, 0BFh, 040h, 0E7h, 02Bh, 0E2h
			db	079h, 00Ch, 0AAh, 082h, 041h, 03Ah, 0EAh, 0B9h, 0E4h, 09Ah, 0A4h, 097h, 07Eh, 0DAh, 07Ah, 017h
			db	066h, 094h, 0A1h, 01Dh, 03Dh, 0F0h, 0DEh, 0B3h, 00Bh, 072h, 0A7h, 01Ch, 0EFh, 0D1h, 053h, 03Eh
			db	08Fh, 033h, 026h, 05Fh, 0ECh, 076h, 02Ah, 049h, 081h, 088h, 0EEh, 021h, 0C4h, 01Ah, 0EBh, 0D9h
			db	0C5h, 039h, 099h, 0CDh, 0ADh, 031h, 08Bh, 001h, 018h, 023h, 0DDh, 01Fh, 04Eh, 02Dh, 0F9h, 048h
			db	04Fh, 0F2h, 065h, 08Eh, 078h, 05Ch, 058h, 019h, 08Dh, 0E5h, 098h, 057h, 067h, 07Fh, 005h, 064h
			db	0AFh, 063h, 0B6h, 0FEh, 0F5h, 0B7h, 03Ch, 0A5h, 0CEh, 0E9h, 068h, 044h, 0E0h, 04Dh, 043h, 069h
			db	029h, 02Eh, 0ACh, 015h, 059h, 0A8h, 00Ah, 09Eh, 06Eh, 047h, 0DFh, 034h, 035h, 06Ah, 0CFh, 0DCh
			db	022h, 0C9h, 0C0h, 09Bh, 089h, 0D4h, 0EDh, 0ABh, 012h, 0A2h, 00Dh, 052h, 0BBh, 002h, 02Fh, 0A9h
			db	0D7h, 061h, 01Eh, 0B4h, 050h, 004h, 0F6h, 0C2h, 016h, 025h, 086h, 056h, 055h, 009h, 0BEh, 091h
