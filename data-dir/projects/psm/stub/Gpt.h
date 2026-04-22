#ifndef GPT_H
#define GPT_H

#include "stub.h"

Gpt_ValueType Gpt_GetTimeElapsed(Gpt_ChannelType Channel);
Gpt_ValueType Gpt_GetTimeRemaining(Gpt_ChannelType Channel);
void Gpt_StartTimer(Gpt_ChannelType Channel, Gpt_ValueType Value);
void Gpt_StopTimer(Gpt_ChannelType Channel);
void Gpt_EnableNotification(Gpt_ChannelType Channel);
void Gpt_DisableNotification(Gpt_ChannelType Channel);

#endif
/*==================[end of file]===========================================*/
