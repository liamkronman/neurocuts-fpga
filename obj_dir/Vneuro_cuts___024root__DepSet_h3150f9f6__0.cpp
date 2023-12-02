// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vneuro_cuts.h for the primary calling header

#include "Vneuro_cuts__pch.h"
#include "Vneuro_cuts___024root.h"

VL_INLINE_OPT void Vneuro_cuts___024root___ico_sequent__TOP__0(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___ico_sequent__TOP__0\n"); );
    // Body
    vlSelf->neuro_cuts__DOT__a = vlSelf->a;
    vlSelf->b = (1U & (IData)(vlSelf->a));
    vlSelf->neuro_cuts__DOT__b = vlSelf->b;
}

void Vneuro_cuts___024root___eval_ico(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___eval_ico\n"); );
    // Body
    if ((1ULL & vlSelf->__VicoTriggered.word(0U))) {
        Vneuro_cuts___024root___ico_sequent__TOP__0(vlSelf);
    }
}

void Vneuro_cuts___024root___eval_triggers__ico(Vneuro_cuts___024root* vlSelf);

bool Vneuro_cuts___024root___eval_phase__ico(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___eval_phase__ico\n"); );
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vneuro_cuts___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelf->__VicoTriggered.any();
    if (__VicoExecute) {
        Vneuro_cuts___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vneuro_cuts___024root___eval_act(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___eval_act\n"); );
}

void Vneuro_cuts___024root___eval_nba(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___eval_nba\n"); );
}

void Vneuro_cuts___024root___eval_triggers__act(Vneuro_cuts___024root* vlSelf);

bool Vneuro_cuts___024root___eval_phase__act(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___eval_phase__act\n"); );
    // Init
    VlTriggerVec<0> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vneuro_cuts___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelf->__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelf->__VactTriggered, vlSelf->__VnbaTriggered);
        vlSelf->__VnbaTriggered.thisOr(vlSelf->__VactTriggered);
        Vneuro_cuts___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vneuro_cuts___024root___eval_phase__nba(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___eval_phase__nba\n"); );
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelf->__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vneuro_cuts___024root___eval_nba(vlSelf);
        vlSelf->__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vneuro_cuts___024root___dump_triggers__ico(Vneuro_cuts___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vneuro_cuts___024root___dump_triggers__nba(Vneuro_cuts___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vneuro_cuts___024root___dump_triggers__act(Vneuro_cuts___024root* vlSelf);
#endif  // VL_DEBUG

void Vneuro_cuts___024root___eval(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___eval\n"); );
    // Init
    IData/*31:0*/ __VicoIterCount;
    CData/*0:0*/ __VicoContinue;
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VicoIterCount = 0U;
    vlSelf->__VicoFirstIteration = 1U;
    __VicoContinue = 1U;
    while (__VicoContinue) {
        if (VL_UNLIKELY((0x64U < __VicoIterCount))) {
#ifdef VL_DEBUG
            Vneuro_cuts___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("neuro_cuts.sv", 2, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vneuro_cuts___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelf->__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY((0x64U < __VnbaIterCount))) {
#ifdef VL_DEBUG
            Vneuro_cuts___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("neuro_cuts.sv", 2, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelf->__VactIterCount = 0U;
        vlSelf->__VactContinue = 1U;
        while (vlSelf->__VactContinue) {
            if (VL_UNLIKELY((0x64U < vlSelf->__VactIterCount))) {
#ifdef VL_DEBUG
                Vneuro_cuts___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("neuro_cuts.sv", 2, "", "Active region did not converge.");
            }
            vlSelf->__VactIterCount = ((IData)(1U) 
                                       + vlSelf->__VactIterCount);
            vlSelf->__VactContinue = 0U;
            if (Vneuro_cuts___024root___eval_phase__act(vlSelf)) {
                vlSelf->__VactContinue = 1U;
            }
        }
        if (Vneuro_cuts___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vneuro_cuts___024root___eval_debug_assertions(Vneuro_cuts___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vneuro_cuts__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vneuro_cuts___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->a & 0xfcU))) {
        Verilated::overWidthError("a");}
}
#endif  // VL_DEBUG
