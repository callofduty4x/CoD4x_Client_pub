#include <cstdlib>
#include "qcommon.h"

void * dAlloc (size_t size)
{
    return malloc (size);
}


void dFree (void *ptr, size_t size)
{
    free (ptr);
}

struct dBase {
  void *operator new (size_t size) { return dAlloc (size); }
  inline void *operator new (size_t size, void* place) { return place; }
  void operator delete (void *ptr, size_t size) { dFree (ptr,size); }
  void *operator new[] (size_t size) { return dAlloc (size); }
  void operator delete[] (void *ptr, size_t size) { dFree (ptr,size); }
};

/* internal object types (all prefixed with `dx') */



typedef float dReal;

/* these types are mainly just used in headers */
typedef dReal dVector3[4];
typedef dReal dVector4[4];
typedef dReal dMatrix3[4*3];
typedef dReal dMatrix4[4*4];
typedef dReal dMatrix6[8*6];
typedef dReal dQuaternion[4];

struct dxWorld;		/* dynamics world */
struct dxSpace;		/* collision space */
struct dxBody;		/* rigid body (dynamics object) */
struct dxGeom;		/* geometry (collision object) */
struct dxJoint;
struct dxJointNode;
struct dxJointGroup;

typedef struct dxWorld *dWorldID;
typedef struct dxSpace *dSpaceID;
typedef struct dxBody *dBodyID;
typedef struct dxGeom *dGeomID;
typedef struct dxJoint *dJointID;
typedef struct dxJointGroup *dJointGroupID;


// geom flags.
//
// GEOM_DIRTY means that the space data structures for this geom are
// potentially not up to date. NOTE THAT all space parents of a dirty geom
// are themselves dirty. this is an invariant that must be enforced.
//
// GEOM_AABB_BAD means that the cached AABB for this geom is not up to date.
// note that GEOM_DIRTY does not imply GEOM_AABB_BAD, as the geom might
// recalculate its own AABB but does not know how to update the space data
// structures for the space it is in. but GEOM_AABB_BAD implies GEOM_DIRTY.
// the valid combinations are: 0, GEOM_DIRTY, GEOM_DIRTY|GEOM_AABB_BAD.

enum {
  GEOM_DIRTY	= 1,	// geom is 'dirty', i.e. position unknown
  GEOM_AABB_BAD	= 2,	// geom's AABB is not valid
  GEOM_PLACEABLE = 4,	// geom is placeable
  GEOM_ENABLED = 8,		// geom is enabled

  // Ray specific
  RAY_FIRSTCONTACT = 0x10000,
  RAY_BACKFACECULL = 0x20000,
  RAY_CLOSEST_HIT  = 0x40000
};



struct dxGeom : public dBase {
  int type;		// geom type number, set by subclass constructor
  int gflags;		// flags used by geom and space
  void *data;		// user-defined data pointer
  dBodyID body;		// dynamics body associated with this object (if any)
  dxGeom *body_next;	// next geom in body's linked list of associated geoms
  dReal *pos;		// pointer to object's position vector
  dReal *R;		// pointer to object's rotation matrix

  // information used by spaces
  dxGeom *next;		// next geom in linked list of geoms
  dxGeom **tome;	// linked list backpointer
  dxSpace *parent_space;// the space this geom is contained in, 0 if none
  dReal aabb[6];	// cached AABB for this space
  unsigned long category_bits,collide_bits;

  dxGeom (dSpaceID _space, int is_placeable, dxBody* newBody);
  virtual ~dxGeom();

  virtual void computeAABB()=0;
  // compute the AABB for this object and put it in aabb. this function
  // always performs a fresh computation, it does not inspect the
  // GEOM_AABB_BAD flag.

  virtual int AABBTest (dxGeom *o, dReal aabb[6]);
  // test whether the given AABB object intersects with this object, return
  // 1=yes, 0=no. this is used as an early-exit test in the space collision
  // functions. the default implementation returns 1, which is the correct
  // behavior if no more detailed implementation can be provided.

  // utility functions

  // compute the AABB only if it is not current. this function manipulates
  // the GEOM_AABB_BAD flag.

  void recomputeAABB() {
    if (gflags & GEOM_AABB_BAD) {
      computeAABB();
      gflags &= ~GEOM_AABB_BAD;
    }
  }

  // add and remove this geom from a linked list maintained by a space.

  void spaceAdd (dxGeom **first_ptr) {
    next = *first_ptr;
    tome = first_ptr;
    if (*first_ptr) (*first_ptr)->tome = &next;
    *first_ptr = this;
  }
  void spaceRemove() {
    if (next) next->tome = tome;
    *tome = next;
  }

  // add and remove this geom from a linked list maintained by a body.

  void bodyAdd (dxBody *b) {

  }
  void bodyRemove();
};



/* contact info set by collision functions */

typedef struct dContactGeom {
  dVector3 pos;
  dVector3 normal;
  dReal depth;
  dGeomID g1,g2;
} dContactGeom;

enum {
  dMaxUserClasses = 5
};

/* class numbers - each geometry object needs a unique number */
enum {
  dSphereClass = 0,
  dBoxClass,
  dCCylinderClass,
  dCylinderClass,
  dPlaneClass,
  dRayClass,
  dGeomTransformClass,
  dTriMeshClass,

  dFirstSpaceClass,
  dSimpleSpaceClass = dFirstSpaceClass,
  dHashSpaceClass,
  dQuadTreeSpaceClass,
  dLastSpaceClass = dQuadTreeSpaceClass,

  dFirstUserClass,
  dLastUserClass = dFirstUserClass + dMaxUserClasses - 1,
  dGeomNumClasses
};


typedef void dNearCallback (void *data, dGeomID o1, dGeomID o2);

//****************************************************************************
// dxGeomTransform class

struct dxGeomTransform : public dxGeom {
  dxGeom *obj;		// object that is being transformed
  int cleanup;		// 1 to destroy obj when destroyed
  int infomode;		// 1 to put Tx geom in dContactGeom g1

  // cached final object transform (body tx + relative tx). this is set by
  // computeAABB(), and it is valid while the AABB is valid.
  dMatrix3 localR;
  dReal    localPos[3];
  dMatrix3 finalR;
  dReal    finalPos[3];


  dxGeomTransform (dSpaceID space, dxBody* body);
  ~dxGeomTransform();
  void computeAABB();
  void computeFinalTx();
  void Destruct();
};

struct dxSpace : public dxGeom {
  int count;			// number of geoms in this space
  dxGeom *first;		// first geom in list
  int cleanup;			// cleanup mode, 1=destroy geoms on exit

  // cached state for getGeom()
  int current_index;		// only valid if current_geom != 0
  dxGeom *current_geom;		// if 0 then there is no information

  // locking stuff. the space is locked when it is currently traversing its
  // internal data structures, e.g. in collide() and collide2(). operations
  // that modify the contents of the space are not permitted when the space
  // is locked.
  int lock_count;

  dxSpace (dSpaceID _space);
  ~dxSpace();

  void computeAABB();

  void setCleanup (int mode);
  int getCleanup();
  int query (dxGeom *geom);
  int getNumGeoms();
  void clear( );
  virtual dxGeom *getGeom (int i);

  virtual void add (dxGeom *);
  virtual void remove (dxGeom *);
  virtual void dirty (dxGeom *);

  virtual void cleanGeoms()=0;
  // turn all dirty geoms into clean geoms by computing their AABBs and any
  // other space data structures that are required. this should clear the
  // GEOM_DIRTY and GEOM_AABB_BAD flags of all geoms.

  virtual void collide (void *data, dNearCallback *callback)=0;
  virtual void collide2 (void *data, dxGeom *geom, dNearCallback *callback)=0;
};

struct dObject : public dBase {
  dxWorld *world;		// world this object is in
  dObject *next;		// next object of this type in list
  dObject **tome;		// pointer to previous object's next ptr
  void *userdata;		// user settable data
  int tag;			// used by dynamics algorithms
};

struct dMass {
  dReal mass;
  dVector4 c;
  dMatrix3 I;


};

struct dxBodyInfo
{
  dVector3 pos;			// position of POR (point of reference)
  dQuaternion q;		// orientation quaternion
  dMatrix3 R;			// rotation matrix, always corresponds to q
  dVector3 lvel,avel;		// linear and angular velocity of POR
};


// auto disable parameters
struct dxAutoDisable {
  dReal linear_threshold;	// linear (squared) velocity treshold
  dReal angular_threshold;	// angular (squared) velocity treshold
  dReal idle_time;		// time the body needs to be idle to auto-disable it
  int idle_steps;		// steps the body needs to be idle to auto-disable it
};


struct dxBody : public dObject {
  dxJointNode *firstjoint;	// list of attached joints
  int flags;			// some dxBodyFlagXXX flags
  dGeomID geom;			// first collision geom associated with body
  dMass mass;			// mass parameters about POR
  dMatrix3 invI;		// inverse of mass.I
  dReal invMass;		// 1 / mass.mass
  dVector3 facc,tacc;		// force and torque accumulators
  dVector3 finite_rot_axis;	// finite rotation axis, unit length or 0=none

  // auto-disable information
  dxAutoDisable adis;		// auto-disable parameters
  dReal adis_timeleft;		// time left to be idle
  int adis_stepsleft;		// steps left to be idle
  dxBodyInfo info;

};







#include <windows.h>

extern "C" int dCollideTransform (dxGeom *o1, dxGeom *o2, int flags, dContactGeom *contact, int skip);

extern "C" int dCollideTransform_Hook (dxGeom *o1, dxGeom *o2, int flags, dContactGeom *contact, int skip)
{
    MessageBoxA(NULL, "Hello", "Hello", MB_OK);

	assert (skip >= (int)sizeof(dContactGeom));
	assert (o1->type == dGeomTransformClass);

	dxGeomTransform *tr = (dxGeomTransform*) o1;
	if (!tr->obj) return 0;
	if (tr->obj->parent_space!=0)
    {
    }
	return dCollideTransform (o1, o2, flags, contact, skip);
}

typedef int dColliderFn (dGeomID o1, dGeomID o2,
			 int flags, dContactGeom *contact, int skip);
struct dColliderEntry {
  dColliderFn *fn;	// collider function, 0 = no function available
  int reverse;		// 1 = reverse o1 and o2
};
// given a pointer `p' to a dContactGeom, return the dContactGeom at
// p + skip bytes.
#define CONTACT(p,skip) ((dContactGeom*) (((char*)p) + (skip)))
#define colliders (((dColliderEntry (*)[16])(0xCC781D0)))


extern "C" int dCollide (dxGeom *o1, dxGeom *o2, int flags, dContactGeom *contact,
	      int skip)
{
    assert(o1 && o2 && contact);
    //assert(colliders_initialized,"colliders array not initialized");
    assertx(o1->type >= 0 && o1->type < dGeomNumClasses,"bad o1 class number");
    assertx(o2->type >= 0 && o2->type < dGeomNumClasses,"bad o2 class number");
    // no contacts if both geoms are the same
    if (o1 == o2) return 0;

    // no contacts if both geoms on the same body, and the body is not 0
    if (o1->body == o2->body && o1->body) return 0;

    dColliderEntry *ce = &colliders[o1->type][o2->type];
    int count = 0;
    if (ce->fn) {
        if (ce->reverse) {
        count = (*ce->fn) (o2,o1,flags,contact,skip);
        for (int i=0; i<count; i++) {
            dContactGeom *c = CONTACT(contact,skip*i);
            c->normal[0] = -c->normal[0];
            c->normal[1] = -c->normal[1];
            c->normal[2] = -c->normal[2];
            dxGeom *tmp = c->g1;
            c->g1 = c->g2;
            c->g2 = tmp;
        }
        } else {
        count = (*ce->fn) (o1,o2,flags,contact,skip);
        }
    }
    return count;
}

extern "C" void Phys_NearCallback(void *userData, dxGeom *geom1, dxGeom *geom2);

extern "C" void ODE_CollideSimpleSpaceWithGeomNoAABBTest(dxSpace *space, dxGeom *geom, void *data)
{
  dxGeom *geom2;


  assert(geom);
  assert(space);

  ++space->lock_count;

  for (geom2 = space->first ; geom2; geom2 = geom2->next )
  {
    if ( (!geom2->body || (geom2->body->flags & 4) == 0) && (geom2->gflags & 8) != 0 )
    {
      assert(geom != geom2);
      assert(geom->body != geom2->body);
      if(geom->type == dGeomTransformClass || geom2->type == dGeomTransformClass){
        __asm__("int $3");
      }
      Phys_NearCallback(data, geom2, geom);
    }
  }
  --space->lock_count;
}

enum PhysWorld
{
  PHYS_WORLD_DYNENT,
  PHYS_WORLD_FX,
  PHYS_WORLD_RAGDOLL,
  PHYS_WORLD_COUNT
};


struct Jitter
{
  float origin[3];
  float innerRadiusSq;
  float outerRadiusSq;
  float innerRadius;
  float outerRadius;
  float minDisplacement;
  float maxDisplacement;
};

struct PhysWorldData
{
  int timeLastSnapshot;
  int timeLastUpdate;
  float timeNowLerpFrac;
  void (__cdecl *collisionCallback)();
  int numJitterRegions;
  bool useContactCentroids;
  Jitter jitterRegions[5];
};



struct PhysGlob
{
  dxWorld *world[3];
  PhysWorldData worldData[3];
  dxSpace *space[3];
  dxJointGroup *contactgroup[3];
  /*
  PhysObjUserData userData[512];
  pooldata_t userDataPool;
  PhysTriMeshInfo triMeshInfo;
  struct dxTriMeshData *triMeshDataID;
  bool dumpContacts;
  dxGeom *visTrisGeom;
  dxGeom *worldGeom;
  int debugActiveObjCount;
  PhysStaticArray<dxJointHinge,192> hingeArray;
  PhysStaticArray<dxJointBall,160> ballArray;
  PhysStaticArray<dxJointAMotor,160> aMotorArray;
  float gravityDirection[3];*/
};

#define physGlob (*((PhysGlob*)(0xcc54478)))

extern "C" void DynEntDebugSpaceBegin(){

  OutputDebugStringA("DynEntDebugSpaceBegin, check start\n");

  dxSpace *space = physGlob.space[PHYS_WORLD_DYNENT];
  ++space->lock_count;

  for (dxGeom* geom = space->first ; geom; geom = geom->next )
  {
      if ( (!geom->body || (geom->body->flags & 4) == 0) && (geom->gflags & 8) != 0 )
      {
          if(geom->type == dGeomTransformClass ){
              __asm__("int $3");
          }
      }
  }
  --space->lock_count;

  OutputDebugStringA("DynEntDebugSpaceBegin, check ended\n");

}

extern "C" void DynEntDebugSpaceEnd(){
    OutputDebugStringA("DynEntDebugSpaceEnd, check start\n");

    dxSpace *space = physGlob.space[PHYS_WORLD_DYNENT];
    ++space->lock_count;

    for (dxGeom* geom = space->first ; geom; geom = geom->next )
    {
        if ( (!geom->body || (geom->body->flags & 4) == 0) && (geom->gflags & 8) != 0 )
        {
            if(geom->type == dGeomTransformClass ){
                __asm__("int $3");
            }
        }
    }
    --space->lock_count;

    OutputDebugStringA("DynEntDebugSpaceEnd, check ended\n");

}