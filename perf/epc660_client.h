#pragma once
void epc660_setGrayMode();
void epc660_set3DMode();
void epc660_fetchdepthdata(int *pGray, int *pDepth, int expo_time, int flag);