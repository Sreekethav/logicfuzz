#ifndef DIO_H
#define DIO_H

#include "stub.h"

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId);
void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level);

#endif /* DIO_H */
/*==================[end of file]============================================*/
