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

#include "AddFilterWidget.h"

#include "ui_AddFilterWidget.h"

namespace
{
QString cleanName(const QString& name)
{
  // Remove all spaces and illegal characters from plugin name
  QString cleanedName = name.trimmed();
  cleanedName.remove(" ");
  cleanedName.remove(QRegExp("[^a-zA-Z_\\d\\s]"));
  return cleanedName;
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterWidget::AddFilterWidget(QWidget* parent)
: QDialog(parent)
, m_Ui(new Ui::Dialog)
{
  m_Ui->setupUi(this);
  m_Ui->addfilterOKButton->setEnabled(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddFilterWidget ::~AddFilterWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AddFilterWidget::isPublic() const
{
  return m_Ui->m_PublicFilter->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AddFilterWidget::getFilterName() const
{
  return (m_Ui->filterName->text());
}

// -----------------------------------------------------------------------------
//  Called when the OK button is clicked.
// -----------------------------------------------------------------------------
void AddFilterWidget::on_addfilterOKButton_clicked()
{
  BtnClicked = true;
  this->close();
}

// -----------------------------------------------------------------------------
//  Called when the Cancel button is clicked.
// -----------------------------------------------------------------------------
void AddFilterWidget::on_addfilterCancelButton_clicked()
{
  BtnClicked = false;
  this->close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AddFilterWidget::getBtnClicked() const
{
  return BtnClicked;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFilterWidget::on_filterName_textChanged(const QString& text)
{
  QString filterName_clean = cleanName(m_Ui->filterName->text());
  if ( filterName_clean.isEmpty() )
  {
    m_Ui->addfilterOKButton->setEnabled(false);
  }
  else
  {
    m_Ui->addfilterOKButton->setEnabled(true);
  }
}
