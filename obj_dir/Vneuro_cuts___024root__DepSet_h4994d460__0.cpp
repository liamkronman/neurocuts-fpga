// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vneuro_cuts.h for the primary calling header

#include "Vneuro_cuts__pch.h"
#include "Vneuro_cuts__Syms.h"
#include "Vneuro_cuts___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vneuro_cuts___024root___dump_triggers__ico(Vneuro_cuts___024root* vlSelf);
#endif  // VL_DEBUG

void Vneuro_cuts___024root___eval_triggers__ico(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___eval_triggers__ico\n"); );
    // Body
    vlSelf->__VicoTriggered.set(0U, (IData)(vlSelf->__VicoFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vneuro_cuts___024root___dump_triggers__ico(vlSelf);
    }
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vneuro_cuts___024root___dump_triggers__act(Vneuro_cuts___024root* vlSelf);
#endif  // VL_DEBUG

void Vneuro_cuts___024root___eval_triggers__act(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___eval_triggers__act\n"); );
    // Body
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vneuro_cuts___024root___dump_triggers__act(vlSelf);
    }
#endif
}
