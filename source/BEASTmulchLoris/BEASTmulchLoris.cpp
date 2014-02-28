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
 
 Loris UGens adapted from Loris 1.5

 Loris is Copyright (c) 1999-2007 by Kelly Fitz and Lippold Haken
 
 Loris is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY, without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 file COPYING or the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 loris@cerlsoundgroup.org
 http://www.cerlsoundgroup.org/Loris/
 
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

#include <limits.h>
#include "SC_PlugIn.h"
#include <vecLib/vDSP.h>
#include "fastInverseSqrt.h"



//////////////////////////////////////////////////////////////////////////////////////////////////

// macros to put rgen state in registers
#define RGET \
RGen& rgen = *unit->mParent->mRGen; \
uint32 s1 = rgen.s1; \
uint32 s2 = rgen.s2; \
uint32 s3 = rgen.s3; 

#define RPUT \
rgen.s1 = s1; \
rgen.s2 = s2; \
rgen.s3 = s3;

//////////////////////////////////////////////////////////////////////////////////////////////////


// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct BufUnit : public Unit
{
	SndBuf *m_buf;
	float m_fbufnum;
};

struct TableLookup : public BufUnit
{
	double m_cpstoinc, m_radtoinc;
	int32 mTableSize;
	int32 m_lomask;
};

struct BEOsc : public TableLookup
{
	int32 m_phase;
	float m_phasein;
	//float mLevel; // for BrownNoise
	float m_x1, m_x2, m_x3; // MA
	float m_y1, m_y2, m_y3; // AR
};

struct LorisPhaseGen : public Unit
{
	double m_a1, m_a2, m_b1, m_y1, m_y2, m_grow, m_level, m_endLevel;
	int m_counter, m_stage, m_shape, m_releaseNode;
	float m_prevGate;
	bool m_released;
};

//struct BERingz : public Unit
//{
//	float m_y1, m_y2, m_b1, m_b2, m_freq, m_decayTime;
//	float m_x1, m_x2, m_x3; // for 4 pt avg
//};

struct FastSqrt : public Unit
{
};
	
struct LP4PAv : public Unit
{
	float m_x1, m_x2, m_x3;
};

struct LP4Noise : public Unit
{
	float m_x1, m_x2, m_x3;
};

struct LorisMod : public Unit
{
	float m_x1, m_x2, m_x3;
};

struct LorisBW : public Unit {};

struct ZapGremlins : public Unit {};

// declare unit generator functions 
extern "C"
{
	void load(InterfaceTable *inTable);
	
	void BEOsc_next_kkk(BEOsc *unit, int inNumSamples);
	void BEOsc_next_kka(BEOsc *unit, int inNumSamples);
	void vBEOsc_next_kkk(BEOsc *unit, int inNumSamples);
	void BEOsc_next_kaa(BEOsc *unit, int inNumSamples);
	void BEOsc_next_kak(BEOsc *unit, int inNumSamples);
	void BEOsc_next_aka(BEOsc *unit, int inNumSamples);
	void BEOsc_next_akk(BEOsc *unit, int inNumSamples);
	void BEOsc_next_aak(BEOsc *unit, int inNumSamples);
	void BEOsc_next_aaa(BEOsc *unit, int inNumSamples);
	void BEOsc_Ctor(BEOsc* unit);
	
	void LorisPhaseGen_next_aa(LorisPhaseGen *unit, int inNumSamples);
	void LorisPhaseGen_next_ak(LorisPhaseGen *unit, int inNumSamples);
	void LorisPhaseGen_next_k(LorisPhaseGen *unit, int inNumSamples);
	void LorisPhaseGen_Ctor(LorisPhaseGen *unit);
	
//	void BERingz_next(BERingz *unit, int inNumSamples);
//	void BERingz_Ctor(BERingz* unit);
	
	void LP4PAv_Ctor(LP4PAv* unit);
	void LP4PAv_next(LP4PAv* unit, int inNumSamples);
	
	void FastSqrt_Ctor(FastSqrt* unit);
	void FastSqrt_next_a(FastSqrt* unit, int inNumSamples);
	void vFastSqrt_next_a(FastSqrt* unit, int inNumSamples);
	void FastSqrt_next_k(FastSqrt* unit, int inNumSamples);
	
	void LP4Noise_Ctor(LP4Noise* unit);
	void LP4Noise_next(LP4Noise* unit, int inNumSamples);
	
	void LorisMod_Ctor(LorisMod* unit);
	void LorisMod_next(LorisMod* unit, int inNumSamples);
	
	void LorisBW_Ctor(LorisBW* unit);
	void LorisBW_next(LorisBW* unit, int inNumSamples);
	
	void ZapGremlins_Ctor(ZapGremlins* unit);
	void ZapGremlins_next(ZapGremlins* unit, int inNumSamples);
	
};

//////////////////////////////////////////////////////////////////


///* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
// Command line: /www/usr/fisher/helpers/mkfilter -Ch -1.0000000000e+00 -Lp -o 3 -a 1.1337868481e-02 0.0000000000e+00 -l */
//
//#define NZEROS 3
//#define NPOLES 3
//#define GAIN   4.663939207e+04
//
//static float xv[NZEROS+1], yv[NPOLES+1];
//
//static void filterloop()
//{ for (;;)
//{ xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; 
//	xv[3] = `next input value' / GAIN;
//	yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; 
//	yv[3] =   (xv[0] + xv[3]) + 3 * (xv[1] + xv[2])
//	+ (  0.9320209047 * yv[0]) + ( -2.8580608588 * yv[1])
//	+ (  2.9258684253 * yv[2]);
//	`next output value' = yv[3];
//}
//}

void BEOsc_Ctor(BEOsc* unit)
{
	
	int tableSize2 = ft->mSineSize;
	unit->m_phasein = ZIN0(1);
	
	if(unit->m_phasein == -INFINITY) unit->m_phasein = 0; // ignore special value
	unit->m_radtoinc = tableSize2 * (rtwopi * 65536.); 
	unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
	unit->m_lomask = (tableSize2 - 1) << 3; 
	
	//if (INRATE(2) == calc_FullRate) Print("Audio bw\n");
	if (INRATE(0) == calc_FullRate) {	// freq audio rate
		if (INRATE(1) == calc_FullRate) {	// freq and phase audio rate
			if (INRATE(2) == calc_FullRate) {
				//Print("next_aaa\n");
				SETCALC(BEOsc_next_aaa); // ar bandwidth
				unit->m_phase = 0;
			} else {
				//Print("next_aak\n");
				SETCALC(BEOsc_next_aak); // kr bandwidth
				unit->m_phase = 0;
			}
		} else {						// freq audio phase control or scalar
			if (INRATE(2) == calc_FullRate) {
				//Print("next_aka\n");
				SETCALC(BEOsc_next_aka);	// ar bandwidth
				unit->m_phase = 0;
			} else {
				//Print("next_akk\n");
				SETCALC(BEOsc_next_akk);	// kr bandwidth
				unit->m_phase = 0;
			}
		}
	} else {							
		if (INRATE(1) == calc_FullRate) {	// freq control or scalar, phase audio
			if (INRATE(2) == calc_FullRate) {
				//Print("next_kaa\n");
				SETCALC(BEOsc_next_kaa); // ar bandwidth
				unit->m_phase = 0;
			} else {
				//Print("next_kak\n");
				SETCALC(BEOsc_next_kak); // kr bandwidth
				unit->m_phase = 0;
			}
				
		} else {
			if (INRATE(2) == calc_FullRate) { // freq and phase control, bw audio
				//Print("next_kka\n");
				SETCALC(BEOsc_next_kka);
			} else {
#if __VEC__
				if (USEVEC) {					// freq and phase control use vec
					//Print("next_kkk VEC\n");
					SETCALC(vBEOsc_next_kkk);
				} else {						// freq and phase control no vec
					//Print("next_kkk\n");
					SETCALC(BEOsc_next_kkk);
				}
#else
				//Print("next_kkk\n");
				SETCALC(BEOsc_next_kkk);	// freq and phase control no vec
#endif
			}
			unit->m_phase = (int32)(unit->m_phasein * unit->m_radtoinc);
		}
	}
	
	unit->m_x1 = 0;
	unit->m_x2 = 0;
	unit->m_x3 = 0;
	unit->m_y1 = 0;
	unit->m_y2 = 0;
	unit->m_y3 = 0;
	
	BEOsc_next_kkk(unit, 1);
}


//////////////////////////////////////////////////////////////////

void BEOsc_next_kkk(BEOsc *unit, int inNumSamples) // freq phase bw control or scalar
{
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float phasein = ZIN0(1);
	float bwin = ZIN0(2);
	
	if(phasein == INFINITY){ // this partial is done
		//Print("Partial Done\n");
		SETCALC(ClearUnitOutputs);
	} else if(phasein == -INFINITY){ // ignore special phase value
		phasein = unit->m_phasein;
	}
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	int32 phaseinc = freq + (int32)(CALCSLOPE(phasein, unit->m_phasein) * unit->m_radtoinc);
	unit->m_phasein = phasein;
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float y3 = unit->m_y3;
	
	RGET
	
	float bw1, bw2, mod; 
	
	// bw coefficients
	bw1 = FastScalarSqrt( 1.f - bwin );
	bw2 = FastScalarSqrt( 2.f * bwin );
	
	LOOP(inNumSamples,
		 //noise
		 x0 = x1; x1 = x2; x2 = x3;
		 x3 = frand2(s1, s2, s3) * 0.00012864661681256f; // kelly uses 6. / GAIN
		 y0 = y1; y1 = y2; y2 = y3;
		 y3 = mod = (x0 + x3) + (3 * (x1 + x2)) + (0.9320209047f * y0) + (-2.8580608588f * y1) + (2.9258684253f * y2);
		 
		 ZXP(out) = lookupi1(table0, table1, phase, lomask)  * (bw1 + ( mod * bw2 ));
		 phase += phaseinc;
		 );
	unit->m_phase = phase;
	
	RPUT
	
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	
	unit->m_y1 = y1;
	unit->m_y2 = y2;
	unit->m_y3 = y3;
	
}

#if __VEC__

void vBEOsc_next_kkk(BEOsc *unit, int inNumSamples) // freq and phase control or scalar use vec
{
	define_vzero
	vfloat32 *vout = (vfloat32*)OUT(0);
	float freqin = ZIN0(0);
	float phasein = ZIN0(1);
	
	if(phasein == INFINITY){ // this partial is done
		//Print("Partial Done\n");
		SETCALC(ClearUnitOutputs);
	} else if(phasein == -INFINITY){ // ignore special phase value
		phasein = unit->m_phasein;
	}
	
	float bwin = ZIN0(2);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	int32 phaseinc = freq + (int32)(CALCSLOPE(phasein, unit->m_phasein) * unit->m_radtoinc);
	unit->m_phasein = phasein;
	
	vint32 vphase = vload(phase, phase+phaseinc, phase+2*phaseinc, phase+3*phaseinc);
	vint32 vphaseinc = vload(phaseinc << 2);
	vint32 v3F800000 = (vint32)vinit(0x3F800000);
	vint32 v007FFF80 = (vint32)vinit(0x007FFF80);
	vint32 vlomask = vload(lomask);
	vuint32 vxlobits1 = (vuint32)vinit(xlobits1);
	vuint32 v7 = (vuint32)vinit(7);
	
	vint32 vtable0 = vload((int32)table0); // assuming 32 bit pointers
	vint32 vtable1 = vload((int32)table1); // assuming 32 bit pointers
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float y3 = unit->m_y3;
	
	//vfloat32 vbw = vload(bw);
	vfloat32 bw1, bw2; 
	
	// bw coefficients
	bw1 = vload(FastScalarSqrt( 1.f - bwin ));
	bw2 = vload(FastScalarSqrt( 2.f * bwin ));
	
	// * (bw1 + ( mod * bw2 ))
	RGET
	
	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		
		vec_union mod;
		for(int j = 0; j < 4; j++) {
			x0 = x1; x1 = x2; x2 = x3;
			x3 = frand2(s1, s2, s3) * 0.00012864661681256f; // kelly uses 6. / GAIN
			y0 = y1; y1 = y2; y2 = y3;
			mod.f[j] = y3 = (x0 + x3) + (3 * (x1 + x2)) + (0.9320209047f * y0) + (-2.8580608588f * y1) + (2.9258684253f * y2);
		}
		
		vfloat32 noise = vec_madd(bw2, mod.vf, bw1);
		
		vfloat32 vfrac = (vfloat32)(vec_or(v3F800000, vec_and(v007FFF80, vec_sl(vphase, v7))));
		vint32 vindex = vec_and(vec_sr(vphase, vxlobits1), vlomask);
		vec_union vaddr0, vaddr1;
		vaddr0.vi = vec_add(vindex, vtable0);
		vaddr1.vi = vec_add(vindex, vtable1);
		
		vec_union vval1, vval2;
		vval1.f[0] = *(float*)(vaddr0.i[0]);
		vval2.f[0] = *(float*)(vaddr1.i[0]);
		vval1.f[1] = *(float*)(vaddr0.i[1]);
		vval2.f[1] = *(float*)(vaddr1.i[1]);
		vval1.f[2] = *(float*)(vaddr0.i[2]);
		vval2.f[2] = *(float*)(vaddr1.i[2]);
		vval1.f[3] = *(float*)(vaddr0.i[3]);
		vval2.f[3] = *(float*)(vaddr1.i[3]);
		
		vfloat32 result = vec_mul(vec_madd(vval2.vf, vfrac, vval1.vf), noise);
		vec_st(result, i, vout);
		if(vec_any_nan(result)) Print("NaN detected in vBEOsc_next_kkk\n");
		
		vphase = vec_add(vphase, vphaseinc);

	}
	unit->m_phase = phase + inNumSamples * phaseinc;
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	
	unit->m_y1 = y1;
	unit->m_y2 = y2;
	unit->m_y3 = y3;
	
	RPUT
	
}

#endif

void BEOsc_next_kka(BEOsc *unit, int inNumSamples) // freq and phase control or scalar
{
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float phasein = ZIN0(1);
	float *bwin = ZIN(2);
	
	if(phasein == INFINITY){ // this partial is done
		//Print("Partial Done\n");
		SETCALC(ClearUnitOutputs);
	} else if(phasein == -INFINITY){ // ignore special phase value
		phasein = unit->m_phasein;
	}
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	int32 phaseinc = freq + (int32)(CALCSLOPE(phasein, unit->m_phasein) * unit->m_radtoinc);
	unit->m_phasein = phasein;
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float y3 = unit->m_y3;
	
	RGET
	
	float mod;
	float bw;
	float thisPhaseIn;
	
	LOOP(inNumSamples,
		 //noise
		 x0 = x1; x1 = x2; x2 = x3;
		 x3 = frand2(s1, s2, s3) * 0.00012864661681256f; // kelly uses 6. / GAIN
		 y0 = y1; y1 = y2; y2 = y3;
		 y3 = mod = (x0 + x3) + (3 * (x1 + x2)) + (0.9320209047f * y0) + (-2.8580608588f * y1) + (2.9258684253f * y2);
		 bw = ZXP(bwin);
		 
		 ZXP(out) = lookupi1(table0, table1, phase, lomask)  * (FastScalarSqrt( 1.f - bw) + ( mod * FastScalarSqrt( 2.f * bw ) ));
		 phase += phaseinc;
		 );
	unit->m_phase = phase;
	
	RPUT
	
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	
	unit->m_y1 = y1;
	unit->m_y2 = y2;
	unit->m_y3 = y3;
	
}

void BEOsc_next_kaa(BEOsc *unit, int inNumSamples) // freq control or scalar phase and bw audio
{
	
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float *phasein = ZIN(1);
	float *bwin = ZIN(2);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	float radtoinc = unit->m_radtoinc;
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float y3 = unit->m_y3;
	
	RGET
	
	float mod;
	float bw;
	float thisPhaseIn;
	float oldPhaseIn = unit->m_phasein;
	
	int32 phaseoffset;
	
	//Print("BEOsc_next_ka %d %g %d\n", inNumSamples, radtoinc, phase);
	
	LOOP(inNumSamples, 
		 
		 thisPhaseIn = ZXP(phasein);
		 if(thisPhaseIn == INFINITY){ // this partial is done
			//Print("Partial Done\n");
			SETCALC(ClearUnitOutputs);
		 } else if(thisPhaseIn == -INFINITY){ // in this case only increment by freq
			phaseoffset = phase; 
		 } else if(oldPhaseIn == -INFINITY){ // in this case reset phase
			phaseoffset = phase = thisPhaseIn;
		 } else {	// in this plain ar phase
			phaseoffset = phase + (int32)(radtoinc * thisPhaseIn);
		 }
		 oldPhaseIn = thisPhaseIn;
		 
		 //noise
		 x0 = x1; x1 = x2; x2 = x3;
		 x3 = frand2(s1, s2, s3) * 0.00012864661681256f; // kelly uses 6. / GAIN
		 y0 = y1; y1 = y2; y2 = y3;
		 y3 = mod = (x0 + x3) + (3 * (x1 + x2)) + (0.9320209047f * y0) + (-2.8580608588f * y1) + (2.9258684253f * y2);
		 bw = ZXP(bwin);
		 
		 ZXP(out) = lookupi1(table0, table1, phaseoffset, lomask) * (FastScalarSqrt( 1.f - bw ) + ( mod * FastScalarSqrt( 2.f * bw ) ));
		 phase += freq;
		 )
		
	unit->m_phase = phase;
	unit->m_phasein = thisPhaseIn;
	
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	
	unit->m_y1 = y1;
	unit->m_y2 = y2;
	unit->m_y3 = y3;
	
	RPUT
	
}

void BEOsc_next_kak(BEOsc *unit, int inNumSamples) // freq control or scalar, phase audio, bw control or scalar
{
	
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float *phasein = ZIN(1);
	float bwin = ZIN0(2);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	float radtoinc = unit->m_radtoinc;
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float y3 = unit->m_y3;
	
	float thisPhaseIn;
	float oldPhaseIn = unit->m_phasein;
	int32 phaseoffset;
	
	float bw1, bw2; 
	
	// bw coefficients
	bw1 = FastScalarSqrt( 1.f - bwin );
	bw2 = FastScalarSqrt( 2.f * bwin );
	
	RGET
		
	float mod;
	
	LOOP(inNumSamples, 
		 thisPhaseIn = ZXP(phasein);
		 if(thisPhaseIn == INFINITY){ // this partial is done
			//Print("Partial Done\n");
			SETCALC(ClearUnitOutputs);
		 } else if(thisPhaseIn == -INFINITY){ // in this case only increment by freq
			phaseoffset = phase; 
		 } else if(oldPhaseIn == -INFINITY){ // in this case reset phase
			phaseoffset = phase = thisPhaseIn;
		 } else {	// in this plain ar phase
			phaseoffset = phase + (int32)(radtoinc * thisPhaseIn);
		 }
		 oldPhaseIn = thisPhaseIn;
		 //noise
		 x0 = x1; x1 = x2; x2 = x3;
		 x3 = frand2(s1, s2, s3) * 0.00012864661681256f; // kelly uses 6. / GAIN
		 y0 = y1; y1 = y2; y2 = y3;
		 y3 = mod = (x0 + x3) + (3 * (x1 + x2)) + (0.9320209047f * y0) + (-2.8580608588f * y1) + (2.9258684253f * y2);
		 
		 ZXP(out) = lookupi1(table0, table1, phaseoffset, lomask) * (bw1 + ( mod * bw2 ));
		 phase += freq;
		 )
	
	
	unit->m_phase = phase;
	unit->m_phasein = thisPhaseIn;
	//unit->mLevel = mod;
	
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	
	unit->m_y1 = y1;
	unit->m_y2 = y2;
	unit->m_y3 = y3;
	
	RPUT
		
}

void BEOsc_next_aaa(BEOsc *unit, int inNumSamples) // freq, phase, bw audio
{
	float *out = ZOUT(0);
	float *freqin = ZIN(0);
	float *phasein = ZIN(1);
	float *bwin = ZIN(2);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	float cpstoinc = unit->m_cpstoinc;
	float radtoinc = unit->m_radtoinc;
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float y3 = unit->m_y3;
	
	float thisPhaseIn;
	float oldPhaseIn = unit->m_phasein;
	int32 phaseoffset;
	
	RGET
	
	float mod;
	float bw;
	//Print("BEOsc_next_aa %d %g %g %d\n", inNumSamples, cpstoinc, radtoinc, phase);
	
	LOOP(inNumSamples,
		 thisPhaseIn = ZXP(phasein);
		 if(thisPhaseIn == INFINITY){ // this partial is done
			//Print("Partial Done\n");
			SETCALC(ClearUnitOutputs);
		 } else if(thisPhaseIn == -INFINITY){ // in this case only increment by freq
			phaseoffset = phase; 
		 } else if(oldPhaseIn == -INFINITY){ // else in this case reset phase (this phase != -inf because we already checked)
		 //Print("Phase Reset\n");
			phaseoffset = phase = thisPhaseIn;
		 } else {	// in this case plain ar phase
			phaseoffset = phase + (int32)(radtoinc * thisPhaseIn);
		 }
		 oldPhaseIn = thisPhaseIn;
		 //noise
		 x0 = x1; x1 = x2; x2 = x3;
		 x3 = frand2(s1, s2, s3) * 0.00012864661681256f; // kelly uses 6. / GAIN
		 y0 = y1; y1 = y2; y2 = y3;
		 y3 = mod = (x0 + x3) + (3 * (x1 + x2)) + (0.9320209047f * y0) + (-2.8580608588f * y1) + (2.9258684253f * y2);
		 
		 bw = ZXP(bwin);
		 
		 float z = lookupi1(table0, table1, phaseoffset, lomask) * (FastScalarSqrt( 1.f - bw ) + ( mod * FastScalarSqrt( 2.f * bw ) ));
		 phase += (int32)(cpstoinc * ZXP(freqin));
		 ZXP(out) = z;
		 );
	
	unit->m_phase = phase;
	unit->m_phasein = thisPhaseIn;
	//unit->mLevel = mod;
	RPUT
		
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	
	unit->m_y1 = y1;
	unit->m_y2 = y2;
	unit->m_y3 = y3;
	
}

void BEOsc_next_aak(BEOsc *unit, int inNumSamples) // freq, phase audio, bw ctrl or scalar
{
	float *out = ZOUT(0);
	float *freqin = ZIN(0);
	float *phasein = ZIN(1);
	float bwin = ZIN0(2);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	float cpstoinc = unit->m_cpstoinc;
	float radtoinc = unit->m_radtoinc;
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float y3 = unit->m_y3;
	
	float thisPhaseIn;
	float oldPhaseIn = unit->m_phasein;
	int32 phaseoffset;
	
	RGET
	
	float bw1, bw2, mod; 
	
	// bw coefficients
	bw1 = FastScalarSqrt( 1.f - bwin );
	bw2 = FastScalarSqrt( 2.f * bwin );
	
	//Print("BEOsc_next_aa %d %g %g %d\n", inNumSamples, cpstoinc, radtoinc, phase);

	LOOP(inNumSamples,
		 thisPhaseIn = ZXP(phasein);
		 if(thisPhaseIn == INFINITY){ // this partial is done
			//Print("Partial Done\n");
			SETCALC(ClearUnitOutputs);
		 } else if(thisPhaseIn == -INFINITY){ // in this case only increment by freq
			phaseoffset = phase; 
		 } else if(oldPhaseIn == -INFINITY){ // in this case reset phase
			phaseoffset = phase = thisPhaseIn;
		 } else {	// in this plain ar phase
			phaseoffset = phase + (int32)(radtoinc * thisPhaseIn);
		 }
		 oldPhaseIn = thisPhaseIn;
		 //noise
		 x0 = x1; x1 = x2; x2 = x3;
		 x3 = frand2(s1, s2, s3) * 0.00012864661681256f; // kelly uses 6. / GAIN
		 y0 = y1; y1 = y2; y2 = y3;
		 y3 = mod = (x0 + x3) + (3 * (x1 + x2)) + (0.9320209047f * y0) + (-2.8580608588f * y1) + (2.9258684253f * y2);
		 
		 float z = lookupi1(table0, table1, phaseoffset, lomask) * (bw1 + ( mod * bw2 ));
		 phase += (int32)(cpstoinc * ZXP(freqin));
		 ZXP(out) = z;
		 );
	
	unit->m_phase = phase;
	unit->m_phasein = thisPhaseIn;
	
	RPUT
		
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	
	unit->m_y1 = y1;
	unit->m_y2 = y2;
	unit->m_y3 = y3;
	
}


void BEOsc_next_aka(BEOsc *unit, int inNumSamples) // freq audio phase control or scalar, audio bw
{
	
	float *out = ZOUT(0);
	float *freqin = ZIN(0);
	float phasein = ZIN0(1);
	float *bwin = ZIN(2);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	float cpstoinc = unit->m_cpstoinc;
	float radtoinc = unit->m_radtoinc;
	float phasemod = unit->m_phasein;
	if(phasein == INFINITY){ // this partial is done
		//Print("Partial Done\n");
		SETCALC(ClearUnitOutputs);
	} else if(phasein == -INFINITY){ // ignore special phase value
		phasein = phasemod;
	}
	float phaseslope = CALCSLOPE(phasein, phasemod);
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float y3 = unit->m_y3;
	
	float z, bw, mod;
	float final;
	int32 pphase;
	
	RGET
	
	LOOP(inNumSamples,
		 
		 x0 = x1; x1 = x2; x2 = x3;
		 x3 = frand2(s1, s2, s3) * 0.00012864661681256f; // kelly uses 6. / GAIN
		 y0 = y1; y1 = y2; y2 = y3;
		 y3 = mod = (x0 + x3) + (3 * (x1 + x2)) + (0.9320209047f * y0) + (-2.8580608588f * y1) + (2.9258684253f * y2);
		 bw = ZXP(bwin);
		 pphase = phase + (int32)(radtoinc * phasemod);
		 phasemod += phaseslope;
		 z = lookupi1(table0, table1, pphase, lomask);
		 phase += (int32)(cpstoinc * ZXP(freqin));
		 final = z * (FastScalarSqrt( 1.f - bw) + ( mod * FastScalarSqrt( 2.f * bw ) ));
		 ZXP(out) = final;
		 )
	
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	
	unit->m_y1 = y1;
	unit->m_y2 = y2;
	unit->m_y3 = y3;
	
	RPUT
	
	unit->m_phase = phase;
	unit->m_phasein = phasein;
}

// control rate bw
void BEOsc_next_akk(BEOsc *unit, int inNumSamples) // freq audio phase control or scalar, control bw
{
	
	float *out = ZOUT(0);
	float *freqin = ZIN(0);
	float phasein = ZIN0(1);
	float bwin = ZIN0(2);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	float cpstoinc = unit->m_cpstoinc;
	float radtoinc = unit->m_radtoinc;
	float phasemod = unit->m_phasein;
	if(phasein == INFINITY){ // this partial is done
		//Print("Partial Done\n");
		SETCALC(ClearUnitOutputs);
	} else if(phasein == -INFINITY){ // ignore special phase value
		phasein = phasemod;
	}
	float phaseslope = CALCSLOPE(phasein, phasemod);
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float y3 = unit->m_y3;
	
	float z, mod;
	float bw1, bw2; 
	int32 pphase;
	
	// bw coefficients
	bw1 = FastScalarSqrt( 1.f - bwin );
	bw2 = FastScalarSqrt( 2.f * bwin );
	
	RGET
	
	LOOP(inNumSamples, 
		 //noise
		 x0 = x1; x1 = x2; x2 = x3;
		 x3 = frand2(s1, s2, s3) * 0.00012864661681256f; // kelly uses 6. / GAIN
		 y0 = y1; y1 = y2; y2 = y3;
		 y3 = mod = (x0 + x3) + (3 * (x1 + x2)) + (0.9320209047f * y0) + (-2.8580608588f * y1) + (2.9258684253f * y2);
		 
		 pphase = phase + (int32)(radtoinc * phasemod);
		 phasemod += phaseslope;
		 z = lookupi1(table0, table1, pphase, lomask);
		 phase += (int32)(cpstoinc * ZXP(freqin));
		 //ZXP(out) = z * (sc_sqrt( 1.f - bw) + ( mod * sc_sqrt( 2.f * bw ) ));
		 ZXP(out) = z * (bw1 + ( mod * bw2 ));
		 )
		
	//unit->mLevel = mod;
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	
	unit->m_y1 = y1;
	unit->m_y2 = y2;
	unit->m_y3 = y3;
	
	RPUT
		
		unit->m_phase = phase;
	unit->m_phasein = phasein;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

// LorisPhaseGen
// hastily and messily adapted (with apologies) from EnvGen
// -INFINITY is understood by BEOsc to mean ignore phase input
// This outputs -INFINITY except at phase reset points

enum {
	kEnvGen_gate,
	kEnvGen_levelScale,
	kEnvGen_levelBias,
	kEnvGen_timeScale,
	kEnvGen_doneAction,
	kEnvGen_initLevel,
	kEnvGen_numStages,
	kEnvGen_releaseNode,
	kEnvGen_loopNode,
	// 'kEnvGen_nodeOffset' must always be last
	// if you need to add an arg, put it before this one
	kEnvGen_nodeOffset
};

void LorisPhaseGen_Ctor(LorisPhaseGen *unit)
{
	//Print("LorisPhaseGen_Ctor A\n");
	if (unit->mCalcRate == calc_FullRate) {
		if (INRATE(0) == calc_FullRate) { // gate
			SETCALC(LorisPhaseGen_next_aa);
		} else {
			SETCALC(LorisPhaseGen_next_ak);
		}
	} else {
		SETCALC(LorisPhaseGen_next_k);
	}
	// gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale
	// level0, numstages, releaseNode, loopNode,
	// [level, dur, shape, curve]
	
	unit->m_endLevel = unit->m_level = ZIN0(kEnvGen_initLevel) * ZIN0(kEnvGen_levelScale) + ZIN0(kEnvGen_levelBias);
	unit->m_counter = 0;
	unit->m_stage = 1000000000;
	unit->m_prevGate = 0.f;
	unit->m_released = false;
	unit->m_releaseNode = (int)ZIN0(kEnvGen_releaseNode);
	LorisPhaseGen_next_k(unit, 1);
}

enum {
	shape_Step,
	shape_Linear,
	shape_Exponential,
	shape_Sine,
	shape_Welch,
	shape_Curve,
	shape_Squared,
	shape_Cubed,
	shape_Sustain = 9999
};

void LorisPhaseGen_next_k(LorisPhaseGen *unit, int inNumSamples)
{
	float *out = OUT(0);
	float gate = ZIN0(kEnvGen_gate); 
	//Print("->EnvGen_next_k gate %g\n", gate);
	int counter = unit->m_counter;
	double level = unit->m_level;
	
	if (unit->m_prevGate <= 0. && gate > 0.) { // trigger
		unit->m_stage = -1;
		unit->mDone = false;
		unit->m_released = false;
		counter = 0;
	} else if (gate <= -1.f && unit->m_prevGate > -1.f) {
		// cutoff
		int numstages = (int)ZIN0(kEnvGen_numStages);
		float dur = -gate - 1.f;
		counter  = (int32)(dur * SAMPLERATE);
		counter  = sc_max(1, counter);
		unit->m_stage = numstages;
		unit->m_shape = shape_Linear;
		// first ZIN0 gets the last envelope node's level, then apply levelScale and levelBias
		unit->m_endLevel = ZIN0(unit->mNumInputs - 4) * ZIN0(kEnvGen_levelScale) + ZIN0(kEnvGen_levelBias);
		unit->m_grow = (unit->m_endLevel - level) / counter;
	} else if (unit->m_prevGate > 0.f && gate <= 0.f 
			   && unit->m_releaseNode >= 0 && !unit->m_released) { // release
		counter = 0;
		unit->m_stage = unit->m_releaseNode - 1;
		unit->m_released = true;
	}
	unit->m_prevGate = gate;
	
	
	// gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale
	// level0, numstages, releaseNode, loopNode,
	// [level, dur, shape, curve]
	
	if (counter <= 0) {
		//Print("stage %d rel %d\n", unit->m_stage, (int)ZIN0(kEnvGen_releaseNode));
		int numstages = (int)ZIN0(kEnvGen_numStages);
		
		//Print("stage %d   numstages %d\n", unit->m_stage, numstages);
		if (unit->m_stage+1 >= numstages) { // num stages
			//Print("stage+1 > num stages\n");
			counter = INT_MAX;
			unit->m_shape = 0;
			level = unit->m_endLevel;
			unit->mDone = true;
			int doneAction = (int)ZIN0(kEnvGen_doneAction);
			DoneAction(doneAction, unit);					// we're done
		} else if (unit->m_stage+1 == unit->m_releaseNode && !unit->m_released) { // sustain stage
			int loopNode = (int)ZIN0(kEnvGen_loopNode);
			if (loopNode >= 0 && loopNode < numstages) {
				unit->m_stage = loopNode;
				goto initSegment;
			} else {
				counter = INT_MAX;
				unit->m_shape = shape_Sustain;
				level = unit->m_level; // changed here from m_endLevel ****
			}
			//Print("sustain\n");
		} else {
			unit->m_stage++;
		initSegment:
			//Print("stage %d\n", unit->m_stage);
			//Print("initSegment\n");
			//out = unit->m_level;
			int stageOffset = (unit->m_stage << 2) + kEnvGen_nodeOffset;
			
			if (stageOffset + 4 > unit->mNumInputs) {
				// oops.
				Print("envelope went past end of inputs.\n");
				ClearUnitOutputs(unit, 1);
				NodeEnd(&unit->mParent->mNode);
				return;
			}
			
			// set this at init segment to value of last endLevel
			level = unit->m_endLevel; 
			
			float** envPtr  = unit->mInBuf + stageOffset;
			double endLevel = *envPtr[0] * ZIN0(kEnvGen_levelScale) + ZIN0(kEnvGen_levelBias); // scale levels
			double dur      = *envPtr[1] * ZIN0(kEnvGen_timeScale);
			//unit->m_shape   = (int32)*envPtr[2];
			unit->m_shape   = 1; // always shape_Linear
			double curve    = *envPtr[3];
			unit->m_endLevel = endLevel;
			
			counter  = (int32)(dur * SAMPLERATE);
			counter  = sc_max(1, counter);
			//Print("stageOffset %d   level %g   endLevel %g   dur %g   shape %d   curve %g\n", stageOffset, level, endLevel, dur, unit->m_shape, curve);
			//Print("SAMPLERATE %g\n", SAMPLERATE);
//			if (counter == 1) unit->m_shape = 1; // shape_Linear
//			//Print("new counter = %d  shape = %d\n", counter, unit->m_shape);
//			switch (unit->m_shape) {
//				case shape_Step : {
//					level = endLevel;
//				} break;
//				case shape_Linear : {
//					unit->m_grow = (endLevel - level) / counter;
//					//Print("grow %g\n", unit->m_grow);
//				} break;
//				case shape_Exponential : {
//					unit->m_grow = std::pow(endLevel / level, 1.0 / counter);
//				} break;
//				case shape_Sine : {
//					double w = pi / counter;
//					
//					unit->m_a2 = (endLevel + level) * 0.5;
//					unit->m_b1 = 2. * cos(w);
//					unit->m_y1 = (endLevel - level) * 0.5;
//					unit->m_y2 = unit->m_y1 * sin(pi * 0.5 - w);
//					level = unit->m_a2 - unit->m_y1;
//				} break;
//				case shape_Welch : {
//					double w = (pi * 0.5) / counter;
//					
//					unit->m_b1 = 2. * cos(w);
//					
//					if (endLevel >= level) {
//						unit->m_a2 = level;
//						unit->m_y1 = 0.;
//						unit->m_y2 = -sin(w) * (endLevel - level);
//					} else {
//						unit->m_a2 = endLevel;
//						unit->m_y1 = level - endLevel;
//						unit->m_y2 = cos(w) * (level - endLevel);
//					}
//					level = unit->m_a2 + unit->m_y1;
//				} break;
//				case shape_Curve : {
//					if (fabs(curve) < 0.001) {
//						unit->m_shape = 1; // shape_Linear
//						unit->m_grow = (endLevel - level) / counter;
//					} else {
//						double a1 = (endLevel - level) / (1.0 - exp(curve));	
//						unit->m_a2 = level + a1;
//						unit->m_b1 = a1; 
//						unit->m_grow = exp(curve / counter);
//					}
//				} break;
//				case shape_Squared : {
//					unit->m_y1 = sqrt(level); 
//					unit->m_y2 = sqrt(endLevel); 
//					unit->m_grow = (unit->m_y2 - unit->m_y1) / counter;
//				} break;
//				case shape_Cubed : {
//					unit->m_y1 = std::pow(level, 0.33333333);
//					unit->m_y2 = std::pow(endLevel, 0.33333333);
//					unit->m_grow = (unit->m_y2 - unit->m_y1) / counter;
//				} break;
//			}
		}
	}
	
	
//	switch (unit->m_shape) {
//		case shape_Step : {
//		} break;
//		case shape_Linear : {
//			double grow = unit->m_grow;
//			//Print("level %g\n", level);
//			level += grow;
//		} break;
//		case shape_Exponential : {
//			double grow = unit->m_grow;
//			level *= grow;
//		} break;
//		case shape_Sine : {
//			double a2 = unit->m_a2;
//			double b1 = unit->m_b1;
//			double y2 = unit->m_y2;
//			double y1 = unit->m_y1;
//			double y0 = b1 * y1 - y2; 
//			level = a2 - y0;
//			y2 = y1; 
//			y1 = y0;
//			unit->m_y1 = y1;
//			unit->m_y2 = y2;
//		} break;
//		case shape_Welch : {
//			double a2 = unit->m_a2;
//			double b1 = unit->m_b1;
//			double y2 = unit->m_y2;
//			double y1 = unit->m_y1;
//			double y0 = b1 * y1 - y2; 
//			level = a2 + y0;
//			y2 = y1; 
//			y1 = y0;
//			unit->m_y1 = y1;
//			unit->m_y2 = y2;
//		} break;
//		case shape_Curve : {
//			double a2 = unit->m_a2;
//			double b1 = unit->m_b1;
//			double grow = unit->m_grow;
//			b1 *= grow;
//			level = a2 - b1;
//			unit->m_b1 = b1;
//		} break;
//		case shape_Squared : {
//			double grow = unit->m_grow;
//			double y1 = unit->m_y1;
//			y1 += grow;
//			level = y1*y1;
//			unit->m_y1 = y1;
//		} break;
//		case shape_Cubed : {
//			double grow = unit->m_grow;
//			double y1 = unit->m_y1;
//			y1 += grow;
//			level = y1*y1*y1;
//			unit->m_y1 = y1;
//		} break;
//		case shape_Sustain : {
//		} break;
//	}
	*out = level;
	//Print("x %d %d %d %g\n", unit->m_stage, counter, unit->m_shape, *out);
	unit->m_level = -INFINITY;
	unit->m_counter = counter - 1;
	
}

// control rate gate
void LorisPhaseGen_next_ak(LorisPhaseGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float gate = ZIN0(kEnvGen_gate);
	int counter = unit->m_counter;
	double level = unit->m_level;
	
	if (unit->m_prevGate <= 0. && gate > 0.) { // trigger
		unit->m_stage = -1;
		unit->mDone = false;
		unit->m_released = false;
		counter = 0;
	} else if (gate <= -1.f && unit->m_prevGate > -1.f) {
		// cutoff
		int numstages = (int)ZIN0(kEnvGen_numStages);
		float dur = -gate - 1.f;
		counter  = (int32)(dur * SAMPLERATE);
		counter  = sc_max(1, counter);
		unit->m_stage = numstages;
		unit->m_shape = shape_Linear;
		unit->m_endLevel = ZIN0(unit->mNumInputs - 4) * ZIN0(kEnvGen_levelScale) + ZIN0(kEnvGen_levelBias);
		unit->m_grow = (unit->m_endLevel - level) / counter;
	} else if (unit->m_prevGate > 0.f && gate <= 0.f 
			   && unit->m_releaseNode >= 0 && !unit->m_released) { // release
		counter = 0;
		unit->m_stage = unit->m_releaseNode - 1;
		unit->m_released = true;
	}
	unit->m_prevGate = gate;
	
	int remain = inNumSamples;
	while (remain)
	{
		if (counter == 0) {
			int numstages = (int)ZIN0(kEnvGen_numStages);
			
			if (unit->m_stage+1 >= numstages) { // num stages
				counter = INT_MAX;
				unit->m_shape = 0;
				level = unit->m_endLevel;
				unit->mDone = true;
				int doneAction = (int)ZIN0(kEnvGen_doneAction);
				DoneAction(doneAction, unit);						// we're done
			} else if (unit->m_stage+1 == (int)ZIN0(kEnvGen_releaseNode) && !unit->m_released) { // sustain stage
				int loopNode = (int)ZIN0(kEnvGen_loopNode);
				if (loopNode >= 0 && loopNode < numstages) {
					unit->m_stage = loopNode;
					goto initSegment;
				} else {
					counter = INT_MAX;
					unit->m_shape = shape_Sustain;
					level = unit->m_level; // changed here from m_endLevel ****
				}
			} else {
				unit->m_stage++;
			initSegment:
				int stageOffset = (unit->m_stage << 2) + kEnvGen_nodeOffset;
				
				if (stageOffset + 4 > unit->mNumInputs) {
					// oops.
					Print("envelope went past end of inputs.\n");
					ClearUnitOutputs(unit, 1);
					NodeEnd(&unit->mParent->mNode);
					return;
				}
				
				// set this at init segment to value of last endLevel
				level = unit->m_endLevel; 
				//Print("Phase reset\n");
				
				float** envPtr  = unit->mInBuf + stageOffset;
				double endLevel = *envPtr[0] * ZIN0(kEnvGen_levelScale) + ZIN0(kEnvGen_levelBias); // scale levels
				double dur      = *envPtr[1] * ZIN0(kEnvGen_timeScale);
				//unit->m_shape   = (int32)*envPtr[2];
				unit->m_shape   = 1; // always shape_Linear
				double curve    = *envPtr[3];
				unit->m_endLevel = endLevel;
				
				counter  = (int32)(dur * SAMPLERATE);
				counter  = sc_max(1, counter);
				
//				if (counter == 1) unit->m_shape = 1; // shape_Linear
//				switch (unit->m_shape) {
//					case shape_Step : {
//						level = endLevel;
//					} break;
//					case shape_Linear : {
//						unit->m_grow = (endLevel - level) / counter;
//					} break;
//					case shape_Exponential : {
//						unit->m_grow = std::pow(endLevel / level, 1.0 / counter);
//					} break;
//					case shape_Sine : {
//						double w = pi / counter;
//						
//						unit->m_a2 = (endLevel + level) * 0.5;
//						unit->m_b1 = 2. * cos(w);
//						unit->m_y1 = (endLevel - level) * 0.5;
//						unit->m_y2 = unit->m_y1 * sin(pi * 0.5 - w);
//						level = unit->m_a2 - unit->m_y1;
//					} break;
//					case shape_Welch : {
//						double w = (pi * 0.5) / counter;
//						
//						unit->m_b1 = 2. * cos(w);
//						
//						if (endLevel >= level) {
//							unit->m_a2 = level;
//							unit->m_y1 = 0.;
//							unit->m_y2 = -sin(w) * (endLevel - level);
//						} else {
//							unit->m_a2 = endLevel;
//							unit->m_y1 = level - endLevel;
//							unit->m_y2 = cos(w) * (level - endLevel);
//						}
//						level = unit->m_a2 + unit->m_y1;
//					} break;
//					case shape_Curve : {
//						if (fabs(curve) < 0.001) {
//							unit->m_shape = 1; // shape_Linear
//							unit->m_grow = (endLevel - level) / counter;
//						} else {
//							double a1 = (endLevel - level) / (1.0 - exp(curve));	
//							unit->m_a2 = level + a1;
//							unit->m_b1 = a1; 
//							unit->m_grow = exp(curve / counter);
//						}
//					} break;
//					case shape_Squared : {
//						unit->m_y1 = sqrt(level); 
//						unit->m_y2 = sqrt(endLevel); 
//						unit->m_grow = (unit->m_y2 - unit->m_y1) / counter;
//					} break;
//					case shape_Cubed : {
//						unit->m_y1 = std::pow(level, 0.33333333);
//						unit->m_y2 = std::pow(endLevel, 0.33333333);
//						unit->m_grow = (unit->m_y2 - unit->m_y1) / counter;
//					} break;
//				}
			}
		}
		
		int nsmps = sc_min(remain, counter);
		
		for (int i=0; i<nsmps; ++i) { // much simpler than EnvGen
			ZXP(out) = level;
			level = -INFINITY;
		}
//		switch (unit->m_shape) {
//			case shape_Step : {
//				for (int i=0; i<nsmps; ++i) {
//					ZXP(out) = level;
//				}
//			} break;
//			case shape_Linear : {
//				double grow = unit->m_grow;
//				for (int i=0; i<nsmps; ++i) {
//					ZXP(out) = level;
//					level += grow;
//				}
//			} break;
//			case shape_Exponential : {
//				double grow = unit->m_grow;
//				for (int i=0; i<nsmps; ++i) {
//					ZXP(out) = level;
//					level *= grow;
//				}
//			} break;
//			case shape_Sine : {
//				double a2 = unit->m_a2;
//				double b1 = unit->m_b1;
//				double y2 = unit->m_y2;
//				double y1 = unit->m_y1;
//				for (int i=0; i<nsmps; ++i) {
//					ZXP(out) = level;
//					double y0 = b1 * y1 - y2; 
//					level = a2 - y0;
//					y2 = y1; 
//					y1 = y0;
//				}
//				unit->m_y1 = y1;
//				unit->m_y2 = y2;
//			} break;
//			case shape_Welch : {
//				double a2 = unit->m_a2;
//				double b1 = unit->m_b1;
//				double y2 = unit->m_y2;
//				double y1 = unit->m_y1;
//				for (int i=0; i<nsmps; ++i) {
//					ZXP(out) = level;
//					double y0 = b1 * y1 - y2; 
//					level = a2 + y0;
//					y2 = y1; 
//					y1 = y0;
//				}
//				unit->m_y1 = y1;
//				unit->m_y2 = y2;
//			} break;
//			case shape_Curve : {
//				double a2 = unit->m_a2;
//				double b1 = unit->m_b1;
//				double grow = unit->m_grow;
//				for (int i=0; i<nsmps; ++i) {
//					ZXP(out) = level;
//					b1 *= grow;
//					level = a2 - b1;
//				}
//				unit->m_b1 = b1;
//			} break;
//			case shape_Squared : {
//				double grow = unit->m_grow;
//				double y1 = unit->m_y1;
//				for (int i=0; i<nsmps; ++i) {
//					ZXP(out) = level;
//					y1 += grow;
//					level = y1*y1;
//				}
//				unit->m_y1 = y1;
//			} break;
//			case shape_Cubed : {
//				double grow = unit->m_grow;
//				double y1 = unit->m_y1;
//				for (int i=0; i<nsmps; ++i) {
//					ZXP(out) = level;
//					y1 += grow;
//					level = y1*y1*y1;
//				}
//				unit->m_y1 = y1;
//			} break;
//			case shape_Sustain : {
//				for (int i=0; i<nsmps; ++i) {
//					ZXP(out) = level;
//				}
//			} break;
//		}
		remain -= nsmps;
		counter -= nsmps;
	}
	//Print("x %d %d %d %g\n", unit->m_stage, counter, unit->m_shape, ZOUT0(0));
	unit->m_level = level;
	unit->m_counter = counter;
	
}


#define CHECK_GATE \
prevGate = gate; \
gate = ZXP(gatein); \
if (prevGate <= 0.f && gate > 0.f) { \
gatein--; \
unit->m_stage = -1; \
unit->m_released = false; \
unit->mDone = false; \
counter = i; \
nsmps = i; \
break; \
} else if (gate <= -1.f && unit->m_prevGate > -1.f) { \
int numstages = (int)ZIN0(kEnvGen_numStages); \
float dur = -gate - 1.f; \
gatein--; \
counter  = (int32)(dur * SAMPLERATE); \
counter  = sc_max(1, counter) + i; \
unit->m_stage = numstages; \
unit->m_shape = shape_Linear; \
unit->m_endLevel = ZIN0(unit->mNumInputs - 4) * ZIN0(kEnvGen_levelScale) + ZIN0(kEnvGen_levelBias); \
unit->m_grow = (unit->m_endLevel - level) / counter; \
nsmps = i; \
break; \
} else if (prevGate > 0.f && gate <= 0.f \
&& unit->m_releaseNode >= 0 && !unit->m_released) { \
gatein--; \
counter = i; \
unit->m_stage = unit->m_releaseNode - 1; \
unit->m_released = true; \
nsmps = i; \
break; \
} \

// audio rate trigger
void LorisPhaseGen_next_aa(LorisPhaseGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *gatein = ZIN(kEnvGen_gate);
	int counter = unit->m_counter;
	double level = unit->m_level;
	float gate = 0.;
	float prevGate = unit->m_prevGate;
	int remain = inNumSamples;
	while (remain)
	{
		if (counter == 0) {
			
			int numstages = (int)ZIN0(kEnvGen_numStages);
			
			if (unit->m_stage+1 >= numstages) { // num stages
				counter = INT_MAX;
				unit->m_shape = 0;
				level = unit->m_endLevel;
				unit->mDone = true;
				int doneAction = (int)ZIN0(kEnvGen_doneAction);
				DoneAction(doneAction, unit);						// we're done
			} else if (unit->m_stage+1 == (int)ZIN0(kEnvGen_releaseNode) && !unit->m_released) { // sustain stage
				int loopNode = (int)ZIN0(kEnvGen_loopNode);
				if (loopNode >= 0 && loopNode < numstages) {
					unit->m_stage = loopNode;
					goto initSegment;
				} else {
					counter = INT_MAX;
					unit->m_shape = shape_Sustain;
					level = unit->m_level; // changed here from m_endLevel **** 
				}
			} else {
				unit->m_stage++;
			initSegment:
				int stageOffset = (unit->m_stage << 2) + kEnvGen_nodeOffset;
				
				if (stageOffset + 4 > unit->mNumInputs) {
					// oops.
					Print("envelope went past end of inputs.\n");
					ClearUnitOutputs(unit, 1);
					NodeEnd(&unit->mParent->mNode);
					return;
				}
				
				// set this at init segment to value of last endLevel
				level = unit->m_endLevel; 
				
				float** envPtr  = unit->mInBuf + stageOffset;
				double endLevel = *envPtr[0] * ZIN0(kEnvGen_levelScale) + ZIN0(kEnvGen_levelBias); // scale levels
				double dur      = *envPtr[1] * ZIN0(kEnvGen_timeScale);
				//unit->m_shape   = (int32)*envPtr[2];
				unit->m_shape   = 1; // always shape_Linear
				double curve    = *envPtr[3];
				unit->m_endLevel = endLevel;
				
				counter  = (int32)(dur * SAMPLERATE);
				counter  = sc_max(1, counter);
//				if (counter == 1) unit->m_shape = 1; // shape_Linear
//				switch (unit->m_shape) {
//					case shape_Step : {
//						level = endLevel;
//					} break;
//					case shape_Linear : {
//						unit->m_grow = (endLevel - level) / counter;
//					} break;
//					case shape_Exponential : {
//						unit->m_grow = std::pow(endLevel / level, 1.0 / counter);
//					} break;
//					case shape_Sine : {
//						double w = pi / counter;
//						
//						unit->m_a2 = (endLevel + level) * 0.5;
//						unit->m_b1 = 2. * cos(w);
//						unit->m_y1 = (endLevel - level) * 0.5;
//						unit->m_y2 = unit->m_y1 * sin(pi * 0.5 - w);
//						level = unit->m_a2 - unit->m_y1;
//					} break;
//					case shape_Welch : {
//						double w = (pi * 0.5) / counter;
//						
//						unit->m_b1 = 2. * cos(w);
//						
//						if (endLevel >= level) {
//							unit->m_a2 = level;
//							unit->m_y1 = 0.;
//							unit->m_y2 = -sin(w) * (endLevel - level);
//						} else {
//							unit->m_a2 = endLevel;
//							unit->m_y1 = level - endLevel;
//							unit->m_y2 = cos(w) * (level - endLevel);
//						}
//						level = unit->m_a2 + unit->m_y1;
//					} break;
//					case shape_Curve : {
//						if (fabs(curve) < 0.001) {
//							unit->m_shape = 1; // shape_Linear
//							unit->m_grow = (endLevel - level) / counter;
//						} else {
//							double a1 = (endLevel - level) / (1.0 - exp(curve));	
//							unit->m_a2 = level + a1;
//							unit->m_b1 = a1; 
//							unit->m_grow = exp(curve / counter);
//						}
//					} break;
//					case shape_Squared : {
//						unit->m_y1 = sqrt(level); 
//						unit->m_y2 = sqrt(endLevel); 
//						unit->m_grow = (unit->m_y2 - unit->m_y1) / counter;
//					} break;
//					case shape_Cubed : {
//						unit->m_y1 = std::pow(level, 0.33333333);
//						unit->m_y2 = std::pow(endLevel, 0.33333333);
//						unit->m_grow = (unit->m_y2 - unit->m_y1) / counter;
//					} break;
//				}
			}
		}
		
		int nsmps = sc_min(remain, counter);
		
		for (int i=0; i<nsmps; ++i) { // much simpler than EnvGen
			CHECK_GATE
			ZXP(out) = level;
			level = -INFINITY;
		}
		
//		switch (unit->m_shape) {
//			case shape_Step : {
//				for (int i=0; i<nsmps; ++i) {
//					CHECK_GATE
//					ZXP(out) = level;
//				}
//			} break;
//			case shape_Linear : {
//				double grow = unit->m_grow;
//				for (int i=0; i<nsmps; ++i) {
//					CHECK_GATE
//					ZXP(out) = level;
//					level += grow;
//				}
//			} break;
//			case shape_Exponential : {
//				double grow = unit->m_grow;
//				for (int i=0; i<nsmps; ++i) {
//					CHECK_GATE
//					ZXP(out) = level;
//					level *= grow;
//				}
//			} break;
//			case shape_Sine : {
//				double a2 = unit->m_a2;
//				double b1 = unit->m_b1;
//				double y2 = unit->m_y2;
//				double y1 = unit->m_y1;
//				for (int i=0; i<nsmps; ++i) {
//					CHECK_GATE
//					ZXP(out) = level;
//					double y0 = b1 * y1 - y2; 
//					level = a2 - y0;
//					y2 = y1; 
//					y1 = y0;
//				}
//				unit->m_y1 = y1;
//				unit->m_y2 = y2;
//			} break;
//			case shape_Welch : {
//				double a2 = unit->m_a2;
//				double b1 = unit->m_b1;
//				double y2 = unit->m_y2;
//				double y1 = unit->m_y1;
//				for (int i=0; i<nsmps; ++i) {
//					CHECK_GATE
//					ZXP(out) = level;
//					double y0 = b1 * y1 - y2; 
//					level = a2 + y0;
//					y2 = y1; 
//					y1 = y0;
//				}
//				unit->m_y1 = y1;
//				unit->m_y2 = y2;
//			} break;
//			case shape_Curve : {
//				double a2 = unit->m_a2;
//				double b1 = unit->m_b1;
//				double grow = unit->m_grow;
//				for (int i=0; i<nsmps; ++i) {
//					CHECK_GATE
//					ZXP(out) = level;
//					b1 *= grow;
//					level = a2 - b1;
//				}
//				unit->m_b1 = b1;
//			} break;
//			case shape_Squared : {
//				double grow = unit->m_grow;
//				double y1 = unit->m_y1;
//				for (int i=0; i<nsmps; ++i) {
//					CHECK_GATE
//					ZXP(out) = level;
//					y1 += grow;
//					level = y1*y1;
//				}
//				unit->m_y1 = y1;
//			} break;
//			case shape_Cubed : {
//				double grow = unit->m_grow;
//				double y1 = unit->m_y1;
//				for (int i=0; i<nsmps; ++i) {
//					CHECK_GATE
//					ZXP(out) = level;
//					y1 += grow;
//					level = y1*y1*y1;
//				}
//				unit->m_y1 = y1;
//			} break;
//			case shape_Sustain : {
//				for (int i=0; i<nsmps; ++i) {
//					CHECK_GATE
//					ZXP(out) = level;
//				}
//			} break;
//		}
		remain -= nsmps;
		counter -= nsmps;
	}
	unit->m_level = level;
	unit->m_counter = counter;
	unit->m_prevGate = gate;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//// legacy

void LP4PAv_Ctor(LP4PAv* unit)
{	
	//postbuf("LPZ2_Reset\n");
	SETCALC(LP4PAv_next);
	unit->m_x1 = unit->m_x2 = unit->m_x3 = ZIN0(0);
	ZOUT0(0) = 0.f;
}


void LP4PAv_next(LP4PAv* unit, int inNumSamples)
{
	
	float *out = ZOUT(0);
	float *in = ZIN(0);
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	// unroll by 4
	LOOP(inNumSamples >> 2,
		 x0 = ZXP(in); 
		 ZXP(out) = 0.25f * (x0 + x1 + x2 + x3);
		 x3 = ZXP(in); 
		 ZXP(out) = 0.25f * (x0 + x1 + x2 + x3);
		 x2 = ZXP(in); 
		 ZXP(out) = 0.25f * (x0 + x1 + x2 + x3);
		 x1 = ZXP(in); 
		 ZXP(out) = 0.25f * (x0 + x1 + x2 + x3);
		 );
	// in case of remainder
	LOOP(inNumSamples & 3, 
		 x0 = ZXP(in); 
		 ZXP(out) = 0.25f * (x0 + x1 + x2 + x3);
		 x3 = x2;
		 x2 = x1;
		 x1 = x0;
		 );
	
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void FastSqrt_Ctor(FastSqrt* unit)
{
	if(INRATE(0) == calc_FullRate) {
#if __VEC__
		if(USEVEC) {
			//Print("vec FastSqrt_a\n");
			SETCALC(vFastSqrt_next_a);
		} else {
			//Print("FastSqrt_a\n");
			SETCALC(FastSqrt_next_a);
		}
#else
		//Print("FastSqrt_a\n");
		SETCALC(FastSqrt_next_a);
#endif
	} else {
		//Print("FastSqrt_k\n");
		SETCALC(FastSqrt_next_k);
	}
}

void FastSqrt_next_a(FastSqrt* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	
// scalar slightly faster by 3s	
	LOOP(unit->mRate->mFilterLoops,
		 ZXP(out) = FastScalarSqrt(ZXP(in));
		 ZXP(out) = FastScalarSqrt(ZXP(in));
		 ZXP(out) = FastScalarSqrt(ZXP(in));
		 )
	LOOP(unit->mRate->mFilterRemain,
		 ZXP(out) = FastScalarSqrt(ZXP(in));
		 )	
}

#if __VEC__
void vFastSqrt_next_a(FastSqrt* unit, int inNumSamples)
{
	vfloat32 *vout = (vfloat32*)OUT(0);
	vfloat32 *vin = (vfloat32*)IN(0);
	
	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		// protect against negative numbers
		vec_st(vecSquareRoot( vec_abs(vec_ld(i, vin)) ), i, vout);

	}
}
#endif

void FastSqrt_next_k(FastSqrt* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float sqr = FastScalarSqrt(ZIN0(0));
	
	//checkBadValues(sqr);
	
	LOOP(inNumSamples,
		 ZXP(out) = sqr;
	)	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void LP4Noise_Ctor(LP4Noise* unit)
{	
	//postbuf("LPZ2_Reset\n");
	SETCALC(LP4Noise_next);
	RGET
	unit->m_x1 = frand2(s1, s2, s3);
	unit->m_x2 = frand2(s1, s2, s3);
	unit->m_x3 = frand2(s1, s2, s3);
	RPUT
	ZOUT0(0) = 0.f;
}


void LP4Noise_next(LP4Noise* unit, int inNumSamples)
{
	
	float *out = ZOUT(0);
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float val;
	
	RGET
		
	// unroll by 4
	
	LOOP(inNumSamples >> 2,
		 x0 = frand2(s1, s2, s3); 
		 val = 0.25f * (x0 + x1 + x2 + x3);
		 //checkBadValues(val);
		 ZXP(out) = val;
		 x3 = frand2(s1, s2, s3); 
		 val = 0.25f * (x0 + x1 + x2 + x3);
		 //checkBadValues(val);
		 ZXP(out) = val;
		 x2 = frand2(s1, s2, s3);
		 val = 0.25f * (x0 + x1 + x2 + x3);
		 //checkBadValues(val);
		 ZXP(out) = val;
		 x1 = frand2(s1, s2, s3); 
		 val = 0.25f * (x0 + x1 + x2 + x3);
		 //checkBadValues(val);
		 ZXP(out) = val;
		 );
	// in case of remainder
	LOOP(inNumSamples & 3, 
		 x0 = frand2(s1, s2, s3); 
		 ZXP(out) = 0.25f * (x0 + x1 + x2 + x3);
		 x3 = x2;
		 x2 = x1;
		 x1 = x0;
		 );
	
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	RPUT
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void LorisMod_Ctor(LorisMod* unit)
{	
	SETCALC(LorisMod_next);
	RGET
		unit->m_x1 = frand2(s1, s2, s3);
	unit->m_x2 = frand2(s1, s2, s3);
	unit->m_x3 = frand2(s1, s2, s3);
	RPUT
		ZOUT0(0) = 0.f;
}


void LorisMod_next(LorisMod* unit, int inNumSamples)
{
	
	float *out = ZOUT(0);
	float *bwin = ZIN(0);
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;
	float x3 = unit->m_x3;
	
	float bw;
	
	RGET
		
	// unroll by 4
	LOOP(inNumSamples >> 2,
		 bw = ZXP(bwin);
		 x0 = frand2(s1, s2, s3); 
		 ZXP(out) = (FastScalarSqrt( 1.f - bw) + ( 0.25f * (x0 + x1 + x2 + x3) * FastScalarSqrt( 2.f * bw ) ));
		 bw = ZXP(bwin);
		 x3 = frand2(s1, s2, s3); 
		 ZXP(out) = (FastScalarSqrt( 1.f - bw) + ( 0.25f * (x0 + x1 + x2 + x3) * FastScalarSqrt( 2.f * bw ) ));
		 bw = ZXP(bwin);
		 x2 = frand2(s1, s2, s3);
		 ZXP(out) = (FastScalarSqrt( 1.f - bw) + ( 0.25f * (x0 + x1 + x2 + x3) * FastScalarSqrt( 2.f * bw ) ));
		 bw = ZXP(bwin);
		 x1 = frand2(s1, s2, s3); 
		 ZXP(out) = (FastScalarSqrt( 1.f - bw) + ( 0.25f * (x0 + x1 + x2 + x3) * FastScalarSqrt( 2.f * bw ) ));
		 );
	// in case of remainder
	LOOP(inNumSamples & 3, 
		 bw = ZXP(bwin);
		 x0 = frand2(s1, s2, s3); 
		 ZXP(out) = (FastScalarSqrt( 1.f - bw) + ( 0.25f * (x0 + x1 + x2 + x3) * FastScalarSqrt( 2.f * bw ) ));
		 x3 = x2;
		 x2 = x1;
		 x1 = x0;
		 );
		
	
	unit->m_x1 = x1;
	unit->m_x2 = x2;
	unit->m_x3 = x3;
	RPUT
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void LorisBW_Ctor(LorisBW* unit)
{	
	SETCALC(LorisBW_next);
	//ZOUT0(0) = 0.f;
	LorisBW_next(unit, 1);
}


void LorisBW_next(LorisBW* unit, int inNumSamples)
{
	
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *bwin = ZIN(1);
	
	float bw;

	LOOP(inNumSamples,
		 bw = ZXP(bwin);
		 ZXP(out) = (FastScalarSqrt( 1.f - bw) + ( 0.25f * ZXP(in) * FastScalarSqrt( 2.f * bw ) ));
		 );
}	

////////////////////////////////////////////////////////////////////

void ZapGremlins_Ctor(ZapGremlins* unit)
{
	SETCALC(ZapGremlins_next);
}	

void ZapGremlins_next(ZapGremlins* unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);
	
	LOOP(inNumSamples,
		 ZXP(out) = zapgremlins(ZXP(in));
	);
	
}


////////////////////////////////////////////////////////////////////

// the load function is called by the host when the plug-in is loaded
//void load(InterfaceTable *inTable)
PluginLoad(BEASTmulchLoris)
{
	ft = inTable;
	
	DefineSimpleUnit(BEOsc);
	DefineSimpleUnit(LorisPhaseGen);
	DefineSimpleUnit(LP4PAv);
	DefineSimpleUnit(FastSqrt);
	DefineSimpleUnit(LP4Noise);
	DefineSimpleUnit(LorisMod);
	DefineSimpleUnit(LorisBW);
	DefineSimpleUnit(ZapGremlins);
}


