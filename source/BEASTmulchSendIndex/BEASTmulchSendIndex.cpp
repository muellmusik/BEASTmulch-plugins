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


//#include "MsgFifo.h"
//#include "SC_SyncCondition.h"
#include "SC_PlugIn.h"
#include <sndfile.h>
#include "SC_Types.h"
#include "SC_Unit.h"
//#include "SC_UnitDef.h"

static InterfaceTable *ft;

//const int kMAXDISKCHANNELS = 32;
//
//int16 gNumDISI = 0;
//
//enum {
//	diskinIdle,
//	diskinStartingEmpty,
//	diskinStartingFull,
//	diskinNormal,
//	diskinLastBuffer,
//	diskinEndSilence
//};
//
//struct DiskInSendIndex : public Unit
//{
//	int16 uniqueID;
//	float m_fbufnum;
//	SndBuf *m_buf;
//	uint32 m_framepos;
//	double mIndPhase;
//	int64 currSeek;
//	int16 waiting;
//};

struct PlayBufSendIndex : public Unit
{
	double m_phase;
	float m_prevtrig;
	float m_fbufnum;
	double mIndPhase;
	float mFreqMul;
	
	SndBuf *m_buf;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

//	void DiskInSendIndex_next(DiskInSendIndex *unit, int inNumSamples);
//	void DiskInSendIndex_Ctor(DiskInSendIndex* unit);
	
	void PlayBufSendIndex_next_aa(PlayBufSendIndex *unit, int inNumSamples);
	void PlayBufSendIndex_next_ak(PlayBufSendIndex *unit, int inNumSamples);
	void PlayBufSendIndex_next_ka(PlayBufSendIndex *unit, int inNumSamples);
	void PlayBufSendIndex_next_kk(PlayBufSendIndex *unit, int inNumSamples);
	void PlayBufSendIndex_Ctor(PlayBufSendIndex* unit);

}

//////////////////////////////////////////////////////////////////////////////////////////////////


//enum {
//	kDiskCmd_Read,
//	kDiskCmd_SendOffset,
//};
//
//struct DiskIOSendIndMsg
//{
//	World *mWorld;
//	int16 mCommand;
//	int16 mChannels;
//	int32 mBufNum;
//	int32 mPos;
//	int32 mFrames;
//	int32 mID;
//	int16 mParentIndex;
//	int16 uniqueID;
//	
//	void Perform();
//};
//
//
//struct FileOffsetMsg
//{
//	int16 mParentIndex;
//	int32 mID;
//	int16 uniqueID;
//	int64 currSeek;
//};
//
//#ifndef SC_WIN32
//MsgFifoNoFree<DiskIOSendIndMsg, 256> gDiskSendIndFifo;
//SC_SyncCondition gDiskSendIndFifoHasData;
//#else // #ifndef SC_WIN32
//MsgFifoNoFree<DiskIOSendIndMsg, 256>* pgDiskSendIndFifo;
//SC_SyncCondition* pgDiskSendIndFifoHasData;
//#endif // #ifndef SC_WIN32
//
//void* disk_io_thread_func(void* arg);
//void* disk_io_thread_func(void* arg)
//{
//	while (true) {
//#ifndef SC_WIN32
//    gDiskSendIndFifoHasData.WaitEach();
//		gDiskSendIndFifo.Perform();
//#else //#ifndef SC_WIN32
//    pgDiskSendIndFifoHasData->WaitEach();
//		pgDiskSendIndFifo->Perform();
//#endif //#ifndef SC_WIN32
//	}
//	return 0;
//}



//////////////////////////////////////////////////////////////////////////////////////////////////

//#define MAXCHANNELS 32
//
////#define GET_BUF \
////	float fbufnum  = ZIN0(0); \
////	if (fbufnum != unit->m_fbufnum) { \
////		uint32 bufnum = (int)fbufnum; \
////		World *world = unit->mWorld; \
////		if (bufnum >= world->mNumSndBufs) bufnum = 0; \
////		unit->m_fbufnum = fbufnum; \
////		unit->m_buf = world->mSndBufs + bufnum; \
////		unit->currSeek = -1; \
////		GET_CURRSEEK \
////	} \
////	SndBuf *buf = unit->m_buf; \
////	uint32 bufChannels = buf->channels; \
////	uint32 bufFrames = buf->frames; \
////	float *bufData = buf->data;
//
//#define SETUP_OUT(offset) \
//	if (unit->mNumOutputs != bufChannels) { \
//		ClearUnitOutputs(unit, inNumSamples); \
//		return; \
//	} \
//	float *out[MAXCHANNELS]; \
//	for (uint32 i=0; i<bufChannels; ++i) out[i] = OUT(i+offset);  
//
//#define GET_CURRSEEK \
//	unit->waiting = 1; \
//	DiskIOSendIndMsg msg; \
//	msg.uniqueID = unit->uniqueID; \
//	msg.mWorld = unit->mWorld; \
//	msg.mCommand = kDiskCmd_SendOffset; \
//	msg.mBufNum = (int)ZIN0(0); \
//	msg.mID = unit->mParent->mNode.mID; \
//	msg.mParentIndex = unit->mParentIndex; \
//	gDiskSendIndFifo.Write(msg); \
//	gDiskSendIndFifoHasData.Signal();
//
//#define SEND_IND \
//if (indPhase >= 1.f) { \
//	indPhase -= 1.f; \
//	if (unit->currSeek >= 0) { \
//		int64 offset; \
//		if (unit->m_framepos >= bufFrames2) { \
//			if(waiting) offset = unit->currSeek - bufFrames; \
//			else offset = unit->currSeek - bufFrames - bufFrames2; \
//		} else { \
//			if(waiting) offset = unit->currSeek - bufFrames2; \
//			else offset = unit->currSeek - bufFrames; \
//		} \
//		SendTrigger(&unit->mParent->mNode, (int)ZIN0(2), (float)(unit->m_framepos + j + offset)); \
//	} \
//} \
//indPhase += indfreq;
//
//
//void DiskInSendIndex_Ctor(DiskInSendIndex* unit)
//{
//	unit->uniqueID = gNumDISI++;
//	unit->m_fbufnum = -1.f;
//	unit->m_buf = unit->mWorld->mSndBufs;
//	unit->m_framepos = 0;
//	unit->mIndPhase = 1.f;
//	unit->currSeek = -1;
//	
//	SETCALC(DiskInSendIndex_next);
//}
//
//// should possibly have wait count rather than waiting flag
//void DiskInSendIndex_next(DiskInSendIndex *unit, int inNumSamples)
//{
//	float indfreq	= ZIN0(1) * unit->mRate->mSampleDur; 
//	double indPhase = unit->mIndPhase;
//	
//	GET_BUF
//	
//	int16 waiting = unit->waiting;
//	
//	if (!bufData || ((bufFrames & ((unit->mWorld->mBufLength<<1) - 1)) != 0)) {
//		printf("foo\n");
//		unit->m_framepos = 0;
//		ClearUnitOutputs(unit, inNumSamples);
//		return;
//	}
//	SETUP_OUT(0)
//	
//	uint32 bufFrames2 = bufFrames >> 1;
//	
//	if (unit->m_framepos >= bufFrames) {
//		unit->m_framepos = 0;
//	}
//
//	bufData += unit->m_framepos * bufChannels;
//	
//	// buffer must be allocated as a multiple of 2*blocksize.
//	if (bufChannels > 2) {
//		for (int j=0; j<inNumSamples; ++j) {
//			for (uint32 i=0; i<bufChannels; ++i) {
//				*out[i]++ = *bufData++;
//			}
//			// Index send
//			SEND_IND
//		}
//	} else if (bufChannels == 2) {
//		float *out0 = out[0];
//		float *out1 = out[1];
//		for (int j=0; j<inNumSamples; ++j) {
//			*out0++ = *bufData++;
//			*out1++ = *bufData++;
//			// Index send
//			SEND_IND
//		}
//	} else {
//		// single channel
//		float *out0 = out[0];
//		for (int j=0; j<inNumSamples; ++j) {
//			*out0++ = *bufData++;
//			// Index send
//			SEND_IND
//		}
//	}
//	
//	unit->m_framepos += inNumSamples;
//	unit->mIndPhase = indPhase;
//	
//	if (unit->m_framepos == bufFrames) {
//		unit->m_framepos = 0;
//		goto sendMessage;
//	} else if (unit->m_framepos == bufFrames2) {
//sendMessage:
//	if(unit->mWorld->mRealTime){
//		// send a message to read
//		unit->waiting = 1;
//		DiskIOSendIndMsg msg;
//		msg.uniqueID = unit->uniqueID;
//		msg.mWorld = unit->mWorld;
//		msg.mCommand = kDiskCmd_Read;
//		msg.mBufNum = (int)fbufnum;
//		msg.mPos = bufFrames2 - unit->m_framepos;
//		msg.mFrames = bufFrames2;
//		msg.mChannels = bufChannels;
//		msg.mID = unit->mParent->mNode.mID;
//		msg.mParentIndex = unit->mParentIndex;
//#ifndef SC_WIN32
//		gDiskSendIndFifo.Write(msg);
//		gDiskSendIndFifoHasData.Signal();
//#else //#ifndef SC_WIN32
//		pgDiskSendIndFifo->Write(msg);
//		pgDiskSendIndFifoHasData->Signal();
//#endif //#ifndef SC_WIN32
//	} else {
//		SndBuf *bufr = World_GetNRTBuf(unit->mWorld, (int) fbufnum);
//		uint32 mPos = bufFrames2 - unit->m_framepos;
//		if (mPos > (uint32)bufr->frames || mPos + bufFrames2 > (uint32)bufr->frames || (uint32) bufr->channels != bufChannels) return;
//		sf_count_t count;
//		count = bufr->sndfile ? sf_readf_float(bufr->sndfile, bufr->data + mPos * bufr->channels, bufFrames2) : 0;
//		if (count < bufFrames2) {
//			memset(bufr->data + (mPos + count) * bufr->channels, 0, (bufFrames2 - count) * bufr->channels);
//		}	
//	}	
//	}
//}
//
//void setCurrOffset(FifoMsg *inMsg);
//void freeFileOffsetMsg(FifoMsg *inMsg);
//	
//void DiskIOSendIndMsg::Perform()
//{
//	NRTLock(mWorld);
//	
//	int64 currSeek = 0;
//	SndBuf *buf = World_GetNRTBuf(mWorld, mBufNum);
//
//	if (mCommand == kDiskCmd_SendOffset) goto sendMessage;
//	if (!buf->sndfile) {
//		currSeek = -1; // buffer was closed
//	}
//	if (mPos > buf->frames || mPos + mFrames > buf->frames || buf->channels != mChannels) {
//		currSeek = -1;
//		goto sendMessage;
//	}
//
//	sf_count_t count;
//	if (mCommand == kDiskCmd_Read) { // if not then kDiskCmd_SendOffset, so don't read
//		count = buf->sndfile ? sf_readf_float(buf->sndfile, buf->data + mPos * buf->channels, mFrames) : 0;
//		if (count < mFrames) {
//			currSeek = -1;
//			memset(buf->data + (mPos + count) * buf->channels, 0, (mFrames - count) * buf->channels);
//		}
//	}
//	
//sendMessage:		
//	FileOffsetMsg* reply = (FileOffsetMsg*)malloc(sizeof(FileOffsetMsg));
//	reply->uniqueID = uniqueID;
//	reply->mParentIndex = mParentIndex;
//	reply->mID = mID;
//	reply->currSeek = currSeek ? currSeek : (int64)sf_seek(buf->sndfile, (sf_count_t)0, SEEK_CUR);
//	FifoMsg fifoMsg;
//	fifoMsg.Set(mWorld, setCurrOffset, freeFileOffsetMsg, (void*)reply);
//	SendMsgToRT(mWorld, fifoMsg);
//
//	NRTUnlock(mWorld);
//}
//
//void setCurrOffset(FifoMsg *inMsg)
//{
//	FileOffsetMsg *msg = (FileOffsetMsg*)inMsg->mData;
//	Graph* mParent = SC_GetGraph(inMsg->mWorld, msg->mID);
//	// check that we found a node with the same ID and enough units
//	if(mParent && mParent->mNumUnits > (uint32)msg->mParentIndex) {
//		DiskInSendIndex *myDI = (DiskInSendIndex*)(mParent->mUnits[msg->mParentIndex]);
//		// is this unit a DiskInSendIndex, and the same one that sent the message?
//		if (strcmp((char*)myDI->mUnitDef->mUnitDefName, "DiskInSendIndex") == 0 && myDI->uniqueID == msg->uniqueID) {
//			myDI->currSeek = msg->currSeek;
//			myDI->waiting = 0;
//		}
//	}
//}
//
//void freeFileOffsetMsg(FifoMsg *inMsg)
//{
//	FileOffsetMsg *msg = (FileOffsetMsg*)inMsg->mData;
//	if (msg) {
//		inMsg->mData = 0;
//		free(msg);
//	}
//}



//////////////////////////////////////////////////////////////////////////////////////////////

//static float cubicinterp(float x, float y0, float y1, float y2, float y3)
//{
//	// 4-point, 3rd-order Hermite (x-form)
//	float c0 = y1;
//	float c1 = 0.5f * (y2 - y0);
//	float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
//	float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
//	
//	return ((c3 * x + c2) * x + c1) * x + c0;
//}

/////////////////////////////////

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

#define GET_BUF2 \
float fbufnum  = ZIN0(0); \
if (fbufnum != unit->m_fbufnum) { \
	uint32 bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
			if (bufnum >= world->mNumSndBufs) bufnum = 0; \
				unit->m_fbufnum = fbufnum; \
					unit->m_buf = world->mSndBufs + bufnum; \
} \
SndBuf *buf = unit->m_buf; \
float *bufData __attribute__((__unused__)) = buf->data; \
uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
uint32 bufFrames = buf->frames; \
int mask __attribute__((__unused__)) = buf->mask; \
int guardFrame __attribute__((__unused__)) = bufFrames - 2; 

#define CHECK_BUF \
if (!bufData) { \
	unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
			return; \
}

#define SETUP_OUT2 \
uint32 numOutputs = unit->mNumOutputs; \
if (numOutputs > bufChannels) { \
	unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
			return; \
} \
float *out[16]; \
for (uint32 i=0; i<numOutputs; ++i) out[i] = ZOUT(i);


#define LOOP_BODY_4 \
phase = sc_loop((Unit*)unit, phase, loopMax, loop); \
int32 iphase = (int32)phase; \
float* table1 = bufData + iphase * bufChannels; \
float* table0 = table1 - bufChannels; \
float* table2 = table1 + bufChannels; \
float* table3 = table2 + bufChannels; \
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


void PlayBufSendIndex_Ctor(PlayBufSendIndex *unit)
{	
	if (INRATE(1) == calc_FullRate) {
		if (INRATE(2) == calc_FullRate) {
			//printf("aa\n");
			SETCALC(PlayBufSendIndex_next_aa);
		} else {
			//printf("ak\n");
			SETCALC(PlayBufSendIndex_next_ak);
		}
	} else {
		if (INRATE(2) == calc_FullRate) {
			//printf("ka\n");
			SETCALC(PlayBufSendIndex_next_ka);
		} else {
			//printf("kk\n");
			SETCALC(PlayBufSendIndex_next_kk);
		}
	}
	
	unit->m_fbufnum = -1e9f;
	unit->m_prevtrig = 0.;
	unit->m_phase = ZIN0(3);
	unit->mFreqMul = unit->mRate->mSampleDur;
	unit->mIndPhase = 1.f;
	
	ClearUnitOutputs(unit, 1);
}

void PlayBufSendIndex_next_aa(PlayBufSendIndex *unit, int inNumSamples)
{
	float *ratein  = ZIN(1);
	float *trigin  = ZIN(2);
	int32 loop     = (int32)ZIN0(4);
	float indfreq	= ZIN0(5) * unit->mFreqMul; 
	double indPhase = unit->mIndPhase;
	
	float fbufnum  = ZIN0(0);
	if (fbufnum != unit->m_fbufnum) {
		uint32 bufnum = (int)fbufnum;
		World *world = unit->mWorld;
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->m_fbufnum = fbufnum;
		unit->m_buf = world->mSndBufs + bufnum;
	}
	SndBuf *buf = unit->m_buf;
	float *bufData __attribute__((__unused__)) = buf->data;
	uint32 bufChannels __attribute__((__unused__)) = buf->channels;
	uint32 bufSamples __attribute__((__unused__)) = buf->samples;
	uint32 bufFrames = buf->frames;
	int mask __attribute__((__unused__)) = buf->mask;
	int guardFrame __attribute__((__unused__)) = bufFrames - 2; 
	
	CHECK_BUF
	SETUP_OUT2	
	
	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);
	double phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	
	for (int i=0; i<inNumSamples; ++i) {
		float trig = ZXP(trigin);
		if (trig > 0.f && prevtrig <= 0.f) {
			unit->mDone = false;
			phase = ZIN0(3);
		}
		prevtrig = trig;
		
		// Index send
		if (indPhase >= 1.f) {
			indPhase -= 1.f;
			SendTrigger(&unit->mParent->mNode, (int)ZIN0(6), (float)phase);
		}
		indPhase += indfreq;
		
		LOOP_BODY_4
			
		phase += ZXP(ratein);
	}
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
	unit->mIndPhase = indPhase;
}

void PlayBufSendIndex_next_ak(PlayBufSendIndex *unit, int inNumSamples)
{
	float *ratein  = ZIN(1);
	float trig     = ZIN0(2);
	int32 loop     = (int32)ZIN0(4);
	float indfreq	= ZIN0(5) * unit->mFreqMul; 
	double indPhase = unit->mIndPhase;
	
	float fbufnum  = ZIN0(0);
	if (fbufnum != unit->m_fbufnum) {
		uint32 bufnum = (int)fbufnum;
		World *world = unit->mWorld;
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->m_fbufnum = fbufnum;
		unit->m_buf = world->mSndBufs + bufnum;
	}
	SndBuf *buf = unit->m_buf;
	float *bufData __attribute__((__unused__)) = buf->data;
	uint32 bufChannels __attribute__((__unused__)) = buf->channels;
	uint32 bufSamples __attribute__((__unused__)) = buf->samples;
	uint32 bufFrames = buf->frames;
	int mask __attribute__((__unused__)) = buf->mask;
	int guardFrame __attribute__((__unused__)) = bufFrames - 2; 
	
	CHECK_BUF
	SETUP_OUT2	
	
	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);
	double phase = unit->m_phase;
    if(phase == -1.) phase = bufFrames;
	if (trig > 0.f && unit->m_prevtrig <= 0.f) {
		unit->mDone = false;
		phase = ZIN0(3);
	}
	unit->m_prevtrig = trig;
	for (int i=0; i<inNumSamples; ++i) {
		
		// Index send
		if (indPhase >= 1.f) {
			indPhase -= 1.f;
			SendTrigger(&unit->mParent->mNode, (int)ZIN0(6), (float)phase);
		}
		indPhase += indfreq;
		
		LOOP_BODY_4
		
		phase += ZXP(ratein);
	}
	unit->m_phase = phase;
	unit->mIndPhase = indPhase;
}

void PlayBufSendIndex_next_kk(PlayBufSendIndex *unit, int inNumSamples)
{
	float rate     = ZIN0(1);
	float trig     = ZIN0(2);
	int32 loop     = (int32)ZIN0(4);
	float indfreq	= ZIN0(5) * unit->mFreqMul; 
	double indPhase = unit->mIndPhase;
	
	GET_BUF2
	CHECK_BUF
	SETUP_OUT2
		
	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);
	double phase = unit->m_phase;
	if (trig > 0.f && unit->m_prevtrig <= 0.f) {
		unit->mDone = false;
		phase = ZIN0(3);
	}
	unit->m_prevtrig = trig;
	for (int i=0; i<inNumSamples; ++i) {
		
		// Index send
		if (indPhase >= 1.f) {
			indPhase -= 1.f;
			SendTrigger(&unit->mParent->mNode, (int)ZIN0(6), (float)phase);
		}
		indPhase += indfreq;
		
		LOOP_BODY_4
		
		phase += rate;
	}
	unit->m_phase = phase;
	unit->mIndPhase = indPhase;
}

void PlayBufSendIndex_next_ka(PlayBufSendIndex *unit, int inNumSamples)
{
	float rate     = ZIN0(1);
	float *trigin  = ZIN(2);
	int32 loop     = (int32)ZIN0(4);
	float indfreq	= ZIN0(5) * unit->mFreqMul; 
	double indPhase = unit->mIndPhase;
	
	GET_BUF2
	CHECK_BUF
	SETUP_OUT2	
	
	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);
	double phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	for (int i=0; i<inNumSamples; ++i) {
		float trig = ZXP(trigin);
		if (trig > 0.f && prevtrig <= 0.f) {
			unit->mDone = false;
			if (INRATE(3) == calc_FullRate) phase = IN(3)[i];
			else phase = ZIN0(3);
		}
		prevtrig = trig;
		
		// Index send
		if (indPhase >= 1.f) {
			indPhase -= 1.f;
			SendTrigger(&unit->mParent->mNode, (int)ZIN0(6), (float)phase);
		}
		indPhase += indfreq;
		
		LOOP_BODY_4
			
		phase += rate;
	}
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
	unit->mIndPhase = indPhase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void load(InterfaceTable *inTable)
//{
//	ft = inTable;
//	
//#ifdef SC_WIN32
//  pgDiskSendIndFifo = new MsgFifoNoFree<DiskIOMsg, 256>;
//  pgDiskSendIndFifoHasData = new SC_SyncCondition;
//  //$$$todo FIXME free those objects . (use a global std::auto_ptr)
//#endif //SC_WIN32
//
//  pthread_t diskioThread;
//	pthread_create (&diskioThread, NULL, disk_io_thread_func, (void*)0);
//	
//	DefineSimpleUnit(DiskInSendIndex);
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//void load(InterfaceTable *inTable)
PluginLoad(BEASTmulchSendIndex)
{
	ft = inTable;	
	DefineSimpleUnit(PlayBufSendIndex);
//	
//#ifdef SC_WIN32
//	pgDiskSendIndFifo = new MsgFifoNoFree<DiskIOMsg, 256>;
//	pgDiskSendIndFifoHasData = new SC_SyncCondition;
//	//$$$todo FIXME free those objects . (use a global std::auto_ptr)
//#endif //SC_WIN32
//	
//	pthread_t diskioThread;
//	pthread_create (&diskioThread, NULL, disk_io_thread_func, (void*)0);
//	
//	DefineSimpleUnit(DiskInSendIndex);
	
}