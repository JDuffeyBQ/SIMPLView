/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

#include "DevHelper/CodeGenerators/FPCodeGenerator.h"

class IntVec2WidgetCodeGenerator : public FPCodeGenerator
{
public:
  SIMPL_SHARED_POINTERS(IntVec2WidgetCodeGenerator)

  static Pointer New(QString humanLabel, QString propertyName, QString category, QString initValue)
  {
    Pointer sharedPtr(new IntVec2WidgetCodeGenerator(humanLabel, propertyName, category, initValue));
    return sharedPtr;
  }

  virtual ~IntVec2WidgetCodeGenerator();

  virtual QString generateSetupFilterParameters();

  virtual QString generateDataCheck();

  virtual QString generateFilterParameters();

  virtual QList<QString> generateHIncludes();

protected:
  IntVec2WidgetCodeGenerator(QString humanLabel, QString propertyName, QString category, QString initValue);

public:
  IntVec2WidgetCodeGenerator(const IntVec2WidgetCodeGenerator&) = delete;            // Copy Constructor Not Implemented
  IntVec2WidgetCodeGenerator(IntVec2WidgetCodeGenerator&&) = delete;                 // Move Constructor Not Implemented
  IntVec2WidgetCodeGenerator& operator=(const IntVec2WidgetCodeGenerator&) = delete; // Copy Assignment Not Implemented
  IntVec2WidgetCodeGenerator& operator=(IntVec2WidgetCodeGenerator&&) = delete;      // Move Assignment Not Implemented
};
