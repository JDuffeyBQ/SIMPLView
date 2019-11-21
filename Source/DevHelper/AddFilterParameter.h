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

#pragma once

#include <QtWidgets/QWidget>

namespace Ui
{
class AddFilterParameter;
}

class AddFilterParameter : public QWidget
{
    Q_OBJECT

  public:
    AddFilterParameter(QWidget* parent = nullptr);
    ~AddFilterParameter() override;

    QString getVariableName() const;
    QString getHumanName() const;
    QString getType() const;
    QString getCategory() const;
    QList<QString> getTypeList() const;
    QString getInitValue() const;

    void setVariableName(const QString& varName);
    void setHumanName(const QString& humanName);
    void setType(const QString& type);
    void setCategory(const QString& category);
    void setInitValue(const QString& initValue);

  protected:
    void setupGui();

  signals:
    void addBtnPressed(AddFilterParameter* widget);

  protected slots:
    void on_addFilterParameterBtn_clicked();
    void on_cancelBtn_clicked();
    void on_varName_textChanged(const QString& text);
    void on_humanName_textChanged(const QString& text);
    void on_type_currentTextChanged(const QString& text);
    //void on_initValue_textChanged(const QString &text);

  private:
    QSharedPointer<Ui::AddFilterParameter> m_Ui = nullptr;
    bool filledOutCheck() const;
    bool filledOutCheck_NoVarName() const;

  public:
    AddFilterParameter(const AddFilterParameter&) = delete;            // Copy Constructor Not Implemented
    AddFilterParameter(AddFilterParameter&&) = delete;                 // Move Constructor Not Implemented
    AddFilterParameter& operator=(const AddFilterParameter&) = delete; // Copy Assignment Not Implemented
    AddFilterParameter& operator=(AddFilterParameter&&) = delete;      // Move Assignment Not Implemented
};

