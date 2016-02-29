/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#ifndef _CalculatorWidget_H_
#define _CalculatorWidget_H_




#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>

#include "QtSupportLib/FaderWidget.h"

#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/CoreFilters/ArrayCalculator.h"
#include "SIMPLib/FilterParameters/CalculatorFilterParameter.h"

#include "SIMPLViewWidgetsLib/SIMPLViewWidgetsLib.h"
#include "SIMPLViewWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"


#include "SIMPLViewWidgetsLib/ui_CalculatorWidget.h"


/**
* @brief This widget is for Filter Parameters that are of a string type.
* @author
* @version
*/
class SIMPLViewWidgetsLib_EXPORT CalculatorWidget : public FilterParameterWidget, private Ui::CalculatorWidget
{
    Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    CalculatorWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);

    virtual ~CalculatorWidget();

    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:
    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);

    void widgetChanged(const QString& msg);

    protected slots:
    void printButtonName();
    void printActionName();

    void on_clearBtn_pressed();
    void on_v1v2Btn_pressed();
    void on_xExpYBtn_pressed();
    void on_scalarsBtn_pressed();
    void on_vectorsBtn_pressed();

  signals:
    void errorSettingFilterParameter(const QString& msg);
    void parametersChanged();

  private:

    bool m_DidCausePreflight;

    ArrayCalculator*                  m_Filter;
    CalculatorFilterParameter*        m_FilterParameter;

    QMenu*                            m_ScalarsMenu;
    QMenu*                            m_VectorsMenu;

    CalculatorWidget(const CalculatorWidget&); // Copy Constructor Not Implemented
    void operator=(const CalculatorWidget&); // Operator '=' Not Implemented

};

#endif /* _CalculatorWidget_H_ */


