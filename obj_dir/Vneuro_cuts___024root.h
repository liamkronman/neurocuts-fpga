// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vneuro_cuts.h for the primary calling header

#ifndef VERILATED_VNEURO_CUTS___024ROOT_H_
#define VERILATED_VNEURO_CUTS___024ROOT_H_  // guard

#include "verilated.h"


class Vneuro_cuts__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vneuro_cuts___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(a,1,0);
    VL_OUT8(b,0,0);
    CData/*1:0*/ neuro_cuts__DOT__a;
    CData/*0:0*/ neuro_cuts__DOT__b;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __VactContinue;
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<0> __VactTriggered;
    VlTriggerVec<0> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vneuro_cuts__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vneuro_cuts___024root(Vneuro_cuts__Syms* symsp, const char* v__name);
    ~Vneuro_cuts___024root();
    VL_UNCOPYABLE(Vneuro_cuts___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
