///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GRAINGENERATOR_H_
#define GRAINGENERATOR_H_

#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/Common/MXATypes.h>
#include <AIM/Common/Constants.h>
#include <AIM/Common/GrainGeneratorFunc.h>


#ifdef AIM_USE_QT
#include <QtCore/QObject>
#include <QtCore/QThread>
#define AIM_STRING QString
#else
#define AIM_STRING std::string
#endif



class GrainGenerator
#ifdef AIM_USE_QT
 : public QThread
#endif
{
#ifdef AIM_USE_QT
Q_OBJECT
#endif

  public:
    MXA_SHARED_POINTERS(GrainGenerator);
    MXA_TYPE_MACRO(GrainGenerator);

#ifdef AIM_USE_QT
    static Pointer New (QObject* parent = 0);
#else
    MXA_STATIC_NEW_MACRO(GrainGenerator);
#endif
    virtual ~GrainGenerator();


    MXA_INSTANCE_STRING_PROPERTY(InputDirectory, m_InputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory, m_OutputDirectory)
    MXA_INSTANCE_PROPERTY_m(int, NumGrains)
    MXA_INSTANCE_PROPERTY_m(int, ShapeClass)
    MXA_INSTANCE_PROPERTY_m(double, XResolution)
    MXA_INSTANCE_PROPERTY_m(double, YResolution)
    MXA_INSTANCE_PROPERTY_m(double, ZResolution)
    MXA_INSTANCE_PROPERTY_m(double, OverlapAllowed)
    MXA_INSTANCE_PROPERTY_m(int, OverlapAssignment)
    MXA_INSTANCE_PROPERTY_m(int, CrystalStructure)
    MXA_INSTANCE_PROPERTY_m(int, ErrorCondition);

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the GrainGenerator normalized to a value between 0 and 100
     */
    void progressMessage(AIM_STRING message, int progress);

#ifdef AIM_USE_QT

    /**
     * @brief Cancel the operation
     */
    MXA_INSTANCE_PROPERTY_m(bool, Cancel);

    /**
     * Qt Signals for connections
     */
    signals:
    //  void workerComplete();
      void updateMessage(QString message);
      void updateProgress(int value);

    public slots:
    /**
     * @brief Slot to receive a signal to cancel the operation
     */
      void on_CancelWorker();


#endif
      /**
       * @brief Main method to run the operation
       */
      void compute();

      /**
       * @brief Temporary to get the code running. Eventually this will be replaced
       * with the normal 'compute' method.
       */
      void jackson_compute();

  protected:
#ifdef AIM_USE_QT
    GrainGenerator(QObject* parent = 0);
    virtual void run();
#else

    GrainGenerator();
#endif

  private:
    GrainGeneratorFunc::Pointer m;

    GrainGenerator(const GrainGenerator&);    // Copy Constructor Not Implemented
    void operator=(const GrainGenerator&);  // Operator '=' Not Implemented
};

#endif /* GRAINGENERATOR_H_ */
