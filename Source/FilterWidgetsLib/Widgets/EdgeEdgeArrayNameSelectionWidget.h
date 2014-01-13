#ifndef _EdgeEdgeArrayNameSelectionWidget_H_
#define _EdgeEdgeArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_EdgeEdgeArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT EdgeEdgeArrayNameSelectionWidget : public QWidget, private Ui::EdgeEdgeArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    EdgeEdgeArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~EdgeEdgeArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    EdgeEdgeArrayNameSelectionWidget(const EdgeEdgeArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const EdgeEdgeArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _EdgeEdgeArrayNameSelectionWidget_H_ */
