/*
 
 BEASTmulch UGens
 Copyright (C) 2010 Scott Wilson
 
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifdef NOVA_SIMD
#include "simd_memory.hpp"
#endif

#include "SC_PlugIn.h"
#include <cstdio>

const int kMAXMEDIANSIZE = 32;

static InterfaceTable *ft;

struct DelayUnit : public Unit
{
	float *m_dlybuf;

	float m_dsamp, m_fdelaylen;
	float m_delaytime, m_maxdelaytime;
	long m_iwrphase, m_idelaylen, m_mask;
	long m_numoutput;
};

struct DelayN : public DelayUnit
{
};

struct DelayL : public DelayUnit
{
};

struct DelayC : public DelayUnit
{
};

struct FeedbackDelay : public DelayUnit
{
	float m_feedbk, m_decaytime;
};

struct CombN : public FeedbackDelay
{
};

struct CombL : public FeedbackDelay
{
};

struct R1C : public DelayUnit
{
	float m_b1, m_y1;
	float m_feedbk;
};

struct R2C : public DelayUnit
{
	float m_b1, m_y1, m_b1_2, m_y1_2;
	float m_feedbk, m_feedbk2;
	float *m_dlybuf2;
	
	float m_dsamp2, m_fdelaylen2;
	float m_delaytime2, m_maxdelaytime2;
	long m_iwrphase2, m_idelaylen2, m_mask2;
	
};


//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

//	void SampleRate_Ctor(Unit *unit, int inNumSamples);
//	void ControlRate_Ctor(Unit *unit, int inNumSamples);
//	void SampleDur_Ctor(Unit *unit, int inNumSamples);
//	void ControlDur_Ctor(Unit *unit, int inNumSamples);
//	void SubsampleOffset_Ctor(Unit *unit, int inNumSamples);
//	void RadiansPerSample_Ctor(Unit *unit, int inNumSamples);
//	void NumInputBuses_Ctor(Unit *unit, int inNumSamples);
//	void NumOutputBuses_Ctor(Unit *unit, int inNumSamples);
//	void NumAudioBuses_Ctor(Unit *unit, int inNumSamples);
//	void NumControlBuses_Ctor(Unit *unit, int inNumSamples);
//	void NumBuffers_Ctor(Unit *unit, int inNumSamples);
//	void NumRunningSynths_Ctor(Unit *unit, int inNumSamples);
//	void NumRunningSynths_next(Unit *unit, int inNumSamples);
//
//	void BufSampleRate_next(BufInfoUnit *unit, int inNumSamples);
//	void BufSampleRate_Ctor(BufInfoUnit *unit, int inNumSamples);
//
//	void BufFrames_next(BufInfoUnit *unit, int inNumSamples);
//	void BufFrames_Ctor(BufInfoUnit *unit, int inNumSamples);
//
//	void BufDur_next(BufInfoUnit *unit, int inNumSamples);
//	void BufDur_Ctor(BufInfoUnit *unit, int inNumSamples);
//
//	void BufChannels_next(BufInfoUnit *unit, int inNumSamples);
//	void BufChannels_Ctor(BufInfoUnit *unit, int inNumSamples);
//
//	void BufSamples_next(BufInfoUnit *unit, int inNumSamples);
//	void BufSamples_Ctor(BufInfoUnit *unit, int inNumSamples);
//
//	void BufRateScale_next(BufInfoUnit *unit, int inNumSamples);
//	void BufRateScale_Ctor(BufInfoUnit *unit, int inNumSamples);
//
//	void PlayBuf_next_aa(PlayBuf *unit, int inNumSamples);
//	void PlayBuf_next_ak(PlayBuf *unit, int inNumSamples);
//	void PlayBuf_next_ka(PlayBuf *unit, int inNumSamples);
//	void PlayBuf_next_kk(PlayBuf *unit, int inNumSamples);
//	void PlayBuf_Ctor(PlayBuf* unit);
//	void PlayBuf_Dtor(PlayBuf* unit);
//
//	void TGrains_next(TGrains *unit, int inNumSamples);
//	void TGrains_Ctor(TGrains* unit);
//
//#if NOTYET
//	void SimpleLoopBuf_next_kk(SimpleLoopBuf *unit, int inNumSamples);
//	void SimpleLoopBuf_Ctor(SimpleLoopBuf* unit);
//	void SimpleLoopBuf_Dtor(SimpleLoopBuf* unit);
//#endif
//
//	void BufRd_Ctor(BufRd *unit);
//	void BufRd_Dtor(BufRd *unit);
//	void BufRd_next_4(BufRd *unit, int inNumSamples);
//	void BufRd_next_2(BufRd *unit, int inNumSamples);
//	void BufRd_next_1(BufRd *unit, int inNumSamples);
//
//	void BufWr_Ctor(BufWr *unit);
//	void BufWr_Dtor(BufWr *unit);
//	void BufWr_next(BufWr *unit, int inNumSamples);
//
//	void RecordBuf_Ctor(RecordBuf *unit);
//	void RecordBuf_Dtor(RecordBuf *unit);
//	void RecordBuf_next(RecordBuf *unit, int inNumSamples);
//	void RecordBuf_next_10(RecordBuf *unit, int inNumSamples);
//
//	void Pitch_Ctor(Pitch *unit);
//	void Pitch_next_a(Pitch *unit, int inNumSamples);
//	void Pitch_next_k(Pitch *unit, int inNumSamples);
//
//	void LocalBuf_Ctor(LocalBuf *unit);
//	void LocalBuf_Dtor(LocalBuf *unit);
//	void LocalBuf_next(LocalBuf *unit, int inNumSamples);
//
//	void MaxLocalBufs_Ctor(MaxLocalBufs *unit);
//
//	void SetBuf_Ctor(SetBuf *unit);
//	void SetBuf_next(SetBuf *unit, int inNumSamples);
//	void ClearBuf_Ctor(ClearBuf *unit);
//	void ClearBuf_next(ClearBuf *unit, int inNumSamples);
//
//	void BufDelayN_Ctor(BufDelayN *unit);
//	void BufDelayN_next(BufDelayN *unit, int inNumSamples);
//	void BufDelayN_next_z(BufDelayN *unit, int inNumSamples);
//	void BufDelayN_next_a(BufDelayN *unit, int inNumSamples);
//	void BufDelayN_next_a_z(BufDelayN *unit, int inNumSamples);
//
//	void BufDelayL_Ctor(BufDelayL *unit);
//	void BufDelayL_next(BufDelayL *unit, int inNumSamples);
//	void BufDelayL_next_z(BufDelayL *unit, int inNumSamples);
//	void BufDelayL_next_a(BufDelayL *unit, int inNumSamples);
//	void BufDelayL_next_a_z(BufDelayL *unit, int inNumSamples);
//
//	void BufDelayC_Ctor(BufDelayC *unit);
//	void BufDelayC_next(BufDelayC *unit, int inNumSamples);
//	void BufDelayC_next_z(BufDelayC *unit, int inNumSamples);
//	void BufDelayC_next_a(BufDelayC *unit, int inNumSamples);
//	void BufDelayC_next_a_z(BufDelayC *unit, int inNumSamples);
//
//	void BufCombN_Ctor(BufCombN *unit);
//	void BufCombN_next(BufCombN *unit, int inNumSamples);
//	void BufCombN_next_z(BufCombN *unit, int inNumSamples);
//	void BufCombN_next_a(BufCombN *unit, int inNumSamples);
//	void BufCombN_next_a_z(BufCombN *unit, int inNumSamples);
//
//	void BufCombL_Ctor(BufCombL *unit);
//	void BufCombL_next(BufCombL *unit, int inNumSamples);
//	void BufCombL_next_z(BufCombL *unit, int inNumSamples);
//	void BufCombL_next_a(BufCombL *unit, int inNumSamples);
//	void BufCombL_next_a_z(BufCombL *unit, int inNumSamples);
//
//	void BufR1C_Ctor(BufR1C *unit);
//	void BufR1C_next(BufR1C *unit, int inNumSamples);
//	void BufR1C_next_z(BufR1C *unit, int inNumSamples);
//	void BufR1C_next_a(BufR1C *unit, int inNumSamples);
//	void BufR1C_next_a_z(BufR1C *unit, int inNumSamples);
//
//	void BufAllpassN_Ctor(BufAllpassN *unit);
//	void BufAllpassN_next(BufAllpassN *unit, int inNumSamples);
//	void BufAllpassN_next_z(BufAllpassN *unit, int inNumSamples);
//	void BufAllpassN_next_a(BufAllpassN *unit, int inNumSamples);
//	void BufAllpassN_next_a_z(BufAllpassN *unit, int inNumSamples);
//
//	void BufAllpassL_Ctor(BufAllpassL *unit);
//	void BufAllpassL_next(BufAllpassL *unit, int inNumSamples);
//	void BufAllpassL_next_z(BufAllpassL *unit, int inNumSamples);
//	void BufAllpassL_next_a(BufAllpassL *unit, int inNumSamples);
//	void BufAllpassL_next_a_z(BufAllpassL *unit, int inNumSamples);
//
//	void BufAllpassC_Ctor(BufAllpassC *unit);
//	void BufAllpassC_next(BufAllpassC *unit, int inNumSamples);
//	void BufAllpassC_next_z(BufAllpassC *unit, int inNumSamples);
//	void BufAllpassC_next_a(BufAllpassC *unit, int inNumSamples);
//	void BufAllpassC_next_a_z(BufAllpassC *unit, int inNumSamples);

	void DelayUnit_Dtor(DelayUnit *unit);

//	void DelayN_Ctor(DelayN *unit);
//	void DelayN_next(DelayN *unit, int inNumSamples);
//	void DelayN_next_z(DelayN *unit, int inNumSamples);
//	void DelayN_next_a(DelayN *unit, int inNumSamples);
//	void DelayN_next_a_z(DelayN *unit, int inNumSamples);
//
//	void DelayL_Ctor(DelayL *unit);
//	void DelayL_next(DelayL *unit, int inNumSamples);
//	void DelayL_next_z(DelayL *unit, int inNumSamples);
//	void DelayL_next_a(DelayL *unit, int inNumSamples);
//	void DelayL_next_a_z(DelayL *unit, int inNumSamples);
//
//	void DelayC_Ctor(DelayC *unit);
//	void DelayC_next(DelayC *unit, int inNumSamples);
//	void DelayC_next_z(DelayC *unit, int inNumSamples);
//	void DelayC_next_a(DelayC *unit, int inNumSamples);
//	void DelayC_next_a_z(DelayC *unit, int inNumSamples);
//
//	void CombN_Ctor(CombN *unit);
//	void CombN_next(CombN *unit, int inNumSamples);
//	void CombN_next_z(CombN *unit, int inNumSamples);
//	void CombN_next_a(CombN *unit, int inNumSamples);
//	void CombN_next_a_z(CombN *unit, int inNumSamples);
//
//	void CombL_Ctor(CombL *unit);
//	void CombL_next(CombL *unit, int inNumSamples);
//	void CombL_next_z(CombL *unit, int inNumSamples);
//	void CombL_next_a(CombL *unit, int inNumSamples);
//	void CombL_next_a_z(CombL *unit, int inNumSamples);

	void R1C_Ctor(R1C *unit);
	void R1C_next(R1C *unit, int inNumSamples);
	void R1C_next_z(R1C *unit, int inNumSamples);
	void R1C_next_a(R1C *unit, int inNumSamples);
	void R1C_next_a_z(R1C *unit, int inNumSamples);
	
	void R2C_Ctor(R2C *unit);
	void R2C_Dtor(R2C *unit);
	void R2C_next(R2C *unit, int inNumSamples);
	void R2C_next_z(R2C *unit, int inNumSamples);
	void R2C_next_a(R2C *unit, int inNumSamples);
	void R2C_next_a_z(R2C *unit, int inNumSamples);

//	void AllpassN_Ctor(AllpassN *unit);
//	void AllpassN_next(AllpassN *unit, int inNumSamples);
//	void AllpassN_next_z(AllpassN *unit, int inNumSamples);
//	void AllpassN_next_a(AllpassN *unit, int inNumSamples);
//	void AllpassN_next_a_z(AllpassN *unit, int inNumSamples);
//
//	void AllpassL_Ctor(AllpassL *unit);
//	void AllpassL_next(AllpassL *unit, int inNumSamples);
//	void AllpassL_next_z(AllpassL *unit, int inNumSamples);
//	void AllpassL_next_a(AllpassL *unit, int inNumSamples);
//	void AllpassL_next_a_z(AllpassL *unit, int inNumSamples);
//
//	void AllpassC_Ctor(AllpassC *unit);
//	void AllpassC_next(AllpassC *unit, int inNumSamples);
//	void AllpassC_next_z(AllpassC *unit, int inNumSamples);
//	void AllpassC_next_a(AllpassC *unit, int inNumSamples);
//	void AllpassC_next_a_z(AllpassC *unit, int inNumSamples);
//
//	void ScopeOut_next(ScopeOut *unit, int inNumSamples);
//	void ScopeOut_Ctor(ScopeOut *unit);
//	void ScopeOut_Dtor(ScopeOut *unit);
//
//	void Pluck_Ctor(Pluck* unit);
//	void Pluck_next_aa(Pluck *unit, int inNumSamples);
//	void Pluck_next_aa_z(Pluck *unit, int inNumSamples);
//	void Pluck_next_kk(Pluck *unit, int inNumSamples);
//	void Pluck_next_kk_z(Pluck *unit, int inNumSamples);
//	void Pluck_next_ka(Pluck *unit, int inNumSamples);
//	void Pluck_next_ka_z(Pluck *unit, int inNumSamples);
//	void Pluck_next_ak(Pluck *unit, int inNumSamples);
//	void Pluck_next_ak_z(Pluck *unit, int inNumSamples);
//
//	void DelTapWr_Ctor(DelTapWr* unit);
//	void DelTapWr_next(DelTapWr *unit, int inNumSamples);
//	void DelTapWr_next_simd(DelTapWr *unit, int inNumSamples);
//
//	void DelTapRd_Ctor(DelTapRd* unit);
//	void DelTapRd_next1_a(DelTapRd *unit, int inNumSamples);
//	void DelTapRd_next2_a(DelTapRd *unit, int inNumSamples);
//	void DelTapRd_next4_a(DelTapRd *unit, int inNumSamples);
//	void DelTapRd_next1_k(DelTapRd *unit, int inNumSamples);
//	void DelTapRd_next1_k_simd(DelTapRd *unit, int inNumSamples);
//	void DelTapRd_next2_k(DelTapRd *unit, int inNumSamples);
//	void DelTapRd_next4_k(DelTapRd *unit, int inNumSamples);
}

//inline float cubicinterp(float x, float y0, float y1, float y2, float y3)
//{
//	// 4-point, 3rd-order Hermite (x-form)
//	float c0 = y1;
//	float c1 = 0.5f * (y2 - y0);
//	float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
//	float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
//	
//	return ((c3 * x + c2) * x + c1) * x + c0;
//}

/* faster loop macro, length is required to be larger than 0 */
#define LOOP1(length, stmt)			\
{	int xxn = (length);			\
assert(length);				\
do {						\
stmt;					\
} while (--xxn);			\
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double sc_loop(Unit *unit, double in, double hi, int loop)
{
	// avoid the divide if possible
	if (in >= hi) {
		if (!loop) {
			unit->mDone = true;
			return hi;
		}
		in -= hi;
		if (in < hi) return in;
	} else if (in < 0.) {
		if (!loop) {
			unit->mDone = true;
			return 0.;
		}
		in += hi;
		if (in >= 0.) return in;
	} else return in;

	return in - hi * floor(in/hi);
}

#define CHECK_BUF \
	if (!bufData) { \
                unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	}

#define SETUP_OUT \
	uint32 numOutputs = unit->mNumOutputs; \
	if (numOutputs > bufChannels) { \
		if(unit->mWorld->mVerbosity > -1 && !unit->mDone){ \
			Print("buffer-reading UGen channel mismatch: numOutputs %i, yet buffer has %i channels\n", numOutputs, bufChannels); \
		} \
                unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	if(!unit->mOut){ \
		unit->mOut = (float**)RTAlloc(unit->mWorld, numOutputs * sizeof(float*)); \
	} \
	float **out = unit->mOut; \
	for (uint32 i=0; i<numOutputs; ++i){ \
		out[i] = ZOUT(i); \
	}

#define TAKEDOWN_OUT \
	if(unit->mOut){ \
		RTFree(unit->mWorld, unit->mOut); \
	}

#define SETUP_IN(offset) \
	uint32 numInputs = unit->mNumInputs - (uint32)offset; \
	if (numInputs != bufChannels) { \
		if(unit->mWorld->mVerbosity > -1 && !unit->mDone){ \
			Print("buffer-writing UGen channel mismatch: numInputs %i, yet buffer has %i channels\n", numInputs, bufChannels); \
		} \
                unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	if(!unit->mIn){ \
		unit->mIn = (float**)RTAlloc(unit->mWorld, numInputs * sizeof(float*)); \
	} \
	float **in = unit->mIn; \
	for (uint32 i=0; i<numInputs; ++i) { \
		in[i] = ZIN(i+offset); \
	}

#define TAKEDOWN_IN \
	if(unit->mIn){ \
		RTFree(unit->mWorld, unit->mIn); \
	}


#define LOOP_BODY_4 \
		phase = sc_loop((Unit*)unit, phase, loopMax, loop); \
		int32 iphase = (int32)phase; \
		const float* table1 = bufData + iphase * bufChannels; \
		const float* table0 = table1 - bufChannels; \
		const float* table2 = table1 + bufChannels; \
		const float* table3 = table2 + bufChannels; \
		if (iphase == 0) { \
			if (loop) { \
				table0 += bufSamples; \
			} else { \
				table0 += bufChannels; \
			} \
		} else if (iphase >= guardFrame) { \
			if (iphase == guardFrame) { \
				if (loop) { \
					table3 -= bufSamples; \
				} else { \
					table3 -= bufChannels; \
				} \
			} else { \
				if (loop) { \
					table2 -= bufSamples; \
					table3 -= bufSamples; \
				} else { \
					table2 -= bufChannels; \
					table3 -= 2 * bufChannels; \
				} \
			} \
		} \
		int32 index = 0; \
		float fracphase = phase - (double)iphase; \
		for (uint32 i=0; i<numOutputs; ++i) { \
			float a = table0[index]; \
			float b = table1[index]; \
			float c = table2[index]; \
			float d = table3[index]; \
			*++(out[i]) = cubicinterp(fracphase, a, b, c, d); \
			index++; \
		}

#define LOOP_BODY_2 \
		phase = sc_loop((Unit*)unit, phase, loopMax, loop); \
		int32 iphase = (int32)phase; \
		const float* table1 = bufData + iphase * bufChannels; \
		const float* table2 = table1 + bufChannels; \
		if (iphase > guardFrame) { \
			if (loop) { \
				table2 -= bufSamples; \
			} else { \
				table2 -= bufChannels; \
			} \
		} \
		int32 index = 0; \
		float fracphase = phase - (double)iphase; \
		for (uint32 i=0; i<numOutputs; ++i) { \
			float b = table1[index]; \
			float c = table2[index]; \
			*++(out[i]) = b + fracphase * (c - b); \
			index++; \
		}

#define LOOP_BODY_1 \
        phase = sc_loop((Unit*)unit, phase, loopMax, loop); \
		int32 iphase = (int32)phase; \
		const float* table1 = bufData + iphase * bufChannels; \
		int32 index = 0; \
		for (uint32 i=0; i<numOutputs; ++i) { \
			*++(out[i]) = table1[index++]; \
		}


////////////////////////////////////////////////////////////////////////////////////////////////////////


#if 0
void DelayUnit_AllocDelayLine(DelayUnit *unit)
{
	long delaybufsize = (long)ceil(unit->m_maxdelaytime * SAMPLERATE + 1.f);
	delaybufsize = delaybufsize + BUFLENGTH;
	delaybufsize = NEXTPOWEROFTWO(delaybufsize);  // round up to next power of two
	unit->m_fdelaylen = unit->m_idelaylen = delaybufsize;

	RTFree(unit->mWorld, unit->m_dlybuf);
	int size = delaybufsize * sizeof(float);
	//Print("->RTAlloc %d\n", size);
	unit->m_dlybuf = (float*)RTAlloc(unit->mWorld, size);
	//Print("<-RTAlloc %08X\n", unit->m_dlybuf);
	unit->m_mask = delaybufsize - 1;
}
#endif
//
//#define BufCalcDelay(delaytime) (sc_clip(delaytime * (float)SAMPLERATE, 1.f, \
//										 (float)(PREVIOUSPOWEROFTWO(bufSamples))-1))
//
//static void BufDelayUnit_Reset(BufDelayUnit *unit)
//{
//	//Print("->DelayUnit_Reset\n");
//	//unit->m_maxdelaytime = ZIN0(1);
//	unit->m_delaytime = ZIN0(2);
//	//Print("unit->m_delaytime %g\n", unit->m_delaytime);
//	//unit->m_dlybuf = 0;
//	unit->m_fbufnum = -1e9f;
//
//	//DelayUnit_AllocDelayLine(unit);
//	//Print("->GET_BUF\n");
//	GET_BUF
//	//Print("<-GET_BUF\n");
//	unit->m_dsamp = BufCalcDelay(unit->m_delaytime);
//	unit->m_numoutput = 0;
//	unit->m_iwrphase = 0;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//static void BufFeedbackDelay_Reset(BufFeedbackDelay *unit)
//{
//	BufDelayUnit_Reset(unit);
//
//	unit->m_decaytime = ZIN0(3);
//	unit->m_feedbk = sc_CalcFeedback(unit->m_delaytime, unit->m_decaytime);
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

/* helper classes for delay functionality */
//template <bool Checked = false>
//struct DelayN_helper
//{
//	static const bool checked = false;
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, long mask)
//	{
//		long irdphase = iwrphase - idsamp;
//		bufData[iwrphase & mask] = ZXP(in);
//		ZXP(out) = bufData[irdphase & mask];
//		iwrphase++;
//	}
//
//	/* the frac argument is unneeded. the compiler should make sure, that it won't be computed */
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, float frac, long mask)
//	{
//		perform(in, out, bufData, iwrphase, idsamp, mask);
//	}
//};
//
//template <>
//struct DelayN_helper<true>
//{
//	static const bool checked = true;
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, long mask)
//	{
//		long irdphase = iwrphase - idsamp;
//
//		bufData[iwrphase & mask] = ZXP(in);
//		if (irdphase < 0)
//			ZXP(out) = 0.f;
//		else
//			ZXP(out) = bufData[irdphase & mask];
//
//		iwrphase++;
//	}
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, float frac, long mask)
//	{
//		perform(in, out, bufData, iwrphase, idsamp, mask);
//	}
//};
//
//template <bool Checked = false>
//struct DelayL_helper
//{
//	static const bool checked = false;
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, float frac, long mask)
//	{
//		bufData[iwrphase & mask] = ZXP(in);
//		long irdphase = iwrphase - idsamp;
//		long irdphaseb = irdphase - 1;
//		float d1 = bufData[irdphase & mask];
//		float d2 = bufData[irdphaseb & mask];
//		ZXP(out) = lininterp(frac, d1, d2);
//		iwrphase++;
//	}
//};
//
//template <>
//struct DelayL_helper<true>
//{
//	static const bool checked = true;
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, float frac, long mask)
//	{
//		bufData[iwrphase & mask] = ZXP(in);
//		long irdphase = iwrphase - idsamp;
//		long irdphaseb = irdphase - 1;
//
//		if (irdphase < 0) {
//			ZXP(out) = 0.f;
//		} else if (irdphaseb < 0) {
//			float d1 = bufData[irdphase & mask];
//			ZXP(out) = d1 - frac * d1;
//		} else {
//			float d1 = bufData[irdphase & mask];
//			float d2 = bufData[irdphaseb & mask];
//			ZXP(out) = lininterp(frac, d1, d2);
//		}
//		iwrphase++;
//	}
//};
//
//template <bool Checked = false>
//struct DelayC_helper
//{
//	static const bool checked = false;
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, float frac, long mask)
//	{
//		bufData[iwrphase & mask] = ZXP(in);
//		long irdphase1 = iwrphase - idsamp;
//		long irdphase2 = irdphase1 - 1;
//		long irdphase3 = irdphase1 - 2;
//		long irdphase0 = irdphase1 + 1;
//		float d0 = bufData[irdphase0 & mask];
//		float d1 = bufData[irdphase1 & mask];
//		float d2 = bufData[irdphase2 & mask];
//		float d3 = bufData[irdphase3 & mask];
//		ZXP(out) = cubicinterp(frac, d0, d1, d2, d3);
//		iwrphase++;
//	}
//};
//
//template <>
//struct DelayC_helper<true>
//{
//	static const bool checked = true;
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, float frac, long mask)
//	{
//		long irdphase1 = iwrphase - idsamp;
//		long irdphase2 = irdphase1 - 1;
//		long irdphase3 = irdphase1 - 2;
//		long irdphase0 = irdphase1 + 1;
//
//		bufData[iwrphase & mask] = ZXP(in);
//		if (irdphase0 < 0) {
//			ZXP(out) = 0.f;
//		} else {
//			float d0, d1, d2, d3;
//			if (irdphase1 < 0) {
//				d1 = d2 = d3 = 0.f;
//				d0 = bufData[irdphase0 & mask];
//			} else if (irdphase2 < 0) {
//				d1 = d2 = d3 = 0.f;
//				d0 = bufData[irdphase0 & mask];
//				d1 = bufData[irdphase1 & mask];
//			} else if (irdphase3 < 0) {
//				d3 = 0.f;
//				d0 = bufData[irdphase0 & mask];
//				d1 = bufData[irdphase1 & mask];
//				d2 = bufData[irdphase2 & mask];
//			} else {
//				d0 = bufData[irdphase0 & mask];
//				d1 = bufData[irdphase1 & mask];
//				d2 = bufData[irdphase2 & mask];
//				d3 = bufData[irdphase3 & mask];
//			}
//			ZXP(out) = cubicinterp(frac, d0, d1, d2, d3);
//		}
//		iwrphase++;
//	}
//};
//
//template <bool Checked = false>
//struct CombN_helper
//{
//	static const bool checked = false;
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, long mask, float feedbk)
//	{
//		long irdphase = iwrphase - idsamp;
//		float value = bufData[irdphase & mask];
//		bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
//		ZXP(out) = value;
//		++iwrphase;
//	}
//
//	/* the frac argument is unneeded. the compiler should make sure, that it won't be computed */
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
//	{
//		perform(in, out, bufData, iwrphase, idsamp, mask, feedbk);
//	}
//};
//
//template <>
//struct CombN_helper<true>
//{
//	static const bool checked = true;
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, long mask, float feedbk)
//	{
//		long irdphase = iwrphase - idsamp;
//
//		if (irdphase < 0) {
//			bufData[iwrphase & mask] = ZXP(in);
//			ZXP(out) = 0.f;
//		} else {
//			float value = bufData[irdphase & mask];
//			bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
//			ZXP(out) = value;
//		}
//
//		iwrphase++;
//	}
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
//	{
//		perform(in, out, bufData, iwrphase, idsamp, mask, feedbk);
//	}
//};
//
//template <bool Checked = false>
//struct CombL_helper
//{
//	static const bool checked = false;
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
//	{
//		long irdphase = iwrphase - idsamp;
//		long irdphaseb = irdphase - 1;
//		float d1 = bufData[irdphase & mask];
//		float d2 = bufData[irdphaseb & mask];
//		float value = lininterp(frac, d1, d2);
//		bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
//		ZXP(out) = value;
//		iwrphase++;
//	}
//};
//
//template <>
//struct CombL_helper<true>
//{
//	static const bool checked = true;
//
//	static inline void perform(const float *& in, float *& out, float * bufData,
//							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
//	{
//		long irdphase = iwrphase - idsamp;
//		long irdphaseb = irdphase - 1;
//
//		float zin = ZXP(in);
//		if (irdphase < 0) {
//			bufData[iwrphase & mask] = zin;
//			ZXP(out) = 0.f;
//		} else if (irdphaseb < 0) {
//			float d1 = bufData[irdphase & mask];
//			float value = d1 - frac * d1;
//			bufData[iwrphase & mask] = zin + feedbk * value;
//			ZXP(out) = value;
//		} else {
//			float d1 = bufData[irdphase & mask];
//			float d2 = bufData[irdphaseb & mask];
//			float value = lininterp(frac, d1, d2);
//			bufData[iwrphase & mask] = zin + feedbk * value;
//			ZXP(out) = value;
//		}
//		iwrphase++;
//	}
//};

template <bool Checked = false>
struct R1C_helper
{
	static const bool checked = false;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk, float coef, R1C *unit)
	{
		long irdphase1 = iwrphase - idsamp;
		long irdphase2 = irdphase1 - 1;
		long irdphase3 = irdphase1 - 2;
		long irdphase0 = irdphase1 + 1;
		float d0 = bufData[irdphase0 & mask];
		float d1 = bufData[irdphase1 & mask];
		float d2 = bufData[irdphase2 & mask];
		float d3 = bufData[irdphase3 & mask];
		float value = cubicinterp(frac, d0, d1, d2, d3);
		float insamp = ZXP(in);
		float writeval;
		float y1 = unit->m_y1;
		// skip slope for modulating coef for now
		if (coef >= 0.f) {
			writeval = y1 = value + coef * (y1 - value);
		} else {
			writeval = y1 = value + coef * (y1 + value);
		}
		writeval = feedbk * writeval;
		unit->m_y1 = zapgremlins(y1); // inefficient but fix it later...
		bufData[iwrphase & mask] = insamp + writeval;
		ZXP(out) = writeval + insamp;
		iwrphase++;
	}
};

template <>
struct R1C_helper<true>
{
	static const bool checked = true;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk, float coef, R1C *unit)
	{
		long irdphase1 = iwrphase - idsamp;
		long irdphase2 = irdphase1 - 1;
		long irdphase3 = irdphase1 - 2;
		long irdphase0 = irdphase1 + 1;

		if (irdphase0 < 0) {
			bufData[iwrphase & mask] = ZXP(in);
			ZXP(out) = 0.f;
		} else {
			float d0, d1, d2, d3;
			if (irdphase1 < 0) {
				d1 = d2 = d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
			} else if (irdphase2 < 0) {
				d1 = d2 = d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
			} else if (irdphase3 < 0) {
				d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
				d2 = bufData[irdphase2 & mask];
			} else {
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
				d2 = bufData[irdphase2 & mask];
				d3 = bufData[irdphase3 & mask];
			}
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float insamp = ZXP(in);
			float writeval;
			float y1 = unit->m_y1;
			// skip slope for modulating coef for now
			if (coef >= 0.f) {
				writeval = y1 = value + coef * (y1 - value);
			} else {
				writeval = y1 = value + coef * (y1 + value);
			}
			writeval = feedbk * writeval;
			unit->m_y1 = zapgremlins(y1); // inefficient but fix it later...
			bufData[iwrphase & mask] = insamp + writeval;
			ZXP(out) = writeval + insamp;
		}
		iwrphase++;
	}
};

/// R2C

template <bool Checked = false>
struct R2C_helper
{
static const bool checked = false;

static inline void perform(const float *& in, float *& out, float * bufData, float * bufData2, long & iwrphase, long & iwrphase2, long idsamp, long idsamp2, 
						   float frac, float frac2, long mask, long mask2, float scale1, float coef1, float scale2, float coef2, R2C *unit)
{
	float insamp = ZXP(in);
	
	// del 1
	long irdphase1 = iwrphase - idsamp;
	long irdphase2 = irdphase1 - 1;
	long irdphase3 = irdphase1 - 2;
	long irdphase0 = irdphase1 + 1;
	float d0 = bufData[irdphase0 & mask];
	float d1 = bufData[irdphase1 & mask];
	float d2 = bufData[irdphase2 & mask];
	float d3 = bufData[irdphase3 & mask];
	float value = cubicinterp(frac, d0, d1, d2, d3);
	
	float del2in;
	float y1 = unit->m_y1;
	// skip slope for modulating coef for now
	if (coef1 >= 0.f) {
		del2in = y1 = value + coef1 * (y1 - value);
	} else {
		del2in = y1 = value + coef1 * (y1 + value);
	}
	del2in = del2in * scale1;
	unit->m_y1 = zapgremlins(y1); // inefficient but fix it later...
	
	// del 2
	long irdphase1_2 = iwrphase2 - idsamp2;
	long irdphase2_2 = irdphase1_2 - 1;
	long irdphase3_2 = irdphase1_2 - 2;
	long irdphase0_2 = irdphase1_2 + 1;
	float d0_2 = bufData2[irdphase0_2 & mask2];
	float d1_2 = bufData2[irdphase1_2 & mask2];
	float d2_2 = bufData2[irdphase2_2 & mask2];
	float d3_2 = bufData2[irdphase3_2 & mask2];
	float value2 = cubicinterp(frac2, d0_2, d1_2, d2_2, d3_2);
	
	float del2Out;
	float y1_2 = unit->m_y1_2;
	// skip slope for modulating coef for now
	if (coef2 >= 0.f) {
		del2Out = y1_2 = value2 + coef2 * (y1_2 - value2);
	} else {
		del2Out = y1_2 = value2 + coef2 * (y1_2 + value2);
	}
	del2Out = del2Out * scale2;
	unit->m_y1_2 = zapgremlins(y1_2); // inefficient but fix it later...
	
	// not sure about writing after, but it's analogous to the single delay versions
	// write to del1
	bufData[iwrphase & mask] = insamp + del2Out;
	
	// write to del2
	bufData2[iwrphase2 & mask2] = del2in;
	
	ZXP(out) = del2in + del2Out + insamp;
	iwrphase++;
	iwrphase2++;
}
};

template <>
struct R2C_helper<true>
{
	static const bool checked = true;
	
	static inline void perform(const float *& in, float *& out, float * bufData, float * bufData2, long & iwrphase, long & iwrphase2, long idsamp, long idsamp2, 
							   float frac, float frac2, long mask, long mask2, float scale1, float coef1, float scale2, float coef2, R2C *unit)
	{
		long irdphase1 = iwrphase - idsamp;
		long irdphase2 = irdphase1 - 1;
		long irdphase3 = irdphase1 - 2;
		long irdphase0 = irdphase1 + 1;
		
		// del2
		long irdphase1_2 = iwrphase2 - idsamp2;
		long irdphase2_2 = irdphase1_2 - 1;
		long irdphase3_2 = irdphase1_2 - 2;
		long irdphase0_2 = irdphase1_2 + 1;
		
		if (irdphase0 < 0) { // del1 has not yet output so just write and output insamp
			float insamp = ZXP(in);
			bufData[iwrphase & mask] = insamp;
			bufData2[iwrphase2 & mask2] = 0.f;
			ZXP(out) = insamp;
		} else {
			float insamp = ZXP(in);
			
			// del 1
			float d0, d1, d2, d3;
			if (irdphase1 < 0) {
				d1 = d2 = d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
			} else if (irdphase2 < 0) {
				d1 = d2 = d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
			} else if (irdphase3 < 0) {
				d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
				d2 = bufData[irdphase2 & mask];
			} else {
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
				d2 = bufData[irdphase2 & mask];
				d3 = bufData[irdphase3 & mask];
			}
			float value = cubicinterp(frac, d0, d1, d2, d3);
			
			float del2In;
			float y1 = unit->m_y1;
			// skip slope for modulating coef for now
			if (coef1 >= 0.f) {
				del2In = y1 = value + coef1 * (y1 - value);
			} else {
				del2In = y1 = value + coef1 * (y1 + value);
			}
			del2In = del2In * scale1;
			unit->m_y1 = zapgremlins(y1); // inefficient but fix it later...
			
			// del 2
			if (irdphase0_2 < 0) { // del2 has not yet output so just write and output del2In + insamp
				bufData2[iwrphase2 & mask2] = del2In;
				bufData[iwrphase & mask] = insamp;
				ZXP(out) = del2In + insamp;
			} else {
				
				float d0_2, d1_2, d2_2, d3_2;
				if (irdphase1_2 < 0) {
					d1_2 = d2_2 = d3_2 = 0.f;
					d0_2 = bufData2[irdphase0_2 & mask2];
				} else if (irdphase2_2 < 0) {
					d1_2 = d2_2 = d3_2 = 0.f;
					d0_2 = bufData2[irdphase0_2 & mask2];
					d1_2 = bufData2[irdphase1_2 & mask2];
				} else if (irdphase3_2 < 0) {
					d3_2 = 0.f;
					d0_2 = bufData2[irdphase0_2 & mask2];
					d1_2 = bufData2[irdphase1_2 & mask2];
					d2_2 = bufData2[irdphase2_2 & mask2];
				} else {
					d0_2 = bufData2[irdphase0_2 & mask2];
					d1_2 = bufData2[irdphase1_2 & mask2];
					d2_2 = bufData2[irdphase2_2 & mask2];
					d3_2 = bufData2[irdphase3_2 & mask2];
				}
				float value2 = cubicinterp(frac2, d0_2, d1_2, d2_2, d3_2);
				
				float del2Out;
				float y1_2 = unit->m_y1_2;
				// skip slope for modulating coef for now
				if (coef2 >= 0.f) {
					del2Out = y1_2 = value2 + coef2 * (y1_2 - value2);
				} else {
					del2Out = y1_2 = value2 + coef2 * (y1_2 + value2);
				}
				del2Out = del2Out * scale2;
				unit->m_y1_2 = zapgremlins(y1_2); // inefficient but fix it later...
			
				// not sure about writing after, but it's analogous to the single delay versions
				// write to del1
				bufData[iwrphase & mask] = insamp + del2Out;
				
				// write to del2
				bufData2[iwrphase2 & mask2] = del2In;
				
				ZXP(out) = del2In + del2Out + insamp;
		}
		
	}
	iwrphase++;
	iwrphase2++;
};

};

}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// generic functions...

static void DelayUnit_AllocDelayLine(DelayUnit *unit)
{
	long delaybufsize = (long)ceil(unit->m_maxdelaytime * SAMPLERATE + 1.f);
	delaybufsize = delaybufsize + BUFLENGTH;
	delaybufsize = NEXTPOWEROFTWO(delaybufsize);  // round up to next power of two
	unit->m_fdelaylen = unit->m_idelaylen = delaybufsize;

	RTFree(unit->mWorld, unit->m_dlybuf);
	unit->m_dlybuf = (float*)RTAlloc(unit->mWorld, delaybufsize * sizeof(float));
	unit->m_mask = delaybufsize - 1;
}

static float CalcDelay(DelayUnit *unit, float delaytime)
{
	float next_dsamp = delaytime * (float)SAMPLERATE;
	return sc_clip(next_dsamp, 1.f, unit->m_fdelaylen);
}

// for R2C
static float CalcDelay2(R2C *unit, float delaytime)
{
	float next_dsamp = delaytime * (float)SAMPLERATE;
	return sc_clip(next_dsamp, 1.f, unit->m_fdelaylen2);
}

static void DelayUnit_Reset(DelayUnit *unit)
{
	unit->m_maxdelaytime = ZIN0(1);
	unit->m_delaytime = ZIN0(2);
	unit->m_dlybuf = 0;

	DelayUnit_AllocDelayLine(unit);

	unit->m_dsamp = CalcDelay(unit, unit->m_delaytime);

	unit->m_numoutput = 0;
	unit->m_iwrphase = 0;
}


void DelayUnit_Dtor(DelayUnit *unit)
{
	RTFree(unit->mWorld, unit->m_dlybuf);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FeedbackDelay_Reset(FeedbackDelay *unit)
{
	unit->m_decaytime = ZIN0(3);

	DelayUnit_Reset(unit);

	unit->m_feedbk = sc_CalcFeedback(unit->m_delaytime, unit->m_decaytime);
}
	
static void R2C_Reset(R2C *unit)
{
	//Delay1
	//FeedbackDelay_Reset(unit);
	unit->m_maxdelaytime = ZIN0(1);
	unit->m_delaytime = ZIN0(2);
	unit->m_dlybuf = 0;
	
	//DelayUnit_AllocDelayLine(unit);
	long delaybufsize = (long)ceil(unit->m_maxdelaytime * SAMPLERATE + 1.f);
	delaybufsize = delaybufsize + BUFLENGTH;
	delaybufsize = NEXTPOWEROFTWO(delaybufsize);  // round up to next power of two
	unit->m_fdelaylen = unit->m_idelaylen = delaybufsize;
	
	RTFree(unit->mWorld, unit->m_dlybuf);
	unit->m_dlybuf = (float*)RTAlloc(unit->mWorld, delaybufsize * sizeof(float));
	unit->m_mask = delaybufsize - 1;
	
	
	unit->m_dsamp = CalcDelay(unit, unit->m_delaytime);
	
	unit->m_numoutput = 0;
	unit->m_iwrphase = 0;
	
	//Delay2
	//FeedbackDelay_Reset(unit);
	unit->m_maxdelaytime2 = ZIN0(3);
	unit->m_delaytime2 = ZIN0(4);
	unit->m_dlybuf2 = 0;
	
	//DelayUnit_AllocDelayLine(unit);
	long delaybufsize2 = (long)ceil(unit->m_maxdelaytime2 * SAMPLERATE + 1.f);
	delaybufsize2 = delaybufsize2 + BUFLENGTH;
	delaybufsize2 = NEXTPOWEROFTWO(delaybufsize2);  // round up to next power of two
	unit->m_fdelaylen2 = unit->m_idelaylen2 = delaybufsize2;
	
	RTFree(unit->mWorld, unit->m_dlybuf2);
	unit->m_dlybuf2 = (float*)RTAlloc(unit->mWorld, delaybufsize2 * sizeof(float));
	unit->m_mask2 = delaybufsize2 - 1;
	
	
	unit->m_dsamp2 = CalcDelay2(unit, unit->m_delaytime2);
	
	unit->m_iwrphase2 = 0;
}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///* template function to generate delay ugen function, control-rate delay time */
//template <typename PerformClass,
//		  typename DelayX
//		 >
//inline void DelayX_perform(DelayX *unit, int inNumSamples, UnitCalcFunc resetFunc)
//{
//	float *out = ZOUT(0);
//	const float *in = ZIN(0);
//	float delaytime = ZIN0(2);
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	long mask = unit->m_mask;
//
//	if (delaytime == unit->m_delaytime) {
//		long idsamp = (long)dsamp;
//		float frac = dsamp - idsamp;
//		LOOP1(inNumSamples,
//			PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask);
//		);
//	} else {
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		LOOP1(inNumSamples,
//			dsamp += dsamp_slope;
//			long idsamp = (long)dsamp;
//			float frac = dsamp - idsamp;
//			PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask);
//		);
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//	}
//
//	unit->m_iwrphase = iwrphase;
//
//	if (PerformClass::checked) {
//		unit->m_numoutput += inNumSamples;
//		if (unit->m_numoutput >= unit->m_idelaylen)
//			unit->mCalcFunc = resetFunc;
//	}
//}
//
///* template function to generate delay ugen function, audio-rate delay time */
//template <typename PerformClass,
//		  typename DelayX
//		 >
//inline void DelayX_perform_a(DelayX *unit, int inNumSamples, UnitCalcFunc resetFunc)
//{
//	float *out = ZOUT(0);
//	const float *in = ZIN(0);
//	float * delaytime = ZIN(2);
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	long mask = unit->m_mask;
//
//	LOOP1(inNumSamples,
//		float dsamp = CalcDelay(unit, ZXP(delaytime));
//		long idsamp = (long)dsamp;
//
//		float frac = dsamp - idsamp;
//		PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask);
//	);
//
//	unit->m_iwrphase = iwrphase;
//
//	if (PerformClass::checked)
//	{
//		unit->m_numoutput += inNumSamples;
//		if (unit->m_numoutput >= unit->m_idelaylen)
//			unit->mCalcFunc = resetFunc;
//	}
//}


////////////////////////////////////////////////////////////////////////////////////////////////////////

//void DelayN_Ctor(DelayN *unit)
//{
//	DelayUnit_Reset(unit);
//	if(INRATE(2) == calc_FullRate)
//		SETCALC(DelayN_next_a_z);
//	else
//		SETCALC(DelayN_next_z);
//	ZOUT0(0) = 0.f;
//}
//
//void DelayN_next(DelayN *unit, int inNumSamples)
//{
//	float *out = ZOUT(0);
//	const float *in = ZIN(0);
//	float delaytime = ZIN0(2);
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	long mask = unit->m_mask;
//
//	//Print("DelayN_next %08X %g %g  %d %d\n", unit, delaytime, dsamp, mask, iwrphase);
//	if (delaytime == unit->m_delaytime) {
//		long irdphase = iwrphase - (long)dsamp;
//		float* dlybuf1 = dlybuf - ZOFF;
//		float* dlyrd   = dlybuf1 + (irdphase & mask);
//		float* dlywr   = dlybuf1 + (iwrphase & mask);
//		float* dlyN    = dlybuf1 + unit->m_idelaylen;
//		long remain = inNumSamples;
//		while (remain) {
//			long rdspace = dlyN - dlyrd;
//			long wrspace = dlyN - dlywr;
//			long nsmps = sc_min(rdspace, wrspace);
//			nsmps = sc_min(remain, nsmps);
//			remain -= nsmps;
//			LOOP(nsmps,
//				ZXP(dlywr) = ZXP(in);
//				ZXP(out) = ZXP(dlyrd);
//			);
//			if (dlyrd == dlyN) dlyrd = dlybuf1;
//			if (dlywr == dlyN) dlywr = dlybuf1;
//		}
//		iwrphase += inNumSamples;
//	} else {
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		LOOP1(inNumSamples,
//			dsamp += dsamp_slope;
//			DelayN_helper<false>::perform(in, out, dlybuf, iwrphase, (long)dsamp, mask);
//		);
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//	}
//
//	unit->m_iwrphase = iwrphase;
//}
//
//
//void DelayN_next_z(DelayN *unit, int inNumSamples)
//{
//	float *out = ZOUT(0);
//	const float *in = ZIN(0);
//	float delaytime = ZIN0(2);
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	long mask = unit->m_mask;
//
//	if (delaytime == unit->m_delaytime) {
//		long irdphase = iwrphase - (long)dsamp;
//		float* dlybuf1 = dlybuf - ZOFF;
//		float* dlyN    = dlybuf1 + unit->m_idelaylen;
//		long remain = inNumSamples;
//		while (remain) {
//			float* dlywr = dlybuf1 + (iwrphase & mask);
//			float* dlyrd = dlybuf1 + (irdphase & mask);
//			long rdspace = dlyN - dlyrd;
//			long wrspace = dlyN - dlywr;
//			long nsmps = sc_min(rdspace, wrspace);
//			nsmps = sc_min(remain, nsmps);
//			remain -= nsmps;
//			if (irdphase < 0) {
//				LOOP(nsmps,
//					ZXP(dlywr) = ZXP(in);
//					ZXP(out) = 0.f;
//				);
//			} else {
//				LOOP(nsmps,
//					ZXP(dlywr) = ZXP(in);
//					ZXP(out) = ZXP(dlyrd);
//				);
//			}
//			iwrphase += nsmps;
//			irdphase += nsmps;
//		}
//	} else {
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		LOOP1(inNumSamples,
//			dsamp += dsamp_slope;
//			DelayN_helper<true>::perform(in, out, dlybuf, iwrphase, (long)dsamp, mask);
//		);
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//	}
//
//	unit->m_iwrphase = iwrphase;
//
//	unit->m_numoutput += inNumSamples;
//	if (unit->m_numoutput >= unit->m_idelaylen)
//		SETCALC(DelayN_next);
//}
//
//template <bool checked>
//inline void DelayN_perform_a(DelayN *unit, int inNumSamples)
//{
//	DelayX_perform_a<DelayN_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)DelayN_next_a);
//}
//
//void DelayN_next_a(DelayN *unit, int inNumSamples)
//{
//	DelayN_perform_a<false>(unit, inNumSamples);
//}
//
//void DelayN_next_a_z(DelayN *unit, int inNumSamples)
//{
//	DelayN_perform_a<true>(unit, inNumSamples);
//}
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void DelayL_Ctor(DelayL *unit)
//{
//	DelayUnit_Reset(unit);
//	if(INRATE(2) == calc_FullRate)
//		SETCALC(DelayL_next_a_z);
//	else
//		SETCALC(DelayL_next_z);
//	ZOUT0(0) = 0.f;
//}
//
//template <bool checked>
//inline void DelayL_perform(DelayL *unit, int inNumSamples)
//{
//	DelayX_perform<DelayL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)DelayL_next);
//}
//
//void DelayL_next(DelayL *unit, int inNumSamples)
//{
//	DelayL_perform<false>(unit, inNumSamples);
//}
//
//void DelayL_next_z(DelayL *unit, int inNumSamples)
//{
//	DelayL_perform<true>(unit, inNumSamples);
//}
//
//template <bool checked>
//inline void DelayL_perform_a(DelayL *unit, int inNumSamples)
//{
//	DelayX_perform_a<DelayL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)DelayL_next_a);
//}
//
//void DelayL_next_a(DelayL *unit, int inNumSamples)
//{
//	DelayL_perform_a<false>(unit, inNumSamples);
//}
//
//void DelayL_next_a_z(DelayL *unit, int inNumSamples)
//{
//	DelayL_perform_a<true>(unit, inNumSamples);
//}

//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void DelayC_Ctor(DelayC *unit)
//{
//	DelayUnit_Reset(unit);
//	if(INRATE(2) == calc_FullRate)
//		SETCALC(DelayC_next_a_z);
//	else
//		SETCALC(DelayC_next_z);
//	ZOUT0(0) = 0.f;
//}
//
//template <bool checked>
//inline void DelayC_perform(DelayC *unit, int inNumSamples)
//{
//	DelayX_perform<DelayC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)DelayC_next);
//}
//
//void DelayC_next(DelayC *unit, int inNumSamples)
//{
//	DelayC_perform<false>(unit, inNumSamples);
//}
//
//void DelayC_next_z(DelayC *unit, int inNumSamples)
//{
//	DelayC_perform<true>(unit, inNumSamples);
//}
//
//
//template <bool checked>
//inline void DelayC_perform_a(DelayC *unit, int inNumSamples)
//{
//	DelayX_perform_a<DelayC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)DelayC_next_a);
//}
//
//void DelayC_next_a(DelayC *unit, int inNumSamples)
//{
//	DelayC_perform_a<false>(unit, inNumSamples);
//}
//
//void DelayC_next_a_z(DelayC *unit, int inNumSamples)
//{
//	DelayC_perform_a<true>(unit, inNumSamples);
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename PerformClass,
		  typename R1X
		 >
inline void FilterX_perform(R1X *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float delaytime = ZIN0(2);
	float coef = ZIN0(3);
	float feedbk = ZIN0(4);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP1(inNumSamples,
			PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask, feedbk, coef, unit);
		);
	} else {
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		//float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(feedbk, unit->m_feedbk);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask, feedbk, coef, unit);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		//unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;

	if (PerformClass::checked) {
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= unit->m_idelaylen)
			unit->mCalcFunc = resetFunc;
	}
}

template <typename PerformClass,
		  typename R1C
		 >
inline void FilterX_perform_a(R1C *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float * delaytime = ZIN(2);
	float coef = ZIN0(3);
	float feedbk = ZIN0(4);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	//float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;

	LOOP1(inNumSamples,
		float del = ZXP(delaytime);
		float dsamp = CalcDelay(unit, del);
		//float feedbk = sc_CalcFeedback(del, decaytime);

		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask, feedbk, coef, unit);
	);

	unit->m_iwrphase = iwrphase;

	if (PerformClass::checked)
	{
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= unit->m_idelaylen)
			unit->mCalcFunc = resetFunc;
	}
}


//// R2C

template <typename PerformClass,
typename R2X
>
inline void R2C_perform(R2X *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float delaytime = ZIN0(2);
	float delaytime2 = ZIN0(4);
	float coef1 = ZIN0(5);
	float scale1 = ZIN0(6);
	float coef2 = ZIN0(7);
	float scale2 = ZIN0(8);
	
	// del1
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;
	
	// del2
	float *dlybuf2 = unit->m_dlybuf2;
	long iwrphase2 = unit->m_iwrphase2;
	float dsamp2 = unit->m_dsamp2;
	long mask2 = unit->m_mask2;
	
	if ((delaytime == unit->m_delaytime) && (delaytime2 == unit->m_delaytime2)) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		long idsamp2 = (long)dsamp2;
		float frac2 = dsamp2 - idsamp2;
		
		LOOP1(inNumSamples,
			  PerformClass::perform(in, out, dlybuf, dlybuf2, iwrphase, iwrphase2, idsamp, idsamp2, frac, frac2, mask, mask2, scale1, coef1, scale2, coef2, unit);
			  );
	} else {
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		//float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope1 = CALCSLOPE(scale1, unit->m_feedbk);
		float feedbk_slope2 = CALCSLOPE(scale2, unit->m_feedbk2);
		
		float next_dsamp2 = CalcDelay2(unit, delaytime2);
		float dsamp_slope2 = CALCSLOPE(next_dsamp2, dsamp2);
		
		LOOP1(inNumSamples,
			  dsamp += dsamp_slope;
			  dsamp2 += dsamp_slope2;
			  scale1 += feedbk_slope1;
			  scale2 += feedbk_slope2;
			  long idsamp = (long)dsamp;
			  float frac = dsamp - idsamp;
			  long idsamp2 = (long)dsamp2;
			  float frac2 = dsamp2 - idsamp2;
			  PerformClass::perform(in, out, dlybuf, dlybuf2, iwrphase, iwrphase2, idsamp, idsamp2, frac, frac2, mask, mask2, scale1, coef1, scale2, coef2, unit);
			  );
		unit->m_feedbk = scale1;
		unit->m_feedbk2 = scale2;
		unit->m_dsamp = dsamp;
		unit->m_dsamp2 = dsamp2;
		unit->m_delaytime = delaytime;
		//unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;
	unit->m_iwrphase2 = iwrphase2;
	
	if (PerformClass::checked) {
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= (unit->m_idelaylen + unit->m_idelaylen2))
			unit->mCalcFunc = resetFunc;
	}
}

template <typename PerformClass,
typename R2C
>
inline void R2C_perform_a(R2C *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float * delaytime = ZIN(2);
	float * delaytime2 = ZIN(4);
	float coef1 = ZIN0(5);
	float scale1 = ZIN0(6);
	float coef2 = ZIN0(7);
	float scale2 = ZIN0(8);
	
	// del 1
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	//float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	
	// del 2
	float *dlybuf2 = unit->m_dlybuf2;
	long iwrphase2 = unit->m_iwrphase2;
	float dsamp2 = unit->m_dsamp2;
	//float feedbk = unit->m_feedbk;
	long mask2 = unit->m_mask2;
	
	LOOP1(inNumSamples,
		  float del = ZXP(delaytime);
		  float dsamp = CalcDelay(unit, del);
		  //float feedbk = sc_CalcFeedback(del, decaytime);
		  
		  float del2 = ZXP(delaytime2);
		  float dsamp2 = CalcDelay2(unit, del2);
		  
		  long idsamp = (long)dsamp;
		  float frac = dsamp - idsamp;
		  
		  long idsamp2 = (long)dsamp2;
		  float frac2 = dsamp2 - idsamp2;
		  
		  PerformClass::perform(in, out, dlybuf, dlybuf2, iwrphase, iwrphase2, idsamp, idsamp2, frac, frac2, mask, mask2, scale1, coef1, scale2, coef2, unit);
		  );
	
	unit->m_iwrphase = iwrphase;
	unit->m_iwrphase2 = iwrphase2;
	
	if (PerformClass::checked)
	{
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= (unit->m_idelaylen + unit->m_idelaylen2))
			unit->mCalcFunc = resetFunc;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void R1C_Ctor(R1C *unit)
{
	DelayUnit_Reset(unit);
	if(INRATE(2) == calc_FullRate)
		SETCALC(R1C_next_a_z);
	else
		SETCALC(R1C_next_z);
	unit->m_b1 = 0.f;
	unit->m_y1 = 0.f;
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void R1C_perform(R1C *unit, int inNumSamples)
{
	FilterX_perform<R1C_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)R1C_next);
}

void R1C_next(R1C *unit, int inNumSamples)
{
	R1C_perform<false>(unit, inNumSamples);
}

void R1C_next_z(R1C *unit, int inNumSamples)
{
	R1C_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void R1C_perform_a(R1C *unit, int inNumSamples)
{
	FilterX_perform_a<R1C_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)R1C_next_a);
}

void R1C_next_a(R1C *unit, int inNumSamples)
{
	R1C_perform_a<false>(unit, inNumSamples);
}

void R1C_next_a_z(R1C *unit, int inNumSamples)
{
	R1C_perform_a<true>(unit, inNumSamples);
}


///// R2C
// don't forget to sort out Dtor

void R2C_Ctor(R2C *unit)
{
	R2C_Reset(unit);
	
	////
	if(INRATE(2) == calc_FullRate)
		SETCALC(R2C_next_a_z);
	else
		SETCALC(R2C_next_z);
	unit->m_b1 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_b1_2 = 0.f;
	unit->m_y1_2 = 0.f;
	ZOUT0(0) = 0.f;
}
	
void R2C_Dtor(R2C *unit)
{
	RTFree(unit->mWorld, unit->m_dlybuf);
	RTFree(unit->mWorld, unit->m_dlybuf2);
}

template <bool checked>
inline void R2C_perform(R2C *unit, int inNumSamples)
{
	R2C_perform<R2C_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)R2C_next);
}

void R2C_next(R2C *unit, int inNumSamples)
{
	R2C_perform<false>(unit, inNumSamples);
}

void R2C_next_z(R2C *unit, int inNumSamples)
{
	R2C_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void R2C_perform_a(R2C *unit, int inNumSamples)
{
	R2C_perform_a<R2C_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)R2C_next_a);
}

void R2C_next_a(R2C *unit, int inNumSamples)
{
	R2C_perform_a<false>(unit, inNumSamples);
}

void R2C_next_a_z(R2C *unit, int inNumSamples)
{
	R2C_perform_a<true>(unit, inNumSamples);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void AllpassN_Ctor(AllpassN *unit)
//{
//	FeedbackDelay_Reset(unit);
//	if(INRATE(2) == calc_FullRate)
//		SETCALC(AllpassN_next_a_z);
//	else
//		SETCALC(AllpassN_next_z);
//	ZOUT0(0) = 0.f;
//}
//
//void AllpassN_next(AllpassN *unit, int inNumSamples)
//{
//	float *out = ZOUT(0);
//	const float *in = ZIN(0);
//	float delaytime = ZIN0(2);
//	float decaytime = ZIN0(3);
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	float feedbk = unit->m_feedbk;
//	long mask = unit->m_mask;
//
//	//postbuf("AllpassN_next %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
//	if (delaytime == unit->m_delaytime) {
//		long irdphase = iwrphase - (long)dsamp;
//		float* dlybuf1 = dlybuf - ZOFF;
//		float* dlyrd   = dlybuf1 + (irdphase & mask);
//		float* dlywr   = dlybuf1 + (iwrphase & mask);
//		float* dlyN    = dlybuf1 + unit->m_idelaylen;
//		if (decaytime == unit->m_decaytime) {
//			long remain = inNumSamples;
//			while (remain) {
//				long rdspace = dlyN - dlyrd;
//				long wrspace = dlyN - dlywr;
//				long nsmps = sc_min(rdspace, wrspace);
//				nsmps = sc_min(remain, nsmps);
//				remain -= nsmps;
//				LOOP(nsmps,
//					float value = ZXP(dlyrd);
//					float dwr = value * feedbk + ZXP(in);
//					ZXP(dlywr) = dwr;
//					ZXP(out) = value - feedbk * dwr;
//				);
//				if (dlyrd == dlyN) dlyrd = dlybuf1;
//				if (dlywr == dlyN) dlywr = dlybuf1;
//			}
//		} else {
//			float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//			long remain = inNumSamples;
//			while (remain) {
//				long rdspace = dlyN - dlyrd;
//				long wrspace = dlyN - dlywr;
//				long nsmps = sc_min(rdspace, wrspace);
//				nsmps = sc_min(remain, nsmps);
//				remain -= nsmps;
//
//				LOOP(nsmps,
//					float value = ZXP(dlyrd);
//					float dwr = value * feedbk + ZXP(in);
//					ZXP(dlywr) = dwr;
//					ZXP(out) = value - feedbk * dwr;
//					feedbk += feedbk_slope;
//				);
//				if (dlyrd == dlyN) dlyrd = dlybuf1;
//				if (dlywr == dlyN) dlywr = dlybuf1;
//			}
//			unit->m_feedbk = feedbk;
//			unit->m_decaytime = decaytime;
//		}
//		iwrphase += inNumSamples;
//	} else {
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//		LOOP1(inNumSamples,
//			dsamp += dsamp_slope;
//			feedbk += feedbk_slope;
//			AllpassN_helper<false>::perform(in, out, dlybuf, iwrphase, (long)dsamp, mask, feedbk);
//		);
//		unit->m_feedbk = feedbk;
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//		unit->m_decaytime = decaytime;
//	}
//
//	unit->m_iwrphase = iwrphase;
//}
//
//
//void AllpassN_next_z(AllpassN *unit, int inNumSamples)
//{
//	float *out = ZOUT(0);
//	const float *in = ZIN(0);
//	float delaytime = ZIN0(2);
//	float decaytime = ZIN0(3);
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	float feedbk = unit->m_feedbk;
//	long mask = unit->m_mask;
//
//	//postbuf("AllpassN_next_z %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
//	if (delaytime == unit->m_delaytime) {
//		long irdphase = iwrphase - (long)dsamp;
//		float* dlybuf1 = dlybuf - ZOFF;
//		float* dlyN    = dlybuf1 + unit->m_idelaylen;
//		if (decaytime == unit->m_decaytime) {
//			long remain = inNumSamples;
//			while (remain) {
//				float* dlywr = dlybuf1 + (iwrphase & mask);
//				float* dlyrd = dlybuf1 + (irdphase & mask);
//				long rdspace = dlyN - dlyrd;
//				long wrspace = dlyN - dlywr;
//				long nsmps = sc_min(rdspace, wrspace);
//				nsmps = sc_min(remain, nsmps);
//				remain -= nsmps;
//				if (irdphase < 0) {
//					feedbk = -feedbk;
//					LOOP(nsmps,
//						float dwr = ZXP(in);
//						ZXP(dlywr) = dwr;
//						ZXP(out) = feedbk * dwr;
//					);
//					feedbk = -feedbk;
//				} else {
//					LOOP(nsmps,
//						float x1 = ZXP(dlyrd);
//						float dwr = x1 * feedbk + ZXP(in);
//						ZXP(dlywr) = dwr;
//						ZXP(out) = x1 - feedbk * dwr;
//					);
//				}
//				iwrphase += nsmps;
//				irdphase += nsmps;
//			}
//		} else {
//			float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//			long remain = inNumSamples;
//			while (remain) {
//				float* dlyrd = dlybuf1 + (irdphase & mask);
//				float* dlywr = dlybuf1 + (iwrphase & mask);
//				long rdspace = dlyN - dlyrd;
//				long wrspace = dlyN - dlywr;
//				long nsmps = sc_min(rdspace, wrspace);
//				nsmps = sc_min(remain, nsmps);
//				remain -= nsmps;
//
//				if (irdphase < 0) {
//					dlyrd += nsmps;
//					LOOP(nsmps,
//						float dwr = ZXP(in);
//						ZXP(dlywr) = dwr;
//						ZXP(out) = -feedbk * dwr;
//						feedbk += feedbk_slope;
//					);
//				} else {
//					LOOP(nsmps,
//						float x1 = ZXP(dlyrd);
//						float dwr = x1 * feedbk + ZXP(in);
//						ZXP(dlywr) = dwr;
//						ZXP(out) = x1 - feedbk * dwr;
//						feedbk += feedbk_slope;
//					);
//				}
//				iwrphase += nsmps;
//				irdphase += nsmps;
//			}
//			unit->m_feedbk = feedbk;
//			unit->m_decaytime = decaytime;
//		}
//	} else {
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//
//		LOOP1(inNumSamples,
//			dsamp += dsamp_slope;
//			feedbk += feedbk_slope;
//			AllpassN_helper<true>::perform(in, out, dlybuf, iwrphase, (long)dsamp, mask, feedbk);
//		);
//		unit->m_feedbk = feedbk;
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//		unit->m_decaytime = decaytime;
//	}
//
//	unit->m_iwrphase = iwrphase;
//
//	unit->m_numoutput += inNumSamples;
//	if (unit->m_numoutput >= unit->m_idelaylen)
//		SETCALC(AllpassN_next);
//}
//
//template <bool checked>
//inline void AllpassN_perform_a(AllpassN *unit, int inNumSamples)
//{
//	FilterX_perform_a<AllpassN_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)AllpassN_next_a);
//}
//
//void AllpassN_next_a(AllpassN *unit, int inNumSamples)
//{
//	AllpassN_perform_a<false>(unit, inNumSamples);
//}
//
//void AllpassN_next_a_z(AllpassN *unit, int inNumSamples)
//{
//	AllpassN_perform_a<true>(unit, inNumSamples);
//}
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void AllpassL_Ctor(AllpassL *unit)
//{
//	FeedbackDelay_Reset(unit);
//	if(INRATE(2) == calc_FullRate)
//		SETCALC(AllpassL_next_a_z);
//	else
//		SETCALC(AllpassL_next_z);
//	ZOUT0(0) = 0.f;
//}
//
//template <bool checked>
//inline void AllpassL_perform(AllpassL *unit, int inNumSamples)
//{
//	FilterX_perform<AllpassL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)AllpassL_next);
//}
//
//void AllpassL_next(AllpassL *unit, int inNumSamples)
//{
//	AllpassL_perform<false>(unit, inNumSamples);
//}
//
//void AllpassL_next_z(AllpassL *unit, int inNumSamples)
//{
//	AllpassL_perform<true>(unit, inNumSamples);
//}
//
//template <bool checked>
//inline void AllpassL_perform_a(AllpassL *unit, int inNumSamples)
//{
//	FilterX_perform_a<AllpassL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)AllpassL_next_a);
//}
//
//void AllpassL_next_a(AllpassL *unit, int inNumSamples)
//{
//	AllpassL_perform_a<false>(unit, inNumSamples);
//}
//
//void AllpassL_next_a_z(AllpassL *unit, int inNumSamples)
//{
//	AllpassL_perform_a<true>(unit, inNumSamples);
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void AllpassC_Ctor(AllpassC *unit)
//{
//	FeedbackDelay_Reset(unit);
//	if(INRATE(2) == calc_FullRate)
//		SETCALC(AllpassC_next_a_z);
//	else
//		SETCALC(AllpassC_next_z);
//	ZOUT0(0) = 0.f;
//}
//
//template <bool checked>
//inline void AllpassC_perform(AllpassC *unit, int inNumSamples)
//{
//	FilterX_perform<AllpassC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)AllpassC_next);
//}
//
//void AllpassC_next(AllpassC *unit, int inNumSamples)
//{
//	AllpassC_perform<false>(unit, inNumSamples);
//}
//
//void AllpassC_next_z(AllpassC *unit, int inNumSamples)
//{
//	AllpassC_perform<true>(unit, inNumSamples);
//}
//
//template <bool checked>
//inline void AllpassC_perform_a(AllpassC *unit, int inNumSamples)
//{
//	FilterX_perform_a<AllpassC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)AllpassC_next_a);
//}
//
//void AllpassC_next_a(AllpassC *unit, int inNumSamples)
//{
//	AllpassC_perform_a<false>(unit, inNumSamples);
//}
//
//void AllpassC_next_a_z(AllpassC *unit, int inNumSamples)
//{
//	AllpassC_perform_a<true>(unit, inNumSamples);
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//inline double sc_loop1(int32 in, int32 lo, int32 hi)
//{
//	// avoid the divide if possible
//	if (in >= hi) {
//		in -= hi;
//		if (in < hi) return in;
//	} else if (in < lo) {
//		in += hi;
//		if (in >= lo) return in;
//	} else return in;
//
//	int32 range = hi - lo;
//	return lo + range * (in-lo) / range;
//}
//
//#if NOTYET
//
//void SimpleLoopBuf_next_kk(SimpleLoopBuf *unit, int inNumSamples)
//{
//	float trig     = ZIN0(1);
//	double loopstart  = (double)ZIN0(2);
//	double loopend    = (double)ZIN0(3);
//	GET_BUF
//	CHECK_BUF
//	SETUP_OUT
//
//	loopend = sc_max(loopend, bufFrames);
//	int32 phase = unit->m_phase;
//	if (trig > 0.f && unit->m_prevtrig <= 0.f) {
//		unit->mDone = false;
//		phase = (int32)ZIN0(2);
//	}
//	unit->m_prevtrig = trig;
//	for (int i=0; i<inNumSamples; ++i) {
//
//		phase = sc_loop1(phase, loopstart, loopend);
//		int32 iphase = (int32)phase;
//		float* table1 = bufData + iphase * bufChannels;
//		int32 index = 0;
//		for (uint32 i=0; i<bufChannels; ++i) {
//			*++(out[i]) = table1[index++];
//		}
//
//		phase++;
//	}
//	unit->m_phase = phase;
//}
//
//
//void SimpleLoopBuf_Ctor(SimpleLoopBuf *unit)
//{
//	SETCALC(SimpleLoopBuf_next_kk);
//
//	unit->m_fbufnum = -1e9f;
//	unit->m_prevtrig = 0.;
//	unit->mOut = 0;
//	unit->m_phase = ZIN0(2);
//
//	ClearUnitOutputs(unit, 1);
//}
//
//void SimpleLoopBuf_Dtor(SimpleLoopBuf *unit)
//{
//	TAKEDOWN_OUT
//}
//#endif
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define GET_SCOPEBUF \
//	float fbufnum  = ZIN0(0); \
//	if (fbufnum != unit->m_fbufnum) { \
//		World *world = unit->mWorld; \
//		if (!world->mNumSndBufs) { \
//			ClearUnitOutputs(unit, inNumSamples); \
//			return; \
//		} \
//		uint32 bufnum = (int)fbufnum; \
//		if (bufnum >= world->mNumSndBufs) bufnum = 0; \
//		unit->m_fbufnum = fbufnum; \
//		unit->m_buf = world->mSndBufs + bufnum; \
//		unit->m_bufupdates = world->mSndBufUpdates + bufnum; \
//	} \
//	SndBuf *buf = unit->m_buf; \
//	LOCK_SNDBUF(buf); \
//	SndBufUpdates *bufupdates = unit->m_bufupdates; \
//	float *bufData __attribute__((__unused__)) = buf->data; \
//	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
//	uint32 bufFrames __attribute__((__unused__)) = buf->frames; \
//
//void ScopeOut_next(ScopeOut *unit, int inNumSamples)
//{
//	GET_SCOPEBUF
//
//	if (!bufData) {
//		unit->m_framepos = 0;
//		return;
//	}
//
//	SETUP_IN(1)
//
//	uint32 framepos = unit->m_framepos;
//	if (framepos >= bufFrames) {
//		unit->m_framepos = 0;
//	}
//
//	if (bufupdates->reads != bufupdates->writes) {
//		unit->m_framepos += inNumSamples;
//		return;
//	}
//
//	bufData += framepos * bufChannels;
//
//	int remain = (bufFrames - framepos), wrap = 0;
//
//	if(inNumSamples <= remain) {
//		remain = inNumSamples;
//		wrap = 0;
//	}
//	else
//		wrap = inNumSamples - remain;
//
//	if (bufChannels > 2) {
//		for (int j=0; j<remain; ++j) {
//			for (uint32 i=0; i<bufChannels; ++i) {
//				*bufData++ = *++(in[i]);
//			}
//		}
//
//		bufData = buf->data;
//
//		for (int j=0; j<wrap; ++j) {
//			for (uint32 i=0; i<bufChannels; ++i) {
//				*bufData++ = *++(in[i]);
//			}
//		}
//	} else if (bufChannels == 2) {
//		float *in0 = in[0];
//		float *in1 = in[1];
//		for (int j=0; j<remain; ++j) {
//			*bufData++ = *++in0;
//			*bufData++ = *++in1;
//		}
//
//		bufData = buf->data;
//
//		for (int j=0; j<wrap; ++j) {
//			*bufData++ = *++in0;
//			*bufData++ = *++in1;
//		}
//	} else {
//		float *in0 = in[0];
//		for (int j=0; j<remain; ++j) {
//			*bufData++ = *++in0;
//		}
//
//		bufData = buf->data;
//
//		for (int j=0; j<wrap; ++j) {
//			*bufData++ = *++in0;
//		}
//	}
//
//	unit->m_framepos += inNumSamples;
//	unit->m_framecount += inNumSamples;
//
//	if (unit->m_framecount >= bufFrames) {
//		bufupdates->writes++;
//		unit->m_framecount = 0;
//	}
//}
//
//
//
//void ScopeOut_Ctor(ScopeOut *unit)
//{
//	unit->m_fbufnum = -1e9;
//	unit->m_framepos = 0;
//	unit->m_framecount = 0;
//	unit->mIn = 0;
//	SETCALC(ScopeOut_next);
//}
//
//void ScopeOut_Dtor(ScopeOut *unit)
//{
//	TAKEDOWN_IN
//}
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//struct PitchShift : public Unit
//{
//	float *dlybuf;
//	float dsamp1, dsamp1_slope, ramp1, ramp1_slope;
//	float dsamp2, dsamp2_slope, ramp2, ramp2_slope;
//	float dsamp3, dsamp3_slope, ramp3, ramp3_slope;
//	float dsamp4, dsamp4_slope, ramp4, ramp4_slope;
//	float fdelaylen, slope;
//	long iwrphase, idelaylen, mask;
//	long counter, stage, numoutput, framesize;
//};
//
//void PitchShift_next(PitchShift *unit, int inNumSamples);
//void PitchShift_next(PitchShift *unit, int inNumSamples)
//{
//	float *out, *in, *dlybuf;
//	float disppchratio, pchratio, pchratio1, value;
//	float dsamp1, dsamp1_slope, ramp1, ramp1_slope;
//	float dsamp2, dsamp2_slope, ramp2, ramp2_slope;
//	float dsamp3, dsamp3_slope, ramp3, ramp3_slope;
//	float dsamp4, dsamp4_slope, ramp4, ramp4_slope;
//	float fdelaylen, d1, d2, frac, slope, samp_slope, startpos, winsize, pchdisp, timedisp;
//	long remain, nsmps, idelaylen, irdphase, irdphaseb, iwrphase, mask, idsamp;
//	long counter, stage, framesize;
//
//	RGET
//
//	out = ZOUT(0);
//	in = ZIN(0);
//
//	pchratio = ZIN0(2);
//	winsize = ZIN0(1);
//	pchdisp = ZIN0(3);
//	timedisp = ZIN0(4);
//	timedisp = sc_clip(timedisp, 0.f, winsize) * SAMPLERATE;
//
//	dlybuf = unit->dlybuf;
//	fdelaylen = unit->fdelaylen;
//	idelaylen = unit->idelaylen;
//	iwrphase = unit->iwrphase;
//
//	counter = unit->counter;
//	stage = unit->stage;
//	mask = unit->mask;
//	framesize = unit->framesize;
//
//	dsamp1 = unit->dsamp1;
//	dsamp2 = unit->dsamp2;
//	dsamp3 = unit->dsamp3;
//	dsamp4 = unit->dsamp4;
//
//	dsamp1_slope = unit->dsamp1_slope;
//	dsamp2_slope = unit->dsamp2_slope;
//	dsamp3_slope = unit->dsamp3_slope;
//	dsamp4_slope = unit->dsamp4_slope;
//
//	ramp1 = unit->ramp1;
//	ramp2 = unit->ramp2;
//	ramp3 = unit->ramp3;
//	ramp4 = unit->ramp4;
//
//	ramp1_slope = unit->ramp1_slope;
//	ramp2_slope = unit->ramp2_slope;
//	ramp3_slope = unit->ramp3_slope;
//	ramp4_slope = unit->ramp4_slope;
//
//	slope = unit->slope;
//
//	remain = inNumSamples;
//	while (remain) {
//		if (counter <= 0) {
//			counter = framesize >> 2;
//			unit->stage = stage = (stage + 1) & 3;
//			disppchratio = pchratio;
//			if (pchdisp != 0.f) {
//				disppchratio += (pchdisp * frand2(s1,s2,s3));
//			}
//			disppchratio = sc_clip(disppchratio, 0.f, 4.f);
//			pchratio1 = disppchratio - 1.f;
//			samp_slope = -pchratio1;
//			startpos = pchratio1 < 0.f ? 2.f : framesize * pchratio1 + 2.f;
//			startpos += (timedisp * frand(s1,s2,s3));
//			switch(stage) {
//				case 0 :
//					unit->dsamp1_slope = dsamp1_slope = samp_slope;
//					dsamp1 = startpos;
//					ramp1 = 0.0;
//					unit->ramp1_slope = ramp1_slope =  slope;
//					unit->ramp3_slope = ramp3_slope = -slope;
//					break;
//				case 1 :
//					unit->dsamp2_slope = dsamp2_slope = samp_slope;
//					dsamp2 = startpos;
//					ramp2 = 0.0;
//					unit->ramp2_slope = ramp2_slope =  slope;
//					unit->ramp4_slope = ramp4_slope = -slope;
//					break;
//				case 2 :
//					unit->dsamp3_slope = dsamp3_slope = samp_slope;
//					dsamp3 = startpos;
//					ramp3 = 0.0;
//					unit->ramp3_slope = ramp3_slope =  slope;
//					unit->ramp1_slope = ramp1_slope = -slope;
//					break;
//				case 3 :
//					unit->dsamp4_slope = dsamp4_slope = samp_slope;
//					dsamp4 = startpos;
//					ramp4 = 0.0;
//					unit->ramp2_slope = ramp2_slope = -slope;
//					unit->ramp4_slope = ramp4_slope =  slope;
//					break;
//			}
//		/*Print("%d %d    %g %g %g %g    %g %g %g %g    %g %g %g %g\n",
//			counter, stage, dsamp1_slope, dsamp2_slope, dsamp3_slope, dsamp4_slope,
//				dsamp1, dsamp2, dsamp3, dsamp4,
//				ramp1, ramp2, ramp3, ramp4);*/
//
//		}
//
//		nsmps = sc_min(remain, counter);
//		remain -= nsmps;
//		counter -= nsmps;
//
//		LOOP(nsmps,
//			iwrphase = (iwrphase + 1) & mask;
//
//			dsamp1 += dsamp1_slope;
//			idsamp = (long)dsamp1;
//			frac = dsamp1 - idsamp;
//			irdphase = (iwrphase - idsamp) & mask;
//			irdphaseb = (irdphase - 1) & mask;
//			d1 = dlybuf[irdphase];
//			d2 = dlybuf[irdphaseb];
//			value = (d1 + frac * (d2 - d1)) * ramp1;
//			ramp1 += ramp1_slope;
//
//			dsamp2 += dsamp2_slope;
//			idsamp = (long)dsamp2;
//			frac = dsamp2 - idsamp;
//			irdphase = (iwrphase - idsamp) & mask;
//			irdphaseb = (irdphase - 1) & mask;
//			d1 = dlybuf[irdphase];
//			d2 = dlybuf[irdphaseb];
//			value += (d1 + frac * (d2 - d1)) * ramp2;
//			ramp2 += ramp2_slope;
//
//			dsamp3 += dsamp3_slope;
//			idsamp = (long)dsamp3;
//			frac = dsamp3 - idsamp;
//			irdphase = (iwrphase - idsamp) & mask;
//			irdphaseb = (irdphase - 1) & mask;
//			d1 = dlybuf[irdphase];
//			d2 = dlybuf[irdphaseb];
//			value += (d1 + frac * (d2 - d1)) * ramp3;
//			ramp3 += ramp3_slope;
//
//			dsamp4 += dsamp4_slope;
//			idsamp = (long)dsamp4;
//			frac = dsamp4 - idsamp;
//			irdphase = (iwrphase - idsamp) & mask;
//			irdphaseb = (irdphase - 1) & mask;
//			d1 = dlybuf[irdphase];
//			d2 = dlybuf[irdphaseb];
//			value += (d1 + frac * (d2 - d1)) * ramp4;
//			ramp4 += ramp4_slope;
//
//			dlybuf[iwrphase] = ZXP(in);
//			ZXP(out) = value *= 0.5;
//		);
//	}
//
//	unit->counter = counter;
//
//	unit->dsamp1 = dsamp1;
//	unit->dsamp2 = dsamp2;
//	unit->dsamp3 = dsamp3;
//	unit->dsamp4 = dsamp4;
//
//	unit->ramp1 = ramp1;
//	unit->ramp2 = ramp2;
//	unit->ramp3 = ramp3;
//	unit->ramp4 = ramp4;
//
//	unit->iwrphase = iwrphase;
//
//	RPUT
//}
//
//
//
//
//void PitchShift_next_z(PitchShift *unit, int inNumSamples);
//void PitchShift_next_z(PitchShift *unit, int inNumSamples)
//{
//	float *out, *in, *dlybuf;
//	float disppchratio, pchratio, pchratio1, value;
//	float dsamp1, dsamp1_slope, ramp1, ramp1_slope;
//	float dsamp2, dsamp2_slope, ramp2, ramp2_slope;
//	float dsamp3, dsamp3_slope, ramp3, ramp3_slope;
//	float dsamp4, dsamp4_slope, ramp4, ramp4_slope;
//	float fdelaylen, d1, d2, frac, slope, samp_slope, startpos, winsize, pchdisp, timedisp;
//	long remain, nsmps, idelaylen, irdphase, irdphaseb, iwrphase;
//	long mask, idsamp;
//	long counter, stage, framesize, numoutput;
//
//	RGET
//
//	out = ZOUT(0);
//	in = ZIN(0);
//	pchratio = ZIN0(2);
//	winsize = ZIN0(1);
//	pchdisp = ZIN0(3);
//	timedisp = ZIN0(4);
//	timedisp = sc_clip(timedisp, 0.f, winsize) * SAMPLERATE;
//
//	dlybuf = unit->dlybuf;
//	fdelaylen = unit->fdelaylen;
//	idelaylen = unit->idelaylen;
//	iwrphase = unit->iwrphase;
//	numoutput = unit->numoutput;
//
//	counter = unit->counter;
//	stage = unit->stage;
//	mask = unit->mask;
//	framesize = unit->framesize;
//
//	dsamp1 = unit->dsamp1;
//	dsamp2 = unit->dsamp2;
//	dsamp3 = unit->dsamp3;
//	dsamp4 = unit->dsamp4;
//
//	dsamp1_slope = unit->dsamp1_slope;
//	dsamp2_slope = unit->dsamp2_slope;
//	dsamp3_slope = unit->dsamp3_slope;
//	dsamp4_slope = unit->dsamp4_slope;
//
//	ramp1 = unit->ramp1;
//	ramp2 = unit->ramp2;
//	ramp3 = unit->ramp3;
//	ramp4 = unit->ramp4;
//
//	ramp1_slope = unit->ramp1_slope;
//	ramp2_slope = unit->ramp2_slope;
//	ramp3_slope = unit->ramp3_slope;
//	ramp4_slope = unit->ramp4_slope;
//
//	slope = unit->slope;
//
//	remain = inNumSamples;
//	while (remain) {
//		if (counter <= 0) {
//			counter = framesize >> 2;
//			unit->stage = stage = (stage + 1) & 3;
//			disppchratio = pchratio;
//			if (pchdisp != 0.f) {
//				disppchratio += (pchdisp * frand2(s1,s2,s3));
//			}
//			disppchratio = sc_clip(disppchratio, 0.f, 4.f);
//			pchratio1 = disppchratio - 1.f;
//			samp_slope = -pchratio1;
//			startpos = pchratio1 < 0.f ? 2.f : framesize * pchratio1 + 2.f;
//			startpos += (timedisp * frand(s1,s2,s3));
//			switch(stage) {
//				case 0 :
//					unit->dsamp1_slope = dsamp1_slope = samp_slope;
//					dsamp1 = startpos;
//					ramp1 = 0.0;
//					unit->ramp1_slope = ramp1_slope =  slope;
//					unit->ramp3_slope = ramp3_slope = -slope;
//					break;
//				case 1 :
//					unit->dsamp2_slope = dsamp2_slope = samp_slope;
//					dsamp2 = startpos;
//					ramp2 = 0.0;
//					unit->ramp2_slope = ramp2_slope =  slope;
//					unit->ramp4_slope = ramp4_slope = -slope;
//					break;
//				case 2 :
//					unit->dsamp3_slope = dsamp3_slope = samp_slope;
//					dsamp3 = startpos;
//					ramp3 = 0.0;
//					unit->ramp3_slope = ramp3_slope =  slope;
//					unit->ramp1_slope = ramp1_slope = -slope;
//					break;
//				case 3 :
//					unit->dsamp4_slope = dsamp4_slope = samp_slope;
//					dsamp4 = startpos;
//					ramp4 = 0.0;
//					unit->ramp2_slope = ramp2_slope = -slope;
//					unit->ramp4_slope = ramp4_slope =  slope;
//					break;
//			}
//		/*Print("z %d %d    %g %g %g %g    %g %g %g %g    %g %g %g %g\n",
//			counter, stage, dsamp1_slope, dsamp2_slope, dsamp3_slope, dsamp4_slope,
//				dsamp1, dsamp2, dsamp3, dsamp4,
//				ramp1, ramp2, ramp3, ramp4);*/
//		}
//		nsmps = sc_min(remain, counter);
//		remain -= nsmps;
//		counter -= nsmps;
//
//		while (nsmps--) {
//			numoutput++;
//			iwrphase = (iwrphase + 1) & mask;
//
//			dsamp1 += dsamp1_slope;
//			idsamp = (long)dsamp1;
//			frac = dsamp1 - idsamp;
//			irdphase = (iwrphase - idsamp) & mask;
//			irdphaseb = (irdphase - 1) & mask;
//			if (numoutput < idelaylen) {
//				if (irdphase > iwrphase) {
//					value = 0.f;
//				} else if (irdphaseb > iwrphase) {
//					d1 = dlybuf[irdphase];
//					value = (d1 - frac * d1) * ramp1;
//				} else {
//					d1 = dlybuf[irdphase];
//					d2 = dlybuf[irdphaseb];
//					value = (d1 + frac * (d2 - d1)) * ramp1;
//				}
//			} else {
//				d1 = dlybuf[irdphase];
//				d2 = dlybuf[irdphaseb];
//				value = (d1 + frac * (d2 - d1)) * ramp1;
//			}
//			ramp1 += ramp1_slope;
//
//			dsamp2 += dsamp2_slope;
//			idsamp = (long)dsamp2;
//			frac = dsamp2 - idsamp;
//			irdphase = (iwrphase - idsamp) & mask;
//			irdphaseb = (irdphase - 1) & mask;
//			if (numoutput < idelaylen) {
//				if (irdphase > iwrphase) {
//					//value += 0.f;
//				} else if (irdphaseb > iwrphase) {
//					d1 = dlybuf[irdphase];
//					value += (d1 - frac * d1) * ramp2;
//				} else {
//					d1 = dlybuf[irdphase];
//					d2 = dlybuf[irdphaseb];
//					value += (d1 + frac * (d2 - d1)) * ramp2;
//				}
//			} else {
//				d1 = dlybuf[irdphase];
//				d2 = dlybuf[irdphaseb];
//				value += (d1 + frac * (d2 - d1)) * ramp2;
//			}
//			ramp2 += ramp2_slope;
//
//			dsamp3 += dsamp3_slope;
//			idsamp = (long)dsamp3;
//			frac = dsamp3 - idsamp;
//			irdphase = (iwrphase - idsamp) & mask;
//			irdphaseb = (irdphase - 1) & mask;
//			if (numoutput < idelaylen) {
//				if (irdphase > iwrphase) {
//					//value += 0.f;
//				} else if (irdphaseb > iwrphase) {
//					d1 = dlybuf[irdphase];
//					value += (d1 - frac * d1) * ramp3;
//				} else {
//					d1 = dlybuf[irdphase];
//					d2 = dlybuf[irdphaseb];
//					value += (d1 + frac * (d2 - d1)) * ramp3;
//				}
//			} else {
//				d1 = dlybuf[irdphase];
//				d2 = dlybuf[irdphaseb];
//				value += (d1 + frac * (d2 - d1)) * ramp3;
//			}
//			ramp3 += ramp3_slope;
//
//			dsamp4 += dsamp4_slope;
//			idsamp = (long)dsamp4;
//			frac = dsamp4 - idsamp;
//			irdphase = (iwrphase - idsamp) & mask;
//			irdphaseb = (irdphase - 1) & mask;
//
//			if (numoutput < idelaylen) {
//				if (irdphase > iwrphase) {
//					//value += 0.f;
//				} else if (irdphaseb > iwrphase) {
//					d1 = dlybuf[irdphase];
//					value += (d1 - frac * d1) * ramp4;
//				} else {
//					d1 = dlybuf[irdphase];
//					d2 = dlybuf[irdphaseb];
//					value += (d1 + frac * (d2 - d1)) * ramp4;
//				}
//			} else {
//				d1 = dlybuf[irdphase];
//				d2 = dlybuf[irdphaseb];
//				value += (d1 + frac * (d2 - d1)) * ramp4;
//			}
//			ramp4 += ramp4_slope;
//
//			dlybuf[iwrphase] = ZXP(in);
//			ZXP(out) = value *= 0.5;
//		}
//	}
//
//	unit->counter = counter;
//	unit->stage = stage;
//	unit->mask = mask;
//
//	unit->dsamp1 = dsamp1;
//	unit->dsamp2 = dsamp2;
//	unit->dsamp3 = dsamp3;
//	unit->dsamp4 = dsamp4;
//
//	unit->ramp1 = ramp1;
//	unit->ramp2 = ramp2;
//	unit->ramp3 = ramp3;
//	unit->ramp4 = ramp4;
//
//	unit->numoutput = numoutput;
//	unit->iwrphase = iwrphase;
//
//	if (numoutput >= idelaylen) {
//		SETCALC(PitchShift_next);
//	}
//
//	RPUT
//}
//
//
//void PitchShift_Ctor(PitchShift *unit);
//void PitchShift_Ctor(PitchShift *unit)
//{
//	long delaybufsize;
//	float *out, *in, *dlybuf;
//	float winsize, pchratio;
//	float fdelaylen, slope;
//	long framesize, last;
//
//	out = ZOUT(0);
//	in = ZIN(0);
//	pchratio = ZIN0(2);
//	winsize = ZIN0(1);
//
//	delaybufsize = (long)ceil(winsize * SAMPLERATE * 3.f + 3.f);
//	fdelaylen = delaybufsize - 3;
//
//	delaybufsize = delaybufsize + BUFLENGTH;
//	delaybufsize = NEXTPOWEROFTWO(delaybufsize);  // round up to next power of two
//	dlybuf = (float*)RTAlloc(unit->mWorld, delaybufsize * sizeof(float));
//
//	SETCALC(PitchShift_next_z);
//
//	*dlybuf = ZIN0(0);
//	ZOUT0(0) = 0.f;
//
//	unit->dlybuf = dlybuf;
//	unit->idelaylen = delaybufsize;
//	unit->fdelaylen = fdelaylen;
//	unit->iwrphase = 0;
//	unit->numoutput = 0;
//	unit->mask = last = (delaybufsize - 1);
//
//	unit->framesize = framesize = ((long)(winsize * SAMPLERATE) + 2) & ~3;
//	unit->slope = slope = 2.f / framesize;
//	unit->stage = 3;
//	unit->counter = framesize >> 2;
//	unit->ramp1 = 0.5;
//	unit->ramp2 = 1.0;
//	unit->ramp3 = 0.5;
//	unit->ramp4 = 0.0;
//
//	unit->ramp1_slope = -slope;
//	unit->ramp2_slope = -slope;
//	unit->ramp3_slope =  slope;
//	unit->ramp4_slope =  slope;
//
//	dlybuf[last  ] = 0.f; // put a few zeroes where we start the read heads
//	dlybuf[last-1] = 0.f;
//	dlybuf[last-2] = 0.f;
//
//	unit->numoutput = 0;
//
//	// start all read heads 2 samples behind the write head
//	unit->dsamp1 = unit->dsamp2 = unit->dsamp3 = unit->dsamp4 = 2.f;
//	// pch ratio is initially zero for the read heads
//	unit->dsamp1_slope = unit->dsamp2_slope = unit->dsamp3_slope = unit->dsamp4_slope = 1.f;
//}
//
//
//
//void PitchShift_Dtor(PitchShift *unit)
//{
//	RTFree(unit->mWorld, unit->dlybuf);
//}
//
//
//
//
//typedef struct graintap1 {
//	float pos, rate, level, slope, curve;
//	long counter;
//	struct graintap1 *next;
//} GrainTap1;
//
//#define MAXDGRAINS 32
//
//struct GrainTap : public Unit
//{
//	float m_fbufnum;
//	SndBuf *m_buf;
//
//	float fdelaylen;
//	long bufsize, iwrphase;
//	long nextTime;
//	GrainTap1 grains[MAXDGRAINS];
//	GrainTap1 *firstActive, *firstFree;
//};
//
//
//// coefs: pos, rate, level, slope, curve, counter
//
//void GrainTap_next(GrainTap *unit, int inNumSamples);
//void GrainTap_next(GrainTap *unit, int inNumSamples)
//{
//	float *out, *out0;
//	const float * dlybuf;
//	float sdur, rdur, rdur2;
//	float dsamp, dsamp_slope, fdelaylen, d1, d2, frac;
//	float level, slope, curve;
//	float maxpitch, pitch, maxtimedisp, timedisp, density;
//	long remain, nsmps, irdphase, irdphaseb, iwrphase, iwrphase0;
//	long idsamp, koffset;
//	long counter;
//	uint32 bufsize;
//	GrainTap1 *grain, *prevGrain, *nextGrain;
//
//	GET_BUF_SHARED
//
//	RGET
//
//	out0 = ZOUT(0);
//
//	// bufnum, grainDur, pchRatio, pchDisp, timeDisp, overlap
//	// 0       1         2         3        4         5
//
//	density = ZIN0(5);
//	density = sc_max(0.0001, density);
//
//	bufsize = unit->bufsize;
//	if (bufsize != bufSamples) {
//		ClearUnitOutputs(unit, inNumSamples);
//		return;
//	}
//
//	dlybuf = bufData;
//	fdelaylen = unit->fdelaylen;
//	iwrphase0 = unit->iwrphase;
//
//	// initialize buffer to zero
//	out = out0;
//	LOOP1(inNumSamples, ZXP(out) = 0.f;);
//
//	// do all current grains
//	prevGrain = NULL;
//	grain = unit->firstActive;
//	while (grain) {
//
//		dsamp = grain->pos;
//		dsamp_slope = grain->rate;
//		level = grain->level;
//		slope = grain->slope;
//		curve = grain->curve;
//		counter = grain->counter;
//
//		nsmps = sc_min(counter, inNumSamples);
//		iwrphase = iwrphase0;
//		out = out0;
//		LOOP(nsmps,
//			dsamp += dsamp_slope;
//			idsamp = (long)dsamp;
//			frac = dsamp - idsamp;
//			iwrphase = (iwrphase + 1) & mask;
//			irdphase = (iwrphase - idsamp) & mask;
//			irdphaseb = (irdphase - 1) & mask;
//			d1 = dlybuf[irdphase];
//			d2 = dlybuf[irdphaseb];
//			ZXP(out) += (d1 + frac * (d2 - d1)) * level;
//			level += slope;
//			slope += curve;
//		);
//		grain->pos = dsamp;
//		grain->level = level;
//		grain->slope = slope;
//		grain->counter -= nsmps;
//
//		nextGrain = grain->next;
//		if (grain->counter <= 0) {
//			// unlink from active list
//			if (prevGrain) prevGrain->next = nextGrain;
//			else unit->firstActive = nextGrain;
//
//			// link onto free list
//			grain->next = unit->firstFree;
//			unit->firstFree = grain;
//		} else {
//			prevGrain = grain;
//		}
//		grain = nextGrain;
//	}
//	// start new grains
//	remain = inNumSamples;
//	while (unit->nextTime <= remain) {
//		remain -= unit->nextTime;
//		sdur = ZIN0(1) * SAMPLERATE;
//		sdur = sc_max(sdur, 4.f);
//
//		grain = unit->firstFree;
//		if (grain) {
//			unit->firstFree = grain->next;
//			grain->next = unit->firstActive;
//			unit->firstActive = grain;
//
//			koffset = inNumSamples - remain;
//			iwrphase = (iwrphase0 + koffset) & mask;
//
//			grain->counter = (long)sdur;
//
//			timedisp = ZIN0(4);
//			timedisp = sc_max(timedisp, 0.f);
//			timedisp = frand(s1,s2,s3) * timedisp * SAMPLERATE;
//
//			pitch = ZIN0(2) + frand2(s1,s2,s3) * ZIN0(3);
//			if (pitch >= 1.f) {
//				maxpitch = 1.f + (fdelaylen/sdur);
//				pitch = sc_min(pitch, maxpitch);
//
//				dsamp_slope = 1.f - pitch;
//				grain->rate = dsamp_slope;
//
//				maxtimedisp = fdelaylen + sdur * dsamp_slope;
//				timedisp = sc_min(timedisp, maxtimedisp);
//
//				dsamp = BUFLENGTH + koffset + 2.f + timedisp - sdur * dsamp_slope;
//				dsamp = sc_min(dsamp, fdelaylen);
//			} else {
//				maxpitch = -(1.f + (fdelaylen/sdur));
//				pitch = sc_max(pitch, maxpitch);
//
//				dsamp_slope = 1.f - pitch;
//				grain->rate = dsamp_slope;
//
//				maxtimedisp = fdelaylen - sdur * dsamp_slope;
//				timedisp = sc_min(timedisp, maxtimedisp);
//
//				dsamp = BUFLENGTH + koffset + 2.f + timedisp;
//				dsamp = sc_min(dsamp, fdelaylen);
//			}
//
//			grain->pos = dsamp;
//			//postbuf("ds %g %g %g\n", dsamp_slope, dsamp, fdelaylen);
//
//			rdur = 1.f / sdur;
//			rdur2 = rdur * rdur;
//			grain->level = level = 0.f;
//			grain->slope = slope = 4.0 * (rdur - rdur2);	// ampslope
//			grain->curve = curve = -8.0 * rdur2;			// ampcurve
//
//			nsmps = remain;
//			out = out0 + koffset;
//			LOOP(nsmps,
//				dsamp += dsamp_slope;
//				idsamp = (long)dsamp;
//				frac = dsamp - idsamp;
//				iwrphase = (iwrphase + 1) & mask;
//				irdphase = (iwrphase - idsamp) & mask;
//				irdphaseb = (irdphase - 1) & mask;
//				d1 = dlybuf[irdphase];
//				d2 = dlybuf[irdphaseb];
//				ZXP(out) += (d1 + frac * (d2 - d1)) * level;
//				level += slope;
//				slope += curve;
//			);
//			grain->pos = dsamp;
//			grain->level = level;
//			grain->slope = slope;
//			grain->counter -= nsmps;
//
//			if (grain->counter <= 0) {
//				// unlink from active list
//				unit->firstActive = grain->next;
//
//				// link onto free list
//				grain->next = unit->firstFree;
//				unit->firstFree = grain;
//			}
//		}
//		unit->nextTime = (long)(sdur / density);
//		if (unit->nextTime < 1) unit->nextTime = 1;
//
//		/*if (grain == NULL) {
//			postbuf("nextTime %d %g %g %08X %08X %08X\n", unit->nextTime, sdur, density,
//				grain, unit->firstActive, unit->firstFree);
//		}*/
//	}
//	iwrphase = (iwrphase0 + BUFLENGTH) & mask;
//	unit->nextTime -= remain;
//	if (unit->nextTime < 0) unit->nextTime = 0;
//
//	unit->iwrphase = iwrphase;
//
//	RPUT
//}
//
//
//void GrainTap_Ctor(GrainTap *unit);
//void GrainTap_Ctor(GrainTap *unit)
//{
//	float fdelaylen;
//	float maxdelaytime;
//
//	GET_BUF
//
//	if (!ISPOWEROFTWO(bufSamples)) {
//		Print("GrainTap buffer size not a power of two.\n");
//		SETCALC(*ClearUnitOutputs);
//		return;
//	}
//
//	fdelaylen = bufSamples - 2 * BUFLENGTH - 3;
//	maxdelaytime = fdelaylen * SAMPLEDUR;
//
//	SETCALC(GrainTap_next);
//
//	ZOUT0(0) = 0.f;
//
//	unit->bufsize = bufSamples;
//	unit->fdelaylen = fdelaylen;
//	unit->iwrphase = 0;
//	unit->nextTime = 0;
//	for (int i=0; i<MAXDGRAINS-1; ++i) {
//		unit->grains[i].next = unit->grains + (i + 1);
//	}
//	unit->grains[MAXDGRAINS-1].next = NULL;
//	unit->firstFree = unit->grains;
//	unit->firstActive = NULL;
//}
//
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//#define GRAIN_BUF \
//	const SndBuf *buf = bufs + bufnum; \
//	LOCK_SNDBUF_SHARED(buf); \
//	const float *bufData __attribute__((__unused__)) = buf->data; \
//	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
//	uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
//	uint32 bufFrames = buf->frames; \
//	int guardFrame __attribute__((__unused__)) = bufFrames - 2; \
//
//
//inline float IN_AT(Unit* unit, int index, int offset)
//{
//	if (INRATE(index) == calc_FullRate) return IN(index)[offset];
//	if (INRATE(index) == calc_DemandRate) return DEMANDINPUT_A(index, offset + 1);
//	return ZIN0(index);
//}
//
//inline double sc_gloop(double in, double hi)
//{
//	// avoid the divide if possible
//	if (in >= hi) {
//		in -= hi;
//		if (in < hi) return in;
//	} else if (in < 0.) {
//		in += hi;
//		if (in >= 0.) return in;
//	} else return in;
//
//	return in - hi * floor(in/hi);
//}
//
//#define GRAIN_LOOP_BODY_4 \
//		float amp = y1 * y1; \
//		phase = sc_gloop(phase, loopMax); \
//		int32 iphase = (int32)phase; \
//		const float* table1 = bufData + iphase; \
//		const float* table0 = table1 - 1; \
//		const float* table2 = table1 + 1; \
//		const float* table3 = table1 + 2; \
//		if (iphase == 0) { \
//			table0 += bufSamples; \
//		} else if (iphase >= guardFrame) { \
//			if (iphase == guardFrame) { \
//				table3 -= bufSamples; \
//			} else { \
//				table2 -= bufSamples; \
//				table3 -= bufSamples; \
//			} \
//		} \
//		float fracphase = phase - (double)iphase; \
//		float a = table0[0]; \
//		float b = table1[0]; \
//		float c = table2[0]; \
//		float d = table3[0]; \
//		float outval = amp * cubicinterp(fracphase, a, b, c, d); \
//		ZXP(out1) += outval * pan1; \
//		ZXP(out2) += outval * pan2; \
//		double y0 = b1 * y1 - y2; \
//		y2 = y1; \
//		y1 = y0; \
//
//
//#define GRAIN_LOOP_BODY_2 \
//		float amp = y1 * y1; \
//		phase = sc_gloop(phase, loopMax); \
//		int32 iphase = (int32)phase; \
//		const float* table1 = bufData + iphase; \
//		const float* table2 = table1 + 1; \
//		if (iphase > guardFrame) { \
//			table2 -= bufSamples; \
//		} \
//		float fracphase = phase - (double)iphase; \
//		float b = table1[0]; \
//		float c = table2[0]; \
//		float outval = amp * (b + fracphase * (c - b)); \
//		ZXP(out1) += outval * pan1; \
//		ZXP(out2) += outval * pan2; \
//		double y0 = b1 * y1 - y2; \
//		y2 = y1; \
//		y1 = y0; \
//
//
//#define GRAIN_LOOP_BODY_1 \
//		float amp = y1 * y1; \
//		phase = sc_gloop(phase, loopMax); \
//		int32 iphase = (int32)phase; \
//		float outval = amp * bufData[iphase]; \
//		ZXP(out1) += outval * pan1; \
//		ZXP(out2) += outval * pan2; \
//		double y0 = b1 * y1 - y2; \
//		y2 = y1; \
//		y1 = y0; \
//
//
//void TGrains_next(TGrains *unit, int inNumSamples)
//{
//	float *trigin = IN(0);
//	float prevtrig = unit->mPrevTrig;
//
//	uint32 numOutputs = unit->mNumOutputs;
//	ClearUnitOutputs(unit, inNumSamples);
//	float *out[16];
//	for (uint32 i=0; i<numOutputs; ++i) out[i] = ZOUT(i);
//
//	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
//	uint32 numBufs = world->mNumSndBufs;
//
//	for (int i=0; i < unit->mNumActive; ) {
//		Grain *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;
//
//		GRAIN_BUF
//
//		if (bufChannels != 1) {
//			 ++i;
//			 continue;
//		}
//
//		double loopMax = (double)bufFrames;
//
//		float pan1 = grain->pan1;
//		float pan2 = grain->pan2;
//		double rate = grain->rate;
//		double phase = grain->phase;
//		double b1 = grain->b1;
//		double y1 = grain->y1;
//		double y2 = grain->y2;
//
//		uint32 chan1 = grain->chan;
//		uint32 chan2 = chan1 + 1;
//		if (chan2 >= numOutputs) chan2 = 0;
//
//		float *out1 = out[chan1];
//		float *out2 = out[chan2];
//		//printf("B chan %d %d  %08X %08X", chan1, chan2, out1, out2);
//
//		int nsmps = sc_min(grain->counter, inNumSamples);
//		if (grain->interp >= 4) {
//			for (int j=0; j<nsmps; ++j) {
//				GRAIN_LOOP_BODY_4;
//				phase += rate;
//			}
//		} else if (grain->interp >= 2) {
//			for (int j=0; j<nsmps; ++j) {
//				GRAIN_LOOP_BODY_2;
//				phase += rate;
//			}
//		} else {
//			for (int j=0; j<nsmps; ++j) {
//				GRAIN_LOOP_BODY_1;
//				phase += rate;
//			}
//		}
//
//		grain->phase = phase;
//		grain->y1 = y1;
//		grain->y2 = y2;
//
//		grain->counter -= nsmps;
//		if (grain->counter <= 0) {
//			// remove grain
//			*grain = unit->mGrains[--unit->mNumActive];
//		} else ++i;
//	}
//
//	int trigSamples = INRATE(0) == calc_FullRate ? inNumSamples : 1;
//
//	for (int i=0; i<trigSamples; ++i) {
//		float trig = trigin[i];
//
//		if (trig > 0.f && prevtrig <= 0.f) {
//			// start a grain
//			if (unit->mNumActive+1 >= kMaxGrains) break;
//			uint32 bufnum = (uint32)IN_AT(unit, 1, i);
//			if (bufnum >= numBufs) continue;
//			GRAIN_BUF
//
//			if (bufChannels != 1) continue;
//
//			float bufSampleRate = buf->samplerate;
//			float bufRateScale = bufSampleRate * SAMPLEDUR;
//			double loopMax = (double)bufFrames;
//
//			Grain *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;
//
//			double counter = floor(IN_AT(unit, 4, i) * SAMPLERATE);
//			counter = sc_max(4., counter);
//			grain->counter = (int)counter;
//
//			double rate = grain->rate = IN_AT(unit, 2, i) * bufRateScale;
//			double centerPhase = IN_AT(unit, 3, i) * bufSampleRate;
//			double phase = centerPhase - 0.5 * counter * rate;
//
//			float pan = IN_AT(unit, 5, i);
//			float amp = IN_AT(unit, 6, i);
//			grain->interp = (int)IN_AT(unit, 7, i);
//
//			float panangle;
//			if (numOutputs > 2) {
//				pan = sc_wrap(pan * 0.5f, 0.f, 1.f);
//				float cpan = numOutputs * pan + 0.5;
//				float ipan = floor(cpan);
//				float panfrac = cpan - ipan;
//				panangle = panfrac * pi2_f;
//				grain->chan = (int)ipan;
//				if (grain->chan >= (int)numOutputs) grain->chan -= numOutputs;
//			} else {
//				grain->chan = 0;
//				pan = sc_wrap(pan * 0.5f + 0.5f, 0.f, 1.f);
//				panangle = pan * pi2_f;
//			}
//			float pan1 = grain->pan1 = amp * cos(panangle);
//			float pan2 = grain->pan2 = amp * sin(panangle);
//			double w = pi / counter;
//			double b1 = grain->b1 = 2. * cos(w);
//			double y1 = sin(w);
//			double y2 = 0.;
//
//			uint32 chan1 = grain->chan;
//			uint32 chan2 = chan1 + 1;
//			if (chan2 >= numOutputs) chan2 = 0;
//
//			float *out1 = out[chan1] + i;
//			float *out2 = out[chan2] + i;
//
//			int nsmps = sc_min(grain->counter, inNumSamples - i);
//			if (grain->interp >= 4) {
//				for (int j=0; j<nsmps; ++j) {
//					GRAIN_LOOP_BODY_4;
//					phase += rate;
//				}
//			} else if (grain->interp >= 2) {
//				for (int j=0; j<nsmps; ++j) {
//					GRAIN_LOOP_BODY_2;
//					phase += rate;
//				}
//			} else {
//				for (int j=0; j<nsmps; ++j) {
//					GRAIN_LOOP_BODY_1;
//					phase += rate;
//				}
//			}
//
//			grain->phase = phase;
//			grain->y1 = y1;
//			grain->y2 = y2;
//
//			grain->counter -= nsmps;
//			if (grain->counter <= 0) {
//				// remove grain
//				*grain = unit->mGrains[--unit->mNumActive];
//			}
//		}
//		prevtrig = trig;
//	}
//
//	unit->mPrevTrig = prevtrig;
//}
//
//void TGrains_Ctor(TGrains *unit)
//{
//	SETCALC(TGrains_next);
//
//	unit->mNumActive = 0;
//	unit->mPrevTrig = 0.;
//
//	ClearUnitOutputs(unit, 1);
//}
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///*
//Pluck - Karplus-Strong
//*/
//void Pluck_Ctor(Pluck *unit)
//{
////	FeedbackDelay_Reset(unit);
//	float maxdelaytime = unit->m_maxdelaytime = IN0(2);
//	float delaytime = unit->m_delaytime = IN0(3);
//	unit->m_dlybuf = 0;
//	DelayUnit_AllocDelayLine(unit);
//	unit->m_dsamp = CalcDelay(unit, unit->m_delaytime);
//
//	unit->m_numoutput = 0;
//	unit->m_iwrphase = 0;
//	unit->m_feedbk = sc_CalcFeedback(unit->m_delaytime, unit->m_decaytime);
//
//	if (INRATE(1) == calc_FullRate) {
//		if(INRATE(5) == calc_FullRate){
//			SETCALC(Pluck_next_aa_z);
//		} else {
//			SETCALC(Pluck_next_ak_z); //ak
//		}
//	} else {
//	    if(INRATE(5) == calc_FullRate){
//			SETCALC(Pluck_next_ka_z); //ka
//		} else {
//			SETCALC(Pluck_next_kk_z); //kk
//		}
//	}
//	OUT0(0) = unit->m_lastsamp = 0.f;
//	unit->m_prevtrig = 0.f;
//	unit->m_inputsamps = 0;
//	unit->m_coef = IN0(5);
//}
//
//void Pluck_next_aa(Pluck *unit, int inNumSamples)
//{
//	float *out = OUT(0);
//	float *in = IN(0);
//	float *trig = IN(1);
//	float delaytime = IN0(3);
//	float decaytime = IN0(4);
//	float *coef = IN(5);
//	float lastsamp = unit->m_lastsamp;
//	unsigned long inputsamps = unit->m_inputsamps;
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	float feedbk = unit->m_feedbk;
//	long mask = unit->m_mask;
//	float thisin, curtrig;
//	float prevtrig = unit->m_prevtrig;
//
//	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
//		long idsamp = (long)dsamp;
//		float frac = dsamp - idsamp;
//	for(int i = 0; i < inNumSamples; i++){
//			curtrig = trig[i];
//			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
//			    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//			    }
//			prevtrig = curtrig;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			float d0 = dlybuf[irdphase0 & mask];
//			float d1 = dlybuf[irdphase1 & mask];
//			float d2 = dlybuf[irdphase2 & mask];
//			float d3 = dlybuf[irdphase3 & mask];
//			float value = cubicinterp(frac, d0, d1, d2, d3);
//			float thiscoef = coef[i];
//			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			iwrphase++;
//		};
//	} else {
//
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//
//	    for(int i = 0; i < inNumSamples; i++){
//			curtrig = trig[i];
//			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
//			    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//			    }
//			prevtrig = curtrig;
//			dsamp += dsamp_slope;
//			long idsamp = (long)dsamp;
//			float frac = dsamp - idsamp;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			float d0 = dlybuf[irdphase0 & mask];
//			float d1 = dlybuf[irdphase1 & mask];
//			float d2 = dlybuf[irdphase2 & mask];
//			float d3 = dlybuf[irdphase3 & mask];
//			float value = cubicinterp(frac, d0, d1, d2, d3);
//			float thiscoef = coef[i];
//			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			feedbk += feedbk_slope;
//			iwrphase++;
//		};
//		unit->m_feedbk = feedbk;
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//		unit->m_decaytime = decaytime;
//	}
//
//	unit->m_prevtrig = prevtrig;
//	unit->m_inputsamps = inputsamps;
//	unit->m_lastsamp = zapgremlins(lastsamp);
//	unit->m_iwrphase = iwrphase;
//
//}
//
//
//void Pluck_next_aa_z(Pluck *unit, int inNumSamples)
//{
//	float *out = OUT(0);
//	float *in = IN(0);
//	float *trig = IN(1);
//	float delaytime = IN0(3);
//	float decaytime = IN0(4);
//	float *coef = IN(5);
//	float lastsamp = unit->m_lastsamp;
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	float feedbk = unit->m_feedbk;
//	long mask = unit->m_mask;
//	float d0, d1, d2, d3;
//	float thisin, curtrig;
//	unsigned long inputsamps = unit->m_inputsamps;
//	float prevtrig = unit->m_prevtrig;
//
//	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
//		long idsamp = (long)dsamp;
//		float frac = dsamp - idsamp;
//		for(int i = 0; i < inNumSamples; i++){
//			curtrig = trig[i];
//			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
//			    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//			    }
//			prevtrig = curtrig;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			if (irdphase0 < 0) {
//				dlybuf[iwrphase & mask] = thisin;
//				out[i] = 0.f;
//			} else {
//				if (irdphase1 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//				} else if (irdphase2 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//				} else if (irdphase3 < 0) {
//					d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//				} else {
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//					d3 = dlybuf[irdphase3 & mask];
//				}
//				float value = cubicinterp(frac, d0, d1, d2, d3);
//			float thiscoef = coef[i];
//			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			}
//			iwrphase++;
//		};
//	} else {
//
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//
//		for(int i = 0; i < inNumSamples; i++) {
//			curtrig = trig[i];
//			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
//			    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//			    }
//			prevtrig = curtrig;
//			dsamp += dsamp_slope;
//			long idsamp = (long)dsamp;
//			float frac = dsamp - idsamp;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			if (irdphase0 < 0) {
//				dlybuf[iwrphase & mask] = thisin;
//				out[i] = 0.f;
//			    } else {
//				if (irdphase1 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//				} else if (irdphase2 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//				} else if (irdphase3 < 0) {
//					d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//				} else {
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//					d3 = dlybuf[irdphase3 & mask];
//				}
//				float value = cubicinterp(frac, d0, d1, d2, d3);
//			float thiscoef = coef[i];
//			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			}
//			feedbk += feedbk_slope;
//			iwrphase++;
//		};
//		unit->m_feedbk = feedbk;
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//		unit->m_decaytime = decaytime;
//	}
//
//	unit->m_inputsamps = inputsamps;
//	unit->m_prevtrig = prevtrig;
//	unit->m_lastsamp = zapgremlins(lastsamp);
//	unit->m_iwrphase = iwrphase;
//
//	unit->m_numoutput += inNumSamples;
//	if (unit->m_numoutput >= unit->m_idelaylen) {
//		SETCALC(Pluck_next_aa);
//	}
//}
//
//void Pluck_next_kk(Pluck *unit, int inNumSamples)
//{
//	float *out = OUT(0);
//	float *in = IN(0);
//	float trig = IN0(1);
//	float delaytime = IN0(3);
//	float decaytime = IN0(4);
//	float coef = IN0(5);
//	float lastsamp = unit->m_lastsamp;
//	unsigned long inputsamps = unit->m_inputsamps;
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	float feedbk = unit->m_feedbk;
//	long mask = unit->m_mask;
//	float thisin;
//
//	if ((unit->m_prevtrig <= 0.f) && (trig > 0.f)) {
//	    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//	    }
//	unit->m_prevtrig = trig;
//
//	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime && coef == unit->m_coef) {
//		long idsamp = (long)dsamp;
//		float frac = dsamp - idsamp;
//
//	    for(int i = 0; i < inNumSamples; i++){
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			float d0 = dlybuf[irdphase0 & mask];
//			float d1 = dlybuf[irdphase1 & mask];
//			float d2 = dlybuf[irdphase2 & mask];
//			float d3 = dlybuf[irdphase3 & mask];
//			float value = cubicinterp(frac, d0, d1, d2, d3);
//			float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
//			out[i] = lastsamp = onepole; //value;
//			iwrphase++;
//		};
//	} else {
//
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//
//		float curcoef = unit->m_coef;
//		float coef_slope = CALCSLOPE(coef, curcoef);
//
//	    for(int i = 0; i < inNumSamples; i++){
//			dsamp += dsamp_slope;
//			long idsamp = (long)dsamp;
//			float frac = dsamp - idsamp;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			float d0 = dlybuf[irdphase0 & mask];
//			float d1 = dlybuf[irdphase1 & mask];
//			float d2 = dlybuf[irdphase2 & mask];
//			float d3 = dlybuf[irdphase3 & mask];
//			float value = cubicinterp(frac, d0, d1, d2, d3);
//			float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
//			out[i] = lastsamp = onepole; //value;
//			feedbk += feedbk_slope;
//			curcoef += coef_slope;
//			iwrphase++;
//		};
//		unit->m_feedbk = feedbk;
//		unit->m_coef = coef;
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//		unit->m_decaytime = decaytime;
//	}
//
//	unit->m_inputsamps = inputsamps;
//	unit->m_lastsamp = zapgremlins(lastsamp);
//	unit->m_iwrphase = iwrphase;
//
//}
//
//
//void Pluck_next_kk_z(Pluck *unit, int inNumSamples)
//{
//	float *out = OUT(0);
//	float *in = IN(0);
//	float trig = IN0(1);
//	float delaytime = IN0(3);
//	float decaytime = IN0(4);
//	float coef = IN0(5);
//	float lastsamp = unit->m_lastsamp;
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	float feedbk = unit->m_feedbk;
//	long mask = unit->m_mask;
//	float d0, d1, d2, d3;
//	float thisin;
//	unsigned long inputsamps = unit->m_inputsamps;
//
//	if ((unit->m_prevtrig <= 0.f) && (trig > 0.f)) {
//	    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//	    }
//	unit->m_prevtrig = trig;
//
//	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime && coef == unit->m_coef) {
//		long idsamp = (long)dsamp;
//		float frac = dsamp - idsamp;
//		for(int i = 0; i < inNumSamples; i++){
//
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			if (irdphase0 < 0) {
//				dlybuf[iwrphase & mask] = thisin;
//				out[i] = 0.f;
//			} else {
//				if (irdphase1 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//				} else if (irdphase2 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//				} else if (irdphase3 < 0) {
//					d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//				} else {
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//					d3 = dlybuf[irdphase3 & mask];
//				}
//				float value = cubicinterp(frac, d0, d1, d2, d3);
//			float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
//			out[i] = lastsamp = onepole; //value;
//			}
//			iwrphase++;
//		};
//	} else {
//
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//
//		float curcoef = unit->m_coef;
//		float coef_slope = CALCSLOPE(coef, curcoef);
//
//		for(int i = 0; i < inNumSamples; i++) {
//			dsamp += dsamp_slope;
//			long idsamp = (long)dsamp;
//			float frac = dsamp - idsamp;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			if (irdphase0 < 0) {
//				dlybuf[iwrphase & mask] = thisin;
//				out[i] = 0.f;
//			    } else {
//				if (irdphase1 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//				} else if (irdphase2 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//				} else if (irdphase3 < 0) {
//					d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//				} else {
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//					d3 = dlybuf[irdphase3 & mask];
//				}
//			float value = cubicinterp(frac, d0, d1, d2, d3);
//			float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
//			out[i] = lastsamp = onepole; //value;
//			}
//			feedbk += feedbk_slope;
//			curcoef += coef_slope;
//			iwrphase++;
//		};
//		unit->m_feedbk = feedbk;
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//		unit->m_decaytime = decaytime;
//		unit->m_coef = coef;
//	}
//
//	unit->m_inputsamps = inputsamps;
//	unit->m_lastsamp = zapgremlins(lastsamp);
//	unit->m_iwrphase = iwrphase;
//
//	unit->m_numoutput += inNumSamples;
//	if (unit->m_numoutput >= unit->m_idelaylen) {
//		SETCALC(Pluck_next_kk);
//	}
//}
//
//void Pluck_next_ak(Pluck *unit, int inNumSamples)
//{
//	float *out = OUT(0);
//	float *in = IN(0);
//	float *trig = IN(1);
//	float delaytime = IN0(3);
//	float decaytime = IN0(4);
//	float coef = IN0(5);
//	float lastsamp = unit->m_lastsamp;
//	unsigned long inputsamps = unit->m_inputsamps;
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	float feedbk = unit->m_feedbk;
//	long mask = unit->m_mask;
//	float thisin, curtrig;
//	float prevtrig = unit->m_prevtrig;
//
//	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
//		long idsamp = (long)dsamp;
//		float frac = dsamp - idsamp;
//	for(int i = 0; i < inNumSamples; i++){
//			curtrig = trig[i];
//			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
//			    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//			    }
//			prevtrig = curtrig;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			float d0 = dlybuf[irdphase0 & mask];
//			float d1 = dlybuf[irdphase1 & mask];
//			float d2 = dlybuf[irdphase2 & mask];
//			float d3 = dlybuf[irdphase3 & mask];
//			float value = cubicinterp(frac, d0, d1, d2, d3);
//			float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			iwrphase++;
//		};
//	} else {
//
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//
//		float curcoef = unit->m_coef;
//		float coef_slope = CALCSLOPE(coef, curcoef);
//
//	    for(int i = 0; i < inNumSamples; i++){
//			curtrig = trig[i];
//			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
//			    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//			    }
//			prevtrig = curtrig;
//			dsamp += dsamp_slope;
//			long idsamp = (long)dsamp;
//			float frac = dsamp - idsamp;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			float d0 = dlybuf[irdphase0 & mask];
//			float d1 = dlybuf[irdphase1 & mask];
//			float d2 = dlybuf[irdphase2 & mask];
//			float d3 = dlybuf[irdphase3 & mask];
//			float value = cubicinterp(frac, d0, d1, d2, d3);
//			float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			feedbk += feedbk_slope;
//			curcoef += coef_slope;
//			iwrphase++;
//		};
//		unit->m_feedbk = feedbk;
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//		unit->m_decaytime = decaytime;
//		unit->m_coef = coef;
//	}
//
//	unit->m_prevtrig = prevtrig;
//	unit->m_inputsamps = inputsamps;
//	unit->m_lastsamp = zapgremlins(lastsamp);
//	unit->m_iwrphase = iwrphase;
//
//}
//
//
//void Pluck_next_ak_z(Pluck *unit, int inNumSamples)
//{
//	float *out = OUT(0);
//	float *in = IN(0);
//	float *trig = IN(1);
//	float delaytime = IN0(3);
//	float decaytime = IN0(4);
//	float coef = IN0(5);
//	float lastsamp = unit->m_lastsamp;
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	float feedbk = unit->m_feedbk;
//	long mask = unit->m_mask;
//	float d0, d1, d2, d3;
//	float thisin, curtrig;
//	unsigned long inputsamps = unit->m_inputsamps;
//	float prevtrig = unit->m_prevtrig;
//
//	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime && coef == unit->m_coef) {
//		long idsamp = (long)dsamp;
//		float frac = dsamp - idsamp;
//		for(int i = 0; i < inNumSamples; i++){
//			curtrig = trig[i];
//			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
//			    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//			    }
//			prevtrig = curtrig;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			if (irdphase0 < 0) {
//				dlybuf[iwrphase & mask] = thisin;
//				out[i] = 0.f;
//			} else {
//				if (irdphase1 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//				} else if (irdphase2 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//				} else if (irdphase3 < 0) {
//					d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//				} else {
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//					d3 = dlybuf[irdphase3 & mask];
//				}
//				float value = cubicinterp(frac, d0, d1, d2, d3);
//			float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			}
//			iwrphase++;
//		};
//	} else {
//
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//
//		float curcoef = unit->m_coef;
//		float coef_slope = CALCSLOPE(coef, curcoef);
//
//		for(int i = 0; i < inNumSamples; i++) {
//			curtrig = trig[i];
//			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
//			    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//			    }
//			prevtrig = curtrig;
//			dsamp += dsamp_slope;
//			long idsamp = (long)dsamp;
//			float frac = dsamp - idsamp;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			if (irdphase0 < 0) {
//				dlybuf[iwrphase & mask] = thisin;
//				out[i] = 0.f;
//			    } else {
//				if (irdphase1 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//				} else if (irdphase2 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//				} else if (irdphase3 < 0) {
//					d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//				} else {
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//					d3 = dlybuf[irdphase3 & mask];
//				}
//				float value = cubicinterp(frac, d0, d1, d2, d3);
//			float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			}
//			feedbk += feedbk_slope;
//			curcoef +=coef_slope;
//			iwrphase++;
//		};
//		unit->m_feedbk = feedbk;
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//		unit->m_decaytime = decaytime;
//		unit->m_coef = coef;
//	}
//
//	unit->m_inputsamps = inputsamps;
//	unit->m_prevtrig = prevtrig;
//	unit->m_lastsamp = zapgremlins(lastsamp);
//	unit->m_iwrphase = iwrphase;
//
//	unit->m_numoutput += inNumSamples;
//	if (unit->m_numoutput >= unit->m_idelaylen) {
//		SETCALC(Pluck_next_ak);
//	}
//}
//
//
//void Pluck_next_ka(Pluck *unit, int inNumSamples)
//{
//	float *out = OUT(0);
//	float *in = IN(0);
//	float trig = IN0(1);
//	float delaytime = IN0(3);
//	float decaytime = IN0(4);
//	float *coef = IN(5);
//	float lastsamp = unit->m_lastsamp;
//	unsigned long inputsamps = unit->m_inputsamps;
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	float feedbk = unit->m_feedbk;
//	long mask = unit->m_mask;
//	float thisin;
//
//	if ((unit->m_prevtrig <= 0.f) && (trig > 0.f)) {
//	    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//	    }
//	unit->m_prevtrig = trig;
//
//	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
//		long idsamp = (long)dsamp;
//		float frac = dsamp - idsamp;
//	for(int i = 0; i < inNumSamples; i++){
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			float d0 = dlybuf[irdphase0 & mask];
//			float d1 = dlybuf[irdphase1 & mask];
//			float d2 = dlybuf[irdphase2 & mask];
//			float d3 = dlybuf[irdphase3 & mask];
//			float value = cubicinterp(frac, d0, d1, d2, d3);
//			float thiscoef = coef[i];
//			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			iwrphase++;
//		};
//	} else {
//
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//
//	    for(int i = 0; i < inNumSamples; i++){
//			dsamp += dsamp_slope;
//			long idsamp = (long)dsamp;
//			float frac = dsamp - idsamp;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			float d0 = dlybuf[irdphase0 & mask];
//			float d1 = dlybuf[irdphase1 & mask];
//			float d2 = dlybuf[irdphase2 & mask];
//			float d3 = dlybuf[irdphase3 & mask];
//			float value = cubicinterp(frac, d0, d1, d2, d3);
//			float thiscoef = coef[i];
//			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			feedbk += feedbk_slope;
//			iwrphase++;
//		};
//		unit->m_feedbk = feedbk;
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//		unit->m_decaytime = decaytime;
//	}
//
//	unit->m_inputsamps = inputsamps;
//	unit->m_lastsamp = zapgremlins(lastsamp);
//	unit->m_iwrphase = iwrphase;
//
//}
//
//
//void Pluck_next_ka_z(Pluck *unit, int inNumSamples)
//{
//	float *out = OUT(0);
//	float *in = IN(0);
//	float trig = IN0(1);
//	float delaytime = IN0(3);
//	float decaytime = IN0(4);
//	float *coef = IN(5);
//	float lastsamp = unit->m_lastsamp;
//
//	float *dlybuf = unit->m_dlybuf;
//	long iwrphase = unit->m_iwrphase;
//	float dsamp = unit->m_dsamp;
//	float feedbk = unit->m_feedbk;
//	long mask = unit->m_mask;
//	float d0, d1, d2, d3;
//	float thisin;
//	unsigned long inputsamps = unit->m_inputsamps;
//
//	if ((unit->m_prevtrig <= 0.f) && (trig > 0.f)) {
//	    inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
//	    }
//
//	unit->m_prevtrig = trig;
//
//	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
//		long idsamp = (long)dsamp;
//		float frac = dsamp - idsamp;
//		for(int i = 0; i < inNumSamples; i++){
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			if (irdphase0 < 0) {
//				dlybuf[iwrphase & mask] = thisin;
//				out[i] = 0.f;
//			} else {
//				if (irdphase1 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//				} else if (irdphase2 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//				} else if (irdphase3 < 0) {
//					d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//				} else {
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//					d3 = dlybuf[irdphase3 & mask];
//				}
//				float value = cubicinterp(frac, d0, d1, d2, d3);
//			float thiscoef = coef[i];
//			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			}
//			iwrphase++;
//		};
//	} else {
//
//		float next_dsamp = CalcDelay(unit, delaytime);
//		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
//
//		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
//		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
//
//		for(int i = 0; i < inNumSamples; i++) {
//			dsamp += dsamp_slope;
//			long idsamp = (long)dsamp;
//			float frac = dsamp - idsamp;
//			long irdphase1 = iwrphase - idsamp;
//			long irdphase2 = irdphase1 - 1;
//			long irdphase3 = irdphase1 - 2;
//			long irdphase0 = irdphase1 + 1;
//			if (inputsamps > 0) {
//			    thisin = in[i];
//			    --inputsamps;
//			    } else {
//			    thisin = 0.f;
//			    }
//			if (irdphase0 < 0) {
//				dlybuf[iwrphase & mask] = thisin;
//				out[i] = 0.f;
//			    } else {
//				if (irdphase1 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//				} else if (irdphase2 < 0) {
//					d1 = d2 = d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//				} else if (irdphase3 < 0) {
//					d3 = 0.f;
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//				} else {
//					d0 = dlybuf[irdphase0 & mask];
//					d1 = dlybuf[irdphase1 & mask];
//					d2 = dlybuf[irdphase2 & mask];
//					d3 = dlybuf[irdphase3 & mask];
//				}
//				float value = cubicinterp(frac, d0, d1, d2, d3);
//			float thiscoef = coef[i];
//			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
//			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
//			out[i] = lastsamp = onepole;
//			}
//			feedbk += feedbk_slope;
//			iwrphase++;
//		};
//		unit->m_feedbk = feedbk;
//		unit->m_dsamp = dsamp;
//		unit->m_delaytime = delaytime;
//		unit->m_decaytime = decaytime;
//	}
//
//	unit->m_inputsamps = inputsamps;
//	unit->m_lastsamp = zapgremlins(lastsamp);
//	unit->m_iwrphase = iwrphase;
//
//	unit->m_numoutput += inNumSamples;
//	if (unit->m_numoutput >= unit->m_idelaylen) {
//		SETCALC(Pluck_next_ka);
//	}
//}
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//#define DELTAP_BUF \
//	World *world = unit->mWorld;\
//	if (bufnum >= world->mNumSndBufs) { \
//		int localBufNum = bufnum - world->mNumSndBufs; \
//		Graph *parent = unit->mParent; \
//		if(localBufNum <= parent->localBufNum) { \
//			unit->m_buf = parent->mLocalSndBufs + localBufNum; \
//		} else { \
//			bufnum = 0; \
//			unit->m_buf = world->mSndBufs + bufnum; \
//		} \
//	} else { \
//		unit->m_buf = world->mSndBufs + bufnum; \
//	} \
//	SndBuf *buf = unit->m_buf; \
//	float *bufData __attribute__((__unused__)) = buf->data; \
//	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
//	uint32 bufSamples = buf->samples; \
//	uint32 bufFrames = buf->frames; \
//	int guardFrame __attribute__((__unused__)) = bufFrames - 2; \
//	double loopMax = (double)bufSamples;
//
//#define CHECK_DELTAP_BUF \
//	if ((!bufData) || (bufChannels != 1)) { \
//				unit->mDone = true; \
//		ClearUnitOutputs(unit, inNumSamples); \
//		return; \
//	}
//
//
//static void DelTapWr_first(DelTapWr *unit, int inNumSamples)
//{
//	float fbufnum  = IN0(0);
//	uint32 bufnum = (uint32)fbufnum;
//	float* in = IN(1);
//	float* out = OUT(0);
//
//	uint32 phase = unit->m_phase;
//
//	DELTAP_BUF
//	CHECK_DELTAP_BUF
//
//	// zero out the buffer!
//#ifdef NOVA_SIMD
//	uint32 unroll = bufSamples & (~15);
//	nova::zerovec_simd(bufData, unroll);
//
//	uint32 remain = bufSamples - unroll;
//	Clear(remain, bufData + unroll);
//#else
//	Clear(bufSamples, bufData);
//#endif
//
//	out[0] = (float)phase;
//	bufData[phase] = in[0];
//	phase++;
//	if(phase == bufSamples)
//		phase -= bufSamples;
//
//	unit->m_phase = phase;
//}
//
//void DelTapWr_Ctor(DelTapWr *unit)
//{
//	if (BUFLENGTH & 15)
//		SETCALC(DelTapWr_next);
//	else
//		SETCALC(DelTapWr_next_simd);
//	unit->m_phase = 0;
//	unit->m_fbufnum = -1e9f;
//	DelTapWr_first(unit, 1);
//}
//
//template <bool simd>
//static inline void DelTapWr_perform(DelTapWr *unit, int inNumSamples)
//{
//	float fbufnum  = IN0(0);
//	uint32 bufnum = (uint32)fbufnum;
//	const float* in = ZIN(1);
//	float* out = ZOUT(0);
//	uint32 * phase_out = (uint32*)out;
//
//	uint32 phase = unit->m_phase;
//
//	DELTAP_BUF
//	CHECK_DELTAP_BUF
//
//	LOCK_SNDBUF(buf);
//	int buf_remain = (int)(bufSamples - phase);
//	if (inNumSamples < buf_remain)
//	{
//		/* fast-path */
//#ifdef NOVA_SIMD
//		if (simd)
//			nova::copyvec_an_simd(bufData+phase, IN(1), inNumSamples);
//		else
//#endif
//			Copy(inNumSamples, bufData + phase, IN(1));
//		LOOP1 (inNumSamples,
//			ZXP(phase_out) = phase++;
//		)
//	} else {
//		LOOP1 (inNumSamples,
//			bufData[phase] = ZXP(in);
//			ZXP(phase_out) = phase++;
//			if(phase == bufSamples)
//				phase -= bufSamples;
//		)
//	}
//
//	unit->m_phase = phase;
//}
//
//void DelTapWr_next(DelTapWr *unit, int inNumSamples)
//{
//	DelTapWr_perform<false>(unit, inNumSamples);
//}
//
//void DelTapWr_next_simd(DelTapWr *unit, int inNumSamples)
//{
//	DelTapWr_perform<true>(unit, inNumSamples);
//}
//
//
//#define SETUP_TAPDELK \
//	float delTime = unit->m_delTime; \
//	float newDelTime = IN0(2) * (float)SAMPLERATE; \
//	float delTimeInc = CALCSLOPE(newDelTime, delTime); \
//	float * fPhaseIn = IN(1); \
//	uint32 * iPhaseIn = (uint32*)fPhaseIn; \
//	uint32 phaseIn = *iPhaseIn; \
//	float fbufnum  = IN0(0); \
//	uint32 bufnum = (uint32)fbufnum; \
//	float* out = ZOUT(0); \
//
//#define SETUP_TAPDELA \
//	float* delTime = ZIN(2); \
//	float * fPhaseIn = IN(1); \
//	uint32 * iPhaseIn = (uint32*)fPhaseIn; \
//	uint32 phaseIn = *iPhaseIn; \
//	float fbufnum  = IN0(0); \
//	uint32 bufnum = (uint32)fbufnum; \
//	float* out = ZOUT(0); \
//
//void DelTapRd_Ctor(DelTapRd *unit)
//{
//	unit->m_fbufnum = -1e9f;
//	unit->m_delTime = IN0(2) * SAMPLERATE;
//	int interp = (int)IN0(3);
//	if (INRATE(2) == calc_FullRate) {
//		if (interp == 2)
//			SETCALC(DelTapRd_next2_a);
//		else if (interp == 4)
//			SETCALC(DelTapRd_next4_a);
//		else
//			SETCALC(DelTapRd_next1_a);
//	} else {
//		if (interp == 2)
//			SETCALC(DelTapRd_next2_k);
//		else if (interp == 4)
//			SETCALC(DelTapRd_next4_k);
//		else
//			if (BUFLENGTH & 15)
//				SETCALC(DelTapRd_next1_k);
//			else {
//				SETCALC(DelTapRd_next1_k_simd);
//				DelTapRd_next1_k(unit, 1);
//				return;
//			}
//	}
//	(unit->mCalcFunc)(unit, 1);
//}
//
//
//void DelTapRd_next1_a(DelTapRd *unit, int inNumSamples)
//{
//	SETUP_TAPDELA
//	DELTAP_BUF
//	CHECK_DELTAP_BUF
//
//	LOCK_SNDBUF_SHARED(buf);
//	LOOP1(inNumSamples,
//		double curDelTimeSamps = ZXP(delTime) * SAMPLERATE;
//		double phase = phaseIn - curDelTimeSamps;
//		if(phase < 0.) phase += loopMax;
//		if(phase >=loopMax) phase -= loopMax;
//		int32 iphase = (int32)phase;
//		ZXP(out) = bufData[iphase];
//		phaseIn += 1.;
//	)
//}
//
//template <bool simd>
//inline void DelTapRd_perform1_k(DelTapRd *unit, int inNumSamples)
//{
//	SETUP_TAPDELK
//	DELTAP_BUF
//	CHECK_DELTAP_BUF
//	float * zout = ZOUT(0);
//
//	LOCK_SNDBUF_SHARED(buf);
//	if (delTime == newDelTime)
//	{
//		double phase = (double)phaseIn - delTime;
//		int32 iphase = (int32)phase;
//		if ( (iphase >= 0) // lower bound
//			&& iphase + inNumSamples < (bufSamples - 1)) //upper bound
//			{
//#ifdef NOVA_SIMD
//				if (simd)
//					nova::copyvec_na_simd(OUT(0), bufData + iphase, inNumSamples);
//				else
//#endif
//					Copy(inNumSamples, OUT(0), bufData + iphase);
//			}
//		else
//			LOOP1(inNumSamples,
//				if(iphase < 0) iphase += bufSamples;
//				if(iphase >= bufSamples) iphase -= bufSamples;
//				ZXP(zout) = bufData[iphase];
//				++iphase;
//			)
//	} else {
//		LOOP1(inNumSamples,
//			double phase = (double)phaseIn - delTime;
//			if(phase < 0.) phase += loopMax;
//			if(phase >=loopMax) phase -= loopMax;
//			int32 iphase = (int32)phase;
//			ZXP(zout) = bufData[iphase];
//			delTime += delTimeInc;
//			++phaseIn;
//		)
//		unit->m_delTime = delTime;
//	}
//}
//
//void DelTapRd_next1_k(DelTapRd *unit, int inNumSamples)
//{
//	DelTapRd_perform1_k<false>(unit, inNumSamples);
//}
//
//void DelTapRd_next1_k_simd(DelTapRd *unit, int inNumSamples)
//{
//	DelTapRd_perform1_k<true>(unit, inNumSamples);
//}
//
//void DelTapRd_next2_k(DelTapRd *unit, int inNumSamples)
//{
//	SETUP_TAPDELK
//	DELTAP_BUF
//	CHECK_DELTAP_BUF
//
//	int32 iloopMax = (int32)bufSamples;
//
//	LOCK_SNDBUF_SHARED(buf);
//
//	if (delTime == newDelTime)
//	{
//		double phase = (double)phaseIn - delTime;
//		double dphase;
//		float fracphase = std::modf(phase, &dphase);
//		int32 iphase = (int32)dphase;
//
//		if ( (phase >= 0) // lower bound
//			&& phase + inNumSamples < (loopMax - 2)) //upper bound
//		{
//			LOOP1(inNumSamples,
//				int32 iphase1 = iphase + 1;
//				float b = bufData[iphase];
//				float c = bufData[iphase1];
//				ZXP(out) = (b + fracphase * (c - b));
//				iphase += 1;
//			);
//		} else {
//			LOOP1(inNumSamples,
//				if(iphase < 0) iphase += iloopMax;
//				else if(iphase >= bufSamples) phase -= iloopMax;
//				int32 iphase1 = iphase + 1;
//				if(iphase1 >= iloopMax) iphase1 -= iloopMax;
//				float b = bufData[iphase];
//				float c = bufData[iphase1];
//				ZXP(out) = (b + fracphase * (c - b));
//				++iphase;
//			);
//		}
//	} else {
//		LOOP1(inNumSamples,
//			double phase = (double)phaseIn - delTime;
//			if(phase < 0.) phase += loopMax;
//			if(phase >= loopMax) phase -= loopMax;
//			int32 iphase = (int32)phase;
//			int32 iphase1 = iphase + 1;
//			if(iphase1 >= iloopMax) iphase1 -= iloopMax;
//			float fracphase = phase - (double)iphase;
//			float b = bufData[iphase];
//			float c = bufData[iphase1];
//			ZXP(out) = (b + fracphase * (c - b));
//			delTime += delTimeInc;
//			++phaseIn;
//		);
//		unit->m_delTime = delTime;
//	}
//}
//
//void DelTapRd_next2_a(DelTapRd *unit, int inNumSamples)
//{
//	SETUP_TAPDELA
//	DELTAP_BUF
//	CHECK_DELTAP_BUF
//
//	int32 iloopMax = (int32)bufSamples;
//
//	LOCK_SNDBUF_SHARED(buf);
//	LOOP1(inNumSamples,
//		double curDelTimeSamps = ZXP(delTime) * SAMPLERATE;
//		double phase = (double)phaseIn - curDelTimeSamps;
//		if(phase < 0.) phase += loopMax;
//		if(phase >= loopMax) phase -= loopMax;
//		int32 iphase = (int32)phase;
//		int32 iphase1 = iphase + 1;
//		if(iphase1 >= iloopMax) iphase1 -= iloopMax;
//		float fracphase = phase - (double)iphase;
//		float b = bufData[iphase];
//		float c = bufData[iphase1];
//		ZXP(out) = (b + fracphase * (c - b));
//		++phaseIn;
//	);
//}
//
//void DelTapRd_next4_k(DelTapRd *unit, int inNumSamples)
//{
//	SETUP_TAPDELK
//	DELTAP_BUF
//	CHECK_DELTAP_BUF
//
//	int32 iloopMax = (int32)loopMax;
//
//	LOCK_SNDBUF_SHARED(buf);
//
//
//	if (delTime == newDelTime)
//	{
//		double phase = (double)phaseIn - delTime;
//		double dphase;
//		float fracphase = std::modf(phase, &dphase);
//		int32 iphase = (int32)dphase;
//
//		if ( (iphase >= 1) // lower bound
//			&& iphase + inNumSamples < (iloopMax - 4)) //upper bound
//		{
//			LOOP1(inNumSamples,
//				int32 iphase0 = iphase - 1;
//				int32 iphase1 = iphase + 1;
//				int32 iphase2 = iphase + 2;
//
//				float a = bufData[iphase0];
//				float b = bufData[iphase];
//				float c = bufData[iphase1];
//				float d = bufData[iphase2];
//				ZXP(out) = cubicinterp(fracphase, a, b, c, d);
//				++iphase;
//			);
//		} else {
//			LOOP1(inNumSamples,
//				if(iphase < 0) iphase += iloopMax;
//				else if(iphase >= iloopMax) iphase -= iloopMax;
//				int32 iphase0 = iphase - 1;
//				int32 iphase1 = iphase + 1;
//				int32 iphase2 = iphase + 2;
//
//				if(iphase0 < 0) iphase0 += iloopMax;
//				if(iphase1 > iloopMax) iphase1 -=iloopMax;
//				if(iphase2 > iloopMax) iphase2 -=iloopMax;
//
//				float a = bufData[iphase0];
//				float b = bufData[iphase];
//				float c = bufData[iphase1];
//				float d = bufData[iphase2];
//				ZXP(out) = cubicinterp(fracphase, a, b, c, d);
//				++iphase;
//			);
//		}
//	} else {
//		LOOP1(inNumSamples,
//			double phase = (double)phaseIn - delTime;
//			double dphase;
//			float fracphase = std::modf(phase, &dphase);
//			int32 iphase = (int32)dphase;
//
//			if(iphase < 0.) iphase += iloopMax;
//			if(iphase >= iloopMax) iphase -= iloopMax;
//			int32 iphase0 = iphase - 1;
//			int32 iphase1 = iphase + 1;
//			int32 iphase2 = iphase + 2;
//
//			if(iphase0 < 0) iphase0 += iloopMax;
//			if(iphase1 > iloopMax) iphase1 -=iloopMax;
//			if(iphase2 > iloopMax) iphase2 -=iloopMax;
//
//			float a = bufData[iphase0];
//			float b = bufData[iphase];
//			float c = bufData[iphase1];
//			float d = bufData[iphase2];
//			ZXP(out) = cubicinterp(fracphase, a, b, c, d);
//			delTime += delTimeInc;
//			++phaseIn;
//		);
//		unit->m_delTime = delTime;
//	}
//}
//
//void DelTapRd_next4_a(DelTapRd *unit, int inNumSamples)
//{
//	SETUP_TAPDELA
//	DELTAP_BUF
//	CHECK_DELTAP_BUF
//
//	int32 iloopMax = (int32)loopMax;
//
//	LOCK_SNDBUF_SHARED(buf);
//	LOOP1(inNumSamples,
//		double curDelTimeSamps = ZXP(delTime) * SAMPLERATE;
//		double phase = (double)phaseIn - curDelTimeSamps;
//		if(phase < 0.) phase += loopMax;
//		if(phase >= loopMax) phase -= loopMax;
//		int32 iphase = (int32)phase;
//		int32 iphase0 = iphase - 1;
//		int32 iphase1 = iphase + 1;
//		int32 iphase2 = iphase + 2;
//
//		if(iphase0 < 0) iphase0 += iloopMax;
//		if(iphase1 > iloopMax) iphase1 -=iloopMax;
//		if(iphase2 > iloopMax) iphase2 -=iloopMax;
//
//		float fracphase = phase - (double)iphase;
//		float a = bufData[iphase0];
//		float b = bufData[iphase];
//		float c = bufData[iphase1];
//		float d = bufData[iphase2];
//		ZXP(out) = cubicinterp(fracphase, a, b, c, d);
//		++phaseIn;
//	);
//}


////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////

//PluginLoad(Delay)
//void load(InterfaceTable *inTable)
PluginLoad(BEASTmulchDelay)
{
	ft = inTable;

#define DefineInfoUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(Unit), (UnitCtorFunc)&name##_Ctor, 0, 0);

//	DefineInfoUnit(ControlRate);
//	DefineInfoUnit(SampleRate);
//	DefineInfoUnit(SampleDur);
//	DefineInfoUnit(ControlDur);
//	DefineInfoUnit(SubsampleOffset);
//	DefineInfoUnit(RadiansPerSample);
//	DefineInfoUnit(NumInputBuses);
//	DefineInfoUnit(NumOutputBuses);
//	DefineInfoUnit(NumAudioBuses);
//	DefineInfoUnit(NumControlBuses);
//	DefineInfoUnit(NumBuffers);
//	DefineInfoUnit(NumRunningSynths);
//
//#define DefineBufInfoUnit(name) \
//	(*ft->fDefineUnit)(#name, sizeof(BufInfoUnit), (UnitCtorFunc)&name##_Ctor, 0, 0);
//
//	DefineBufInfoUnit(BufSampleRate);
//	DefineBufInfoUnit(BufRateScale);
//	DefineBufInfoUnit(BufSamples);
//	DefineBufInfoUnit(BufFrames);
//	DefineBufInfoUnit(BufChannels);
//	DefineBufInfoUnit(BufDur);
//
//	DefineDtorCantAliasUnit(PlayBuf);
//#if NOTYET
//	DefineDtorUnit(SimpleLoopBuf);
//#endif
//	DefineDtorUnit(RecordBuf);
//	DefineDtorUnit(BufRd);
//	DefineDtorUnit(BufWr);
//	DefineDtorUnit(Pitch);
//
//	DefineSimpleUnit(BufDelayN);
//	DefineSimpleUnit(BufDelayL);
//	DefineSimpleUnit(BufDelayC);
//	DefineSimpleUnit(BufCombN);
//	DefineSimpleUnit(BufCombL);
//	DefineSimpleUnit(BufR1C);
//	DefineSimpleUnit(BufAllpassN);
//	DefineSimpleUnit(BufAllpassL);
//	DefineSimpleUnit(BufAllpassC);

#define DefineDelayUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, \
	(UnitDtorFunc)&DelayUnit_Dtor, 0);

//	DefineDelayUnit(DelayN);
//	DefineDelayUnit(DelayL);
//	DefineDelayUnit(DelayC);
//	DefineDelayUnit(CombN);
//	DefineDelayUnit(CombL);
	DefineDelayUnit(R1C);
	DefineDtorUnit(R2C);
//	DefineDelayUnit(AllpassN);
//	DefineDelayUnit(AllpassL);
//	DefineDelayUnit(AllpassC);

//	DefineDtorUnit(PitchShift);
//	DefineSimpleUnit(GrainTap);
//	DefineSimpleCantAliasUnit(TGrains);
//	DefineDtorUnit(ScopeOut);
//	DefineDelayUnit(Pluck);
//
//	DefineSimpleUnit(DelTapWr);
//	DefineSimpleUnit(DelTapRd);
//
//	DefineDtorUnit(LocalBuf);
//	DefineSimpleUnit(MaxLocalBufs);
//	DefineSimpleUnit(SetBuf);
//	DefineSimpleUnit(ClearBuf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
