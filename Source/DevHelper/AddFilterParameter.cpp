/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "AddFilterParameter.h"

#include <iostream>

#include "DevHelper/PluginMaker.h"
#include "DevHelper/FilterMaker.h"

#include "ui_AddFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterParameter::AddFilterParameter(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::AddFilterParameter)
{
  setAttribute(Qt::WA_DeleteOnClose);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterParameter::~AddFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::setupGui()
{
  m_Ui->setupUi(this);

  // Populate the type combo box
  QList<QString> typeList = getTypeList();
  for (int i = 0; i < typeList.size(); i++)
  {
    m_Ui->type->insertItem(i, typeList[i]);
  }

  // Populate the category combo box
  m_Ui->category->insertItem(0, "Parameter");
  m_Ui->category->insertItem(1, "Required Arrays");
  m_Ui->category->insertItem(2, "Created Arrays");

  m_Ui->errorString->setText("");

  // Update the "Add" button
  m_Ui->addFilterParameterBtn->setEnabled(filledOutCheck());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::on_addFilterParameterBtn_clicked()
{
  /* SLOT: FilterMaker::addFilterParameterToTable()
     CONNECT: FilterMaker::on_addFilterParameterBtn_clicked() */
  emit addBtnPressed(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::on_varName_textChanged(const QString& text)
{
  // Update the "Add" button
  if(m_Ui->type->currentText() == "SeparatorWidget")
  {
    m_Ui->addFilterParameterBtn->setEnabled(filledOutCheck_NoVarName());
  }
  else
  {
    m_Ui->addFilterParameterBtn->setEnabled(filledOutCheck());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::on_humanName_textChanged(const QString& text)
{
  // Update the "Add" button
  if(m_Ui->type->currentText() == "SeparatorWidget")
  {
    m_Ui->addFilterParameterBtn->setEnabled(filledOutCheck_NoVarName());
  }
  else
  {
    m_Ui->addFilterParameterBtn->setEnabled(filledOutCheck());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::on_type_currentTextChanged(const QString& text)
{
  if (text == "SeparatorWidget")
  {
    m_Ui->varName->clear();
    m_Ui->initValue->clear();
    m_Ui->varName->setEnabled(false);
    m_Ui->initValue->setEnabled(false);
  }
  else
  {
    m_Ui->varName->setEnabled(true);
    m_Ui->initValue->setEnabled(true);
  }

  // Update the "Add" button
  if(m_Ui->type->currentText() == "SeparatorWidget")
  {
    m_Ui->addFilterParameterBtn->setEnabled(filledOutCheck_NoVarName());
  }
  else
  {
    m_Ui->addFilterParameterBtn->setEnabled(filledOutCheck());
  }
}

//// -----------------------------------------------------------------------------
////
//// -----------------------------------------------------------------------------
//void AddFilterParameter::on_initValue_textChanged(const QString &text)
//{
//  // Update the "Add" button
//  addFilterParameterBtn->setEnabled(filledOutCheck());
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::on_cancelBtn_clicked()
{
  // Close the widget
  close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AddFilterParameter::getVariableName() const
{
  return m_Ui->varName->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AddFilterParameter::getHumanName() const
{
  return m_Ui->humanName->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AddFilterParameter::getType() const
{
  return m_Ui->type->currentText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AddFilterParameter::getCategory() const
{
  return m_Ui->category->currentText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> AddFilterParameter::getTypeList() const
{
#include "SVWidgetsLib/Filter_Parameter_TypeList.cpp"
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AddFilterParameter::getInitValue() const
{
  return m_Ui->initValue->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::setVariableName(const QString& varName)
{
  m_Ui->varName->setText(varName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::setHumanName(const QString& humanName)
{
  m_Ui->humanName->setText(humanName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::setType(const QString& type)
{
  m_Ui->type->setCurrentText(type);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::setCategory(const QString& category)
{
  m_Ui->category->setCurrentText(category);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterParameter::setInitValue(const QString& initValue)
{
  m_Ui->initValue->setText(initValue);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AddFilterParameter::filledOutCheck() const
{
  if(m_Ui->type->currentIndex() >= 0 && m_Ui->varName->text().isEmpty() == false && m_Ui->humanName->text().isEmpty() == false /* && initValue->text().isEmpty() == false */)
  {
    return true;
  }
  else
  {
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AddFilterParameter::filledOutCheck_NoVarName() const
{
  if(m_Ui->type->currentIndex() >= 0 && m_Ui->humanName->text().isEmpty() == false /* && initValue->text().isEmpty() == false */)
  {
    return true;
  }
  else
  {
    return false;
  }
}



