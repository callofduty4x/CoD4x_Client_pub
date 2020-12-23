#include "q_shared.h"
#include "server.h"
#include "game.h"
#include "win_sys.h"

qboolean __cdecl CM_TraceBox(const float* a, const float* b, const float* c, float d);
void __cdecl CM_TransformedBoxTrace(trace_t* trace, const float* start, const float* end, const float* mins, const float* maxs,
			clipHandle_t cliphandle, int contentmask, const float* origin, const float* angle);

#ifdef WIN32
void __cdecl SV_ClipMoveToEntity(trace_t *trace){

	svEntity_t *entity;
	moveclip_t *clip;
	__asm__ volatile("":"=a" (entity), "=D" (clip));

#else
void __cdecl SV_ClipMoveToEntity(moveclip_t *clip, svEntity_t *entity, trace_t *trace){
#endif

	sharedEntity_t *touch;
	int		touchNum;
	float		*origin, *angles;
	vec3_t		mins, maxs;
	float		oldfraction;
	clipHandle_t	clipHandle;

	touchNum = entity - sv.svEntities;
	touch = SV_GentityNum( touchNum );

	if( !(clip->contentmask & touch->r.contents))
		return;

	
	if ( clip->passEntityNum != ENTITYNUM_NONE ) {

		if( touchNum == clip->passEntityNum )
			return;
		
		if(touch->r.ownerNum){
		
			if( touch->r.ownerNum - 1 == clip->passEntityNum )
			    return;
			
			if( touch->r.ownerNum - 1 == clip->passOwnerNum )
			    return;
		}

		if( clip->contentmask & CONTENTS_PLAYERCLIP && touchNum < 64 && OnSameTeam( &g_entities[clip->passEntityNum], &g_entities[touchNum])){
//			Com_Printf("EntityNum player: %i Other EntityNum: %i Content: %s\n", clip->passEntityNum , touchNum, Q_BitConv(clip->contentmask));
			return;
		}
	}


	VectorAdd(touch->r.absmin, clip->mins, mins);
	VectorAdd(touch->r.absmax, clip->maxs, maxs);

	
	if(CM_TraceBox(clip->start, mins, maxs, trace->fraction))
		return;

#ifdef WIN32
	clipHandle = SV_ClipHandleForEntity(touch);
#else
	if(!touch->r.bmodel)
		clipHandle = CM_TempBoxModel(touch->r.mins, touch->r.maxs, touch->r.contents);
	else
		clipHandle = touch->s.modelindex;
#endif
	
	origin = touch->r.currentOrigin;
	angles = touch->r.currentAngles;

	if ( !touch->r.bmodel ) {
		angles = vec3_origin;   // boxes don't rotate
	}

	oldfraction = trace->fraction;

	CM_TransformedBoxTrace( trace, clip->start, clip->end,
				clip->mins, clip->maxs, clipHandle, clip->contentmask, origin, angles );

	if ( trace->fraction < oldfraction ) {
		trace->var_02 = qtrue;
		trace->entityNum = touch->s.number;
	}

}