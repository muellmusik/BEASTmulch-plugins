/*
 BEASTmulch UGens
 Copyright (C) 2009 Scott Wilson
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA 
 
 http://www.beast.bham.ac.uk/research/mulch.shtml
 beastmulch-info@contacts.bham.ac.uk
 
 The BEASTmulch project was supported by a grant from the Arts and Humanities Research Council of the UK: http://www.ahrc.ac.uk
*/

/*
 SuperCollider real time audio synthesis system
 Copyright (c) 2002 James McCartney. All rights reserved.
 http://www.audiosynth.com
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "BEASTmulchPV.h"

static InterfaceTable *ft;

struct PV_Decorrelate : Unit
{
	int m_numbins;
	float m_prevtrig, *m_shift;
	bool m_triggered;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

//	void FFT_MaxSize(sc_msg_iter *msg);

	void PV_Decorrelate_Ctor(PV_Decorrelate *unit);
	void PV_Decorrelate_Dtor(PV_Decorrelate *unit);
	void PV_Decorrelate_next(PV_Decorrelate *unit, int inNumSamples);

}

//////////////////////////////////////////////////////////////////////////////////////////////////



SCPolarBuf* ToPolarApx(SndBuf *buf);
SCPolarBuf* ToPolarApx(SndBuf *buf)
{
	if (buf->coord == coord_Complex) {
		SCComplexBuf* p = (SCComplexBuf*)buf->data;
		int numbins = buf->samples - 2 >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToPolarApxInPlace();
		}
		buf->coord = coord_Polar;
	}
	return (SCPolarBuf*)buf->data;
}
//
//SCComplexBuf* ToComplexApx(SndBuf *buf);
//SCComplexBuf* ToComplexApx(SndBuf *buf)
//{
//	if (buf->coord == coord_Polar) {
//		SCPolarBuf* p = (SCPolarBuf*)buf->data;
//		int numbins = buf->samples - 2 >> 1;
//		for (int i=0; i<numbins; ++i) {
//			p->bin[i].ToComplexApxInPlace();
//		}
//		buf->coord = coord_Complex;
//	}
//	return (SCComplexBuf*)buf->data;
//}


/////////////////////////////////////////////////////////////////////////////////////////////

void PV_Decorrelate_choose(PV_Decorrelate* unit);
void PV_Decorrelate_choose(PV_Decorrelate* unit)
{
	RGET
	for (int i=0; i<unit->m_numbins; ++i) {
		unit->m_shift[i] = frand(s1,s2,s3) * twopi;
	}
	RPUT
}

void PV_Decorrelate_next(PV_Decorrelate *unit, int inNumSamples)
{
	float trig = ZIN0(1);
	float scale = ZIN0(2);
	if (trig > 0.f && unit->m_prevtrig <= 0.f) unit->m_triggered = true;
	unit->m_prevtrig = trig;

	PV_GET_BUF
	
	if (!unit->m_shift) {
		unit->m_shift = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_numbins = numbins;
		PV_Decorrelate_choose(unit);
	} else {
		if (numbins != unit->m_numbins) return;
		if (unit->m_triggered) {
			unit->m_triggered = false;
			PV_Decorrelate_choose(unit);
		}
	}

	int n = (int)(ZIN0(1) * numbins);
	n = sc_clip(n, 0, numbins);
	
	SCPolarBuf *p = ToPolarApx(buf);
	
	float *shift = unit->m_shift;
	for (int i=0; i<n; ++i) {
		//printf("phase: %f\n", p->bin[i].phase);
		p->bin[i].phase += (shift[i] * scale);
	}
	
}


void PV_Decorrelate_Ctor(PV_Decorrelate* unit)
{
	SETCALC(PV_Decorrelate_next);
	ZOUT0(0) = ZIN0(0);
	unit->m_shift = 0;
	unit->m_prevtrig = 0.f;
	unit->m_triggered = false;
}

void PV_Decorrelate_Dtor(PV_Decorrelate* unit)
{
	RTFree(unit->mWorld, unit->m_shift);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void init_SCComplex(InterfaceTable *inTable);

//void load(InterfaceTable *inTable)
PluginLoad(BEASTmulchPV)
{
	ft = inTable;
	init_SCComplex(inTable);
	DefineDtorUnit(PV_Decorrelate);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
