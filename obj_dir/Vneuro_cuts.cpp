// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vneuro_cuts__pch.h"

//============================================================
// Constructors

Vneuro_cuts::Vneuro_cuts(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vneuro_cuts__Syms(contextp(), _vcname__, this)}
    , a{vlSymsp->TOP.a}
    , b{vlSymsp->TOP.b}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vneuro_cuts::Vneuro_cuts(const char* _vcname__)
    : Vneuro_cuts(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vneuro_cuts::~Vneuro_cuts() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vneuro_cuts___024root___eval_debug_assertions(Vneuro_cuts___024root* vlSelf);
#endif  // VL_DEBUG
void Vneuro_cuts___024root___eval_static(Vneuro_cuts___024root* vlSelf);
void Vneuro_cuts___024root___eval_initial(Vneuro_cuts___024root* vlSelf);
void Vneuro_cuts___024root___eval_settle(Vneuro_cuts___024root* vlSelf);
void Vneuro_cuts___024root___eval(Vneuro_cuts___024root* vlSelf);

void Vneuro_cuts::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vneuro_cuts::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vneuro_cuts___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vneuro_cuts___024root___eval_static(&(vlSymsp->TOP));
        Vneuro_cuts___024root___eval_initial(&(vlSymsp->TOP));
        Vneuro_cuts___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vneuro_cuts___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vneuro_cuts::eventsPending() { return false; }

uint64_t Vneuro_cuts::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "%Error: No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vneuro_cuts::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vneuro_cuts___024root___eval_final(Vneuro_cuts___024root* vlSelf);

VL_ATTR_COLD void Vneuro_cuts::final() {
    Vneuro_cuts___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vneuro_cuts::hierName() const { return vlSymsp->name(); }
const char* Vneuro_cuts::modelName() const { return "Vneuro_cuts"; }
unsigned Vneuro_cuts::threads() const { return 1; }
void Vneuro_cuts::prepareClone() const { contextp()->prepareClone(); }
void Vneuro_cuts::atClone() const {
    contextp()->threadPoolpOnClone();
}

//============================================================
// Trace configuration

VL_ATTR_COLD void Vneuro_cuts::trace(VerilatedVcdC* tfp, int levels, int options) {
    vl_fatal(__FILE__, __LINE__, __FILE__,"'Vneuro_cuts::trace()' called on model that was Verilated without --trace option");
}
