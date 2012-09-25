/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef __ARCH_INVOCATION_H
#define __ARCH_INVOCATION_H

enum arch_invocation_label {
    IA32PageTableMap = nInvocationLabels + 20,
    IA32PageTableUnmap = nInvocationLabels + 21,
    IA32PageMap = nInvocationLabels + 22,
    IA32PageRemap = nInvocationLabels + 23,
    IA32PageUnmap = nInvocationLabels + 24,
    IA32ASIDControlMakePool = nInvocationLabels + 25,
    IA32ASIDPoolAssign = nInvocationLabels + 26,
    IA32IOPortIn8 = nInvocationLabels + 27,
    IA32IOPortIn16 = nInvocationLabels + 28,
    IA32IOPortIn32 = nInvocationLabels + 29,
    IA32IOPortOut8 = nInvocationLabels + 30,
    IA32IOPortOut16 = nInvocationLabels + 31,
    IA32IOPortOut32 = nInvocationLabels + 32,

    IA32IOPageTableMap = nInvocationLabels + 33,
    IA32PageMapIO = nInvocationLabels + 34

};

#endif 
