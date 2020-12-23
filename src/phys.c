typedef unsigned char byte;

typedef void dxGeom_t;
struct dxJointNode_s;
struct dxJoint_s;
struct dxJoint_Vtable;
struct dxWorld_s;


typedef union
{
  //dBase_t baseclass_0;
  struct dxWorld_s *world;
}dxWorldBase_t;


typedef struct dObject_s
{
  dxWorldBase_t ___u0;
  struct dObject_s *next;
  struct dObject_s **tome;
  void *userdata;
  int tag;
}dObject_t;

typedef struct dMass_s
{
  float mass;
  float c[4];
  float I[12];
}dMass_t;

typedef struct dxAutoDisable_s
{
  float linear_threshold;
  float angular_threshold;
  float idle_time;
  int idle_steps;
}dxAutoDisable_t;


typedef struct dxBody_s
{
  struct dObject_s baseclass_0;
  struct dxJointNode_s *firstjoint;
  int flags;
  dxGeom_t *geom;
  dMass_t mass;
  float invI[12];
  float invMass;
  float pos[4];
  float q[4];
  float R[12];
  float lvel[4];
  float avel[4];
  float facc[4];
  float tacc[4];
  float finite_rot_axis[4];
  dxAutoDisable_t adis;
  float adis_timeleft;
  int adis_stepsleft;
}dxBody_t;


typedef struct
{
  char dummy[1];
  byte pad[3];
}dBase_t;

typedef union
{
  //dBase_t baseclass_0;
  dxBody_t *firstbody;
}dxBodyBase_t;


typedef struct dxQuickStepParameters_s
{
  int num_iterations;
  float w;
}dxQuickStepParameters_t;

typedef struct dxContactParameters_s
{
  float max_vel;
  float min_depth;
}dxContactParameters_t;

typedef struct SorLcpData_s
{
  int order[444];
}SorLcpData_t;

typedef struct ConstraintRowData_s
{
  float J_body1Linear[3];
  float lambda;
  float J_body1Angular[3];
  int body1;
  float J_body2Linear[3];
  int body2;
  float J_body2Angular[3];
  float padding1;
  float iMJ_body1Linear[3];
  float padding2;
  float iMJ_body1Angular[3];
  float padding3;
  float iMJ_body2Linear[3];
  float padding4;
  float iMJ_body2Angular[3];
  float padding5;
  float lo;
  float hi;
  float rhs;
  float Ad;
}ConstraintRowData_t;

typedef struct QuickstepData_s
{
  ConstraintRowData_t rowData[444];
}QuickstepData_t;




typedef struct dxWorld_s
{
  dxBodyBase_t ___u0;
  struct dxJoint_s *firstjoint;
  int nb;
  int nj;
  float gravity[4];
  float global_erp;
  float global_cfm;
  dxAutoDisable_t adis;
  int adis_flag;
  dxQuickStepParameters_t qs;
  dxContactParameters_t contactp;
  SorLcpData_t sd;
  __attribute__((aligned(8))) QuickstepData_t qd;
}dxWorld_t;




typedef struct dxJointNode_s
{
  struct dxJoint_s *joint;
  void * body; //struct dxBody_s *body;
  void * next; //struct dxJointNode_s *next;
}dxJointNode_t;



typedef struct dxJoint_s
{
  dObject_t baseclass_0;
  struct dxJoint_Vtable *vtable;
  int flags;
  dxJointNode_t node[2];
  void* feedback;//dJointFeedback *feedback;
  float lambda[6];
  byte debug;
  byte pad[3];
}dxJoint_t;

struct dxJoint_Info1
{
  int m;
  int nub;
};


struct dxJoint_Info2
{
  float fps;
  float erp;
  float *J1l;
  float *J1a;
  float *J2l;
  float *J2a;
  int rowskip;
  float *c;
  float *cfm;
  float *lo;
  float *hi;
  int *findex;
};

struct dxJoint_Vtable
{
  int size;
  void (__cdecl *init)(dxJoint_t *);
  void (__cdecl *getInfo1)(dxJoint_t *, struct dxJoint_Info1 *);
  void (__cdecl *getInfo2)(dxJoint_t *, struct dxJoint_Info2 *);
  int typenum;
};

void removeObjectFromList (dObject_t *obj)                                     
{                                                                                          
  if (obj->next) obj->next->tome = obj->tome;                                              
  if (obj->tome) *(obj->tome) = obj->next;                                                                
  // safeguard                                                                             
  obj->next = 0;                                                                           
  obj->tome = 0;
  //Not really part of this function
  --obj->___u0.world->nj;
}                                                                                          

