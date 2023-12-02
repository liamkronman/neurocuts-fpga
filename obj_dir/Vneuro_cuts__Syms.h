// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VNEURO_CUTS__SYMS_H_
#define VERILATED_VNEURO_CUTS__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vneuro_cuts.h"

// INCLUDE MODULE CLASSES
#include "Vneuro_cuts___024root.h"

// DPI TYPES for DPI Export callbacks (Internal use)

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vneuro_cuts__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vneuro_cuts* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vneuro_cuts___024root          TOP;

    // SCOPE NAMES
    VerilatedScope __Vscope_TOP;
    VerilatedScope __Vscope_neuro_cuts;

    // CONSTRUCTORS
    Vneuro_cuts__Syms(VerilatedContext* contextp, const char* namep, Vneuro_cuts* modelp);
    ~Vneuro_cuts__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
