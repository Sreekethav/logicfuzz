#ifndef SCHM_PSM_H
#define SCHM_PSM_H

#include "stub.h"

FUNC(void, RTE_CODE) SchM_Enter_Psm_INTERRUPT_CONTROL_PROTECTION_AREA (void);
FUNC(void, RTE_CODE) SchM_Exit_Psm_INTERRUPT_CONTROL_PROTECTION_AREA (void);
FUNC(void, RTE_CODE) SchM_ActMainFunction_Psm_IrPsmId_PowerSequence (void);
FUNC(void, PSM_CODE) Psm_MainFunction_PowerSequence (void);

#endif
/* ==================[end of file]============================================ */
