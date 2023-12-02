// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vneuro_cuts__pch.h"
#include "Vneuro_cuts.h"
#include "Vneuro_cuts___024root.h"

// FUNCTIONS
Vneuro_cuts__Syms::~Vneuro_cuts__Syms()
{
}

Vneuro_cuts__Syms::Vneuro_cuts__Syms(VerilatedContext* contextp, const char* namep, Vneuro_cuts* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
    // Setup scopes
    __Vscope_TOP.configure(this, name(), "TOP", "TOP", 0, VerilatedScope::SCOPE_OTHER);
    __Vscope_neuro_cuts.configure(this, name(), "neuro_cuts", "neuro_cuts", 0, VerilatedScope::SCOPE_OTHER);
    // Setup export functions
    for (int __Vfinal = 0; __Vfinal < 2; ++__Vfinal) {
        __Vscope_TOP.varInsert(__Vfinal,"a", &(TOP.a), false, VLVT_UINT8,VLVD_IN|VLVF_PUB_RW,1 ,1,0);
        __Vscope_TOP.varInsert(__Vfinal,"b", &(TOP.b), false, VLVT_UINT8,VLVD_OUT|VLVF_PUB_RW,0);
        __Vscope_neuro_cuts.varInsert(__Vfinal,"a", &(TOP.neuro_cuts__DOT__a), false, VLVT_UINT8,VLVD_NODIR|VLVF_PUB_RW,1 ,1,0);
        __Vscope_neuro_cuts.varInsert(__Vfinal,"b", &(TOP.neuro_cuts__DOT__b), false, VLVT_UINT8,VLVD_NODIR|VLVF_PUB_RW,0);
    }
}
