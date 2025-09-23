#ifndef FORMULAR_ENGINE_
#define FORMULAR_ENGINE_

#include "CFormularContent.h"
#include "CFormularComputeParent.h"
#include "CFormularCompute.h"
#include "GmtTime.h"
#include "SelectStockStruct.h"
#include "define.h"
#include "TechExport.h"

EXPORT_API T_IndexOutArray* formula_index( CFormularContent* pContent, CArray<CKLine,CKLine>& KLines,MERCH_EXTRA_INFO *pExtraInfo=NULL /*T_MerchNodeUserData *pUserData=NULL*/);
EXPORT_API int ComputeFormu2(CFormularCompute*& pEqution,INPUT_PARAM& input);// 计算所指定指标的数值，need to delete pEqution
void GetParams(IN char* StrParam, OUT float& fValue1, OUT float& fValue2, OUT float& fValue3, OUT float& fValue4);


#endif