/*
 * File: open_loop_sin_deriv.h
 *
 * Code generated for Simulink model 'open_loop_sin_deriv'.
 *
 * Model version                  : 1.14
 * Simulink Coder version         : 8.2 (R2012a) 29-Dec-2011
 * TLC version                    : 8.2 (Jan 25 2012)
 * C/C++ source code generated on : Thu Sep  6 12:03:13 2012
 *
 * Target selection: ert_shrlib.tlc
 * Embedded hardware selection: 32-bit Generic
 * Emulation hardware selection:
 *    Differs from embedded hardware (MATLAB Host)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_open_loop_sin_deriv_h_
#define RTW_HEADER_open_loop_sin_deriv_h_
#ifndef open_loop_sin_deriv_COMMON_INCLUDES_
# define open_loop_sin_deriv_COMMON_INCLUDES_
#include <math.h>
#include <string.h>
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                                 /* open_loop_sin_deriv_COMMON_INCLUDES_ */

#include "open_loop_sin_deriv_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetT
# define rtmGetT(rtm)                  (rtmGetTPtr((rtm))[0])
#endif

/* Block signals (auto storage) */
typedef struct {
  real_T SineWave;                     /* '<Root>/Sine Wave' */
  real_T VectorConcatenate[2];         /* '<Root>/Vector Concatenate' */
} BlockIO_open_loop_sin_deriv;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  real_T DirtyDerivative_DSTATE[4];    /* '<Root>/DirtyDerivative' */
} D_Work_open_loop_sin_deriv;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T Out1[2];                      /* '<Root>/Out1' */
} ExternalOutputs_open_loop_sin_d;

/* Parameters (auto storage) */
struct Parameters_open_loop_sin_deriv_ {
  real_T SineWave_Amp;                 /* Expression: 6
                                        * Referenced by: '<Root>/Sine Wave'
                                        */
  real_T SineWave_Bias;                /* Expression: 0
                                        * Referenced by: '<Root>/Sine Wave'
                                        */
  real_T SineWave_Freq;                /* Expression: 2*pi
                                        * Referenced by: '<Root>/Sine Wave'
                                        */
  real_T SineWave_Phase;               /* Expression: 0
                                        * Referenced by: '<Root>/Sine Wave'
                                        */
  real_T DirtyDerivative_A[16];        /* Computed Parameter: DirtyDerivative_A
                                        * Referenced by: '<Root>/DirtyDerivative'
                                        */
  real_T DirtyDerivative_B[4];         /* Computed Parameter: DirtyDerivative_B
                                        * Referenced by: '<Root>/DirtyDerivative'
                                        */
  real_T DirtyDerivative_C[4];         /* Computed Parameter: DirtyDerivative_C
                                        * Referenced by: '<Root>/DirtyDerivative'
                                        */
  real_T DirtyDerivative_D;            /* Computed Parameter: DirtyDerivative_D
                                        * Referenced by: '<Root>/DirtyDerivative'
                                        */
  real_T DirtyDerivative_X0[4];        /* Expression: x0
                                        * Referenced by: '<Root>/DirtyDerivative'
                                        */
  real_T Gain_Gain;                    /* Expression: 1/(2*pi)
                                        * Referenced by: '<Root>/Gain'
                                        */
};

/* Real-time Model Data Structure */
struct RT_MODEL_open_loop_sin_deriv {
  const char_T * volatile errorStatus;
  RTWSolverInfo solverInfo;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
    time_T stepSize0;
    uint32_T clockTick1;
    SimTimeStep simTimeStep;
    time_T *t;
    time_T tArray[2];
  } Timing;
};

/* Block parameters (auto storage) */
extern Parameters_open_loop_sin_deriv open_loop_sin_deriv_P;

/* Block signals (auto storage) */
extern BlockIO_open_loop_sin_deriv open_loop_sin_deriv_B;

/* Block states (auto storage) */
extern D_Work_open_loop_sin_deriv open_loop_sin_deriv_DWork;

/* External outputs (root outports fed by signals with auto storage) */
extern ExternalOutputs_open_loop_sin_d open_loop_sin_deriv_Y;

/* Model entry point functions */
extern void open_loop_sin_deriv_initialize(void);
extern void open_loop_sin_deriv_step(void);
extern void open_loop_sin_deriv_terminate(void);

/* Real-time Model object */
extern struct RT_MODEL_open_loop_sin_deriv *const open_loop_sin_deriv_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'open_loop_sin_deriv'
 */
#endif                                 /* RTW_HEADER_open_loop_sin_deriv_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
