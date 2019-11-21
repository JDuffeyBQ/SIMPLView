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


#include "PluginMaker.h"
#include <iostream>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QWidget>

#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSApplicationFileInfo.h"

#include "AddFilterWidget.h"
#include "FilterBundler.h"
#include "PMDirGenerator.h"
#include "PMFileGenerator.h"
#include "PMGeneratorTreeItem.h"
#include "BrandedStrings.h"

#include "ui_PluginMaker.h"

namespace
{
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString cleanName(const QString& name)
{
  QString cleanedName = name;

  // Remove all uses of "Plugin", "plugin", "Filter", and "filter"
  QRegExp rx("Filter|filter");
  cleanedName.replace(rx, "");
  // Remove all spaces and illegal characters from plugin name
  cleanedName = cleanedName.trimmed();
  cleanedName.remove(" ");
  cleanedName.remove(QRegExp("[^a-zA-Z_\\d\\s]"));
  return cleanedName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString cleanName_filters(const QString& name)
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
PluginMaker::PluginMaker(QWidget* parent) :
  QWidget(parent)
, m_Ui(new Ui::PluginMaker)
{
  m_OpenDialogLastDirectory = QDir::homePath();

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginMaker::~PluginMaker() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::setupGui()
{
  m_Ui->setupUi(this);

  // Setup error string
  m_Ui->errorString->setTextFormat(Qt::PlainText);
  m_Ui->errorString->setTextInteractionFlags(Qt::NoTextInteraction);
  m_Ui->errorString->changeStyleSheet(QtSFSDropLabel::FS_DOESNOTEXIST_STYLE);
  m_Ui->errorString->setText("");

  // Stretch Factors
  m_Ui->splitter->setStretchFactor(0, 0);
  m_Ui->splitter->setStretchFactor(1, 1);

  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->m_OutputDir->setCompleter(com);
  QObject::connect(com, SIGNAL(activated(const QString&)), this, SLOT(on_m_OutputDir_textChanged(const QString&)));

  QString pathTemplate;

  m_Ui->nameLabel->setToolTip("Plugin Name");
  m_Ui->m_PluginName->setToolTip("Enter Plugin Name Here");
  m_Ui->outputFileNameLabel->setToolTip("Output Directory");
  m_Ui->m_OutputDir->setToolTip("Enter Output Directory Here");
  m_Ui->selectButton->setToolTip("Select Directory");
  m_Ui->generateButton->setToolTip("Generate File Structure");


  PMGeneratorTreeItem* F_main = new PMGeneratorTreeItem(m_Ui->treeWidget);
  F_main->setText(0, "Unknown Plugin Name");
  {
    pathTemplate = "@PluginName@";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_Ui->m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             F_main,
                                             this);
    gen->setDisplaySuffix("");
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            gen, SLOT(pluginNameChanged(QString)));
  }

  //// This is for the .clang-format File Generation (this is an invisible file, and should not appear in the preview window)
  {
    pathTemplate = "@PluginName@";
    QString resourceTemplate( QtSApplicationFileInfo::GenerateFileSystemPath("/Template/.clang-format.in") );
    PMFileGenerator* clangFormatGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                              pathTemplate,
                                                              QString(".clang-format"),
                                                              resourceTemplate,
                                                              nullptr,
                                                              this);
    clangFormatGen->setDoesGenerateOutput(true);
    clangFormatGen->setNameChangeable(false);

    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            clangFormatGen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            clangFormatGen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            clangFormatGen, SLOT(generateOutput()));
    connect(clangFormatGen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
  }

  //// This is for the @PluginName@Constants File Generation
  PMGeneratorTreeItem* pluginConstants = new PMGeneratorTreeItem(F_main);
  pluginConstants->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@";
  QString resourceTemplate( QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Constants.h.in") );
  PMFileGenerator* constantsPluginGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                            pathTemplate,
                                                            QString(""),
                                                            resourceTemplate,
                                                            pluginConstants,
                                                            this);
  pluginConstants->setFileGenPtr(constantsPluginGen);
  constantsPluginGen->setDisplaySuffix("Constants.h");
  constantsPluginGen->setDoesGenerateOutput(true);
  constantsPluginGen->setNameChangeable(true);
  connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
          constantsPluginGen, SLOT(pluginNameChanged(QString)));
  connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
          constantsPluginGen, SLOT(outputDirChanged(QString)));
  // For "Directories" this probably isn't needed
  connect(m_Ui->generateButton, SIGNAL(clicked()),
          constantsPluginGen, SLOT(generateOutput()));
  connect(constantsPluginGen, SIGNAL(outputError(QString)),
          this, SLOT(generationError(QString)));


  //// This is for the @PluginName@Plugin.cpp File Generation
  PMGeneratorTreeItem* pluginCPP = new PMGeneratorTreeItem(F_main);
  pluginCPP->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@";
  resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Plugin.cpp.in");
  PMFileGenerator* cppPluginGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                      pathTemplate,
                                                      QString(""),
                                                      resourceTemplate,
                                                      pluginCPP,
                                                      this);
  pluginCPP->setFileGenPtr(cppPluginGen);
  cppPluginGen->setDisplaySuffix("Plugin.cpp");
  cppPluginGen->setDoesGenerateOutput(true);
  cppPluginGen->setNameChangeable(true);
  connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
          cppPluginGen, SLOT(pluginNameChanged(QString)));
  connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
          cppPluginGen, SLOT(outputDirChanged(QString)));
  // For "Directories" this probably isn't needed
  connect(m_Ui->generateButton, SIGNAL(clicked()),
          cppPluginGen, SLOT(generateOutput()));
  connect(cppPluginGen, SIGNAL(outputError(QString)),
          this, SLOT(generationError(QString)));


  //// This is for the @PluginName@Plugin.h File Generation
  PMGeneratorTreeItem* pluginH = new PMGeneratorTreeItem(F_main);
  pluginH->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@";
  resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Plugin.h.in");
  PMFileGenerator* hPluginGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(""),
                                                    resourceTemplate,
                                                    pluginH,
                                                    this);
  pluginH->setFileGenPtr(hPluginGen);
  hPluginGen->setDisplaySuffix("Plugin.h");
  hPluginGen->setDoesGenerateOutput(true);
  hPluginGen->setNameChangeable(true);
  connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
          hPluginGen, SLOT(pluginNameChanged(QString)));
  connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
          hPluginGen, SLOT(outputDirChanged(QString)));
  // For "Directories" this probably isn't needed
  connect(m_Ui->generateButton, SIGNAL(clicked()),
          hPluginGen, SLOT(generateOutput()));
  connect(hPluginGen, SIGNAL(outputError(QString)),
          this, SLOT(generationError(QString)));


  F_res = new PMGeneratorTreeItem(F_main);
  F_res->setText(0, tr("Resources"));

  PMGeneratorTreeItem* resources_sourceList = new PMGeneratorTreeItem(F_res);
  resources_sourceList->setText(0, tr("SourceList.cmake"));
  {
    pathTemplate = "@PluginName@/Resources";

    QString resourceTemplate(QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Resources/SourceList.cmake.in"));
    PMFileGenerator* gen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                               pathTemplate,
                                               QString("SourceList.cmake"),
                                               resourceTemplate,
                                               resources_sourceList,
                                               this);

    resources_sourceList->setFileGenPtr(gen);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            gen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            gen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
    gen->setDoesGenerateOutput(true);
  }

  PMGeneratorTreeItem* resources_qrc = new PMGeneratorTreeItem(F_res);
  resources_qrc->setText(0, tr("Unknown Plugin Name"));
  {
    pathTemplate = "@PluginName@/Resources";

    QString resourceTemplate(QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Resources/Filter.qrc.in"));
    PMFileGenerator* gen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                               pathTemplate,
                                               QString(""),
                                               resourceTemplate,
                                               resources_qrc,
                                               this);

    resources_qrc->setFileGenPtr(gen);
    gen->setNameChangeable(true);
    gen->setDisplaySuffix(".qrc");
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            gen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            gen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
    gen->setDoesGenerateOutput(true);
  }

  F_res_sub = new PMGeneratorTreeItem(F_res);
  F_res_sub->setText(0, tr("Unknown Plugin Name"));
  {
    pathTemplate = "@PluginName@/Resources";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_Ui->m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             F_res_sub,
                                             this);
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            gen, SLOT(pluginNameChanged(QString)));
  }

  PMGeneratorTreeItem* resources_license = new PMGeneratorTreeItem(F_res_sub);
  resources_license->setText(0, tr("Unknown Plugin Name"));
  {
    pathTemplate = "@PluginName@/Resources/@PluginName@";

    QString resourceTemplate(QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Resources/FilterLicense.txt.in"));
    PMFileGenerator* gen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                               pathTemplate,
                                               QString(""),
                                               resourceTemplate,
                                               resources_license,
                                               this);

    resources_license->setFileGenPtr(gen);
    gen->setNameChangeable(true);
    gen->setDoesGenerateOutput(true);
    gen->setDisplaySuffix("License.txt");
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            gen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            gen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
    gen->setDoesGenerateOutput(true);
  }

  PMGeneratorTreeItem* resources_description = new PMGeneratorTreeItem(F_res_sub);
  resources_description->setText(0, tr("Unknown Plugin Name"));
  {
    pathTemplate = "@PluginName@/Resources/@PluginName@";

    QString resourceTemplate(QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Resources/FilterDescription.txt.in"));
    PMFileGenerator* gen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                               pathTemplate,
                                               QString(""),
                                               resourceTemplate,
                                               resources_description,
                                               this);

    resources_description->setFileGenPtr(gen);
    gen->setNameChangeable(true);
    gen->setDoesGenerateOutput(true);
    gen->setDisplaySuffix("Description.txt");
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            gen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            gen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
  }

  F_test = new PMGeneratorTreeItem(F_main);
  F_test->setText(0, tr("Test"));

  PMGeneratorTreeItem* F_test_cmake = new PMGeneratorTreeItem(F_test);
  F_test_cmake->setText(0, tr("CMakeLists.txt"));
  {
    pathTemplate = "@PluginName@/Test";
    QString resourceTemplate(QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Test/CMakeLists.txt.in"));
    PMFileGenerator* gen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                               pathTemplate,
                                               QString("CMakeLists.txt"),
                                               resourceTemplate,
                                               F_test_cmake,
                                               this);

    F_test_cmake->setFileGenPtr(gen);
    gen->setDoesGenerateOutput(true);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            gen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            gen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            this, SLOT(testFileLocationsHandler()));
    connect(this, SIGNAL(clicked(QSet<QString>)),
            gen, SLOT(generateOutputWithFilterNames(QSet<QString>)));
    connect(gen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
  }

  PMGeneratorTreeItem* F_test_fileLoc = new PMGeneratorTreeItem(F_test);
  F_test_fileLoc->setText(0, tr("TestFileLocations.h.in"));
  {
    pathTemplate = "@PluginName@/Test";
    QString resourceTemplate(QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Test/TestFileLocations.h.in.in"));
    PMFileGenerator* gen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                               pathTemplate,
                                               QString("TestFileLocations.h.in"),
                                               resourceTemplate,
                                               F_test_fileLoc,
                                               this);

    m_TestFileLocationNames.insert("@PluginName@Filter");   // This name needs to be used in the TestFileLocations.h.in file
    F_test_fileLoc->setFileGenPtr(gen);
    gen->setDoesGenerateOutput(true);
    gen->setNameChangeable(false);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            gen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            gen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            this, SLOT(testFileLocationsHandler()));
    connect(this, SIGNAL(clicked(QSet<QString>)),
            gen, SLOT(generateOutputWithFilterNames(QSet<QString>)));
    connect(gen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
  }

  PMGeneratorTreeItem* F_test_filterTest = new PMGeneratorTreeItem(F_test);
  F_test_filterTest->setText(0, tr("Unknown Plugin Name"));
  pathTemplate = "@PluginName@/Test";
  resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Test/FilterTest.cpp.in");
  PMFileGenerator* testgen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                 pathTemplate,
                                                 QString("@PluginName@"),
                                                 resourceTemplate,
                                                 F_test_filterTest,
                                                 this);

  F_test_filterTest->setFileGenPtr(testgen);
  testgen->setDoesGenerateOutput(true);
  testgen->setNameChangeable(true);
  testgen->setDisplaySuffix("FilterTest.cpp");

  connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
          testgen, SLOT(pluginNameChanged(QString)));
  connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
          testgen, SLOT(outputDirChanged(QString)));
  // For "Directories" this probably isn't needed
  connect(m_Ui->generateButton, SIGNAL(clicked()),
          testgen, SLOT(generateOutput()));
  connect(testgen, SIGNAL(outputError(QString)),
          this, SLOT(generationError(QString)));


  F_doc = new PMGeneratorTreeItem(F_main);
  F_doc->setText(0, tr("Documentation"));

  PMGeneratorTreeItem* cmake = new PMGeneratorTreeItem(F_main);
  cmake->setText(0, tr("CMakeLists.txt"));
  {
    pathTemplate = "@PluginName@";

    QString resourceTemplate(QtSApplicationFileInfo::GenerateFileSystemPath("/Template/CMakeLists.txt.in"));
    PMFileGenerator* gen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                               pathTemplate,
                                               QString("CMakeLists.txt"),
                                               resourceTemplate,
                                               cmake,
                                               this);

    cmake->setFileGenPtr(gen);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            gen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            gen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
    gen->setDoesGenerateOutput(true);
  }

  // Dummy HTML file (to bundle the plugin CPP and H files in a FilterBundler)
  PMFileGenerator* htmlPluginDoc = new PMFileGenerator("", "", "", "", nullptr, this);

  // Dummy test file (to bundle the plugin CPP and H files in a FilterBundler)
  PMFileGenerator* testPluginFile = new PMFileGenerator("", "", "", "", nullptr, this);

  FilterBundler fb(cppPluginGen, hPluginGen, htmlPluginDoc, testPluginFile, true);



//// TreeItem for the PluginNameFilters directory
  F_name = new PMGeneratorTreeItem(F_main);
  F_name->setText(0, "Unknown Plugin Name");
  {
    pathTemplate = "@PluginName@/@PluginName@Filters/";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_Ui->m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             F_name,
                                             this);
    gen->setDisplaySuffix("Filters");
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)), gen, SLOT(pluginNameChanged(QString)));
  }

  //// TreeItem for a Filter Class source filfe
  PMGeneratorTreeItem* filterCPP = new PMGeneratorTreeItem(F_name);
  filterCPP->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@/@PluginName@Filters/";
  resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.cpp.in");
  PMFileGenerator* cppFilterGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                      pathTemplate,
                                                      QString(""),
                                                      resourceTemplate,
                                                      filterCPP,
                                                      this);
  filterCPP->setFileGenPtr(cppFilterGen);
  cppFilterGen->setDisplaySuffix("Filter.cpp");
  cppFilterGen->setDoesGenerateOutput(true);
  cppFilterGen->setNameChangeable(true);
  cppFilterGen->setInitListContents("");
  connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
          cppFilterGen, SLOT(pluginNameChanged(QString)));
  connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
          cppFilterGen, SLOT(outputDirChanged(QString)));
  // For "Directories" this probably isn't needed
  connect(m_Ui->generateButton, SIGNAL(clicked()),
          cppFilterGen, SLOT(generateOutput()));
  connect(cppFilterGen, SIGNAL(outputError(QString)),
          this, SLOT(generationError(QString)));

  //// TreeItem for a Filter Class Header
  PMGeneratorTreeItem* filterH = new PMGeneratorTreeItem(F_name);
  filterH->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@/@PluginName@Filters/";
  resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.h.in");
  PMFileGenerator* hFilterGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(""),
                                                    resourceTemplate,
                                                    filterH,
                                                    this);
  filterH->setFileGenPtr(hFilterGen);
  hFilterGen->setDisplaySuffix("Filter.h");
  hFilterGen->setDoesGenerateOutput(true);
  hFilterGen->setNameChangeable(true);
  connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
          hFilterGen, SLOT(pluginNameChanged(QString)));
  connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
          hFilterGen, SLOT(outputDirChanged(QString)));
  // For "Directories" this probably isn't needed
  connect(m_Ui->generateButton, SIGNAL(clicked()),
          hFilterGen, SLOT(generateOutput()));
  connect(hFilterGen, SIGNAL(outputError(QString)),
          this, SLOT(generationError(QString)));

  //// TreeItem for the SourceList.cmake file that will compile all the filters
  PMGeneratorTreeItem* sourceList = new PMGeneratorTreeItem(F_name);
  sourceList->setText(0, tr("SourceList.cmake"));

  //// This adds a "Directory" TreeItem for the Gui folder
  QTreeWidgetItem* fpw_fp = new PMGeneratorTreeItem(F_main);
  fpw_fp->setText(0, "FilterParameters");
  {  
    //// This is for the FilerParameters->SourceList.txt File Generation
    PMGeneratorTreeItem* pluginCM = new PMGeneratorTreeItem(fpw_fp);
    pluginCM->setText(0, "SourceList.cmake");
    pathTemplate = "@PluginName@/FilterParameters/";
    resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/FilterParameters/SourceList.cmake.in");
    PMFileGenerator* cmPluginGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                      pathTemplate,
                                                      QString("SourceList.cmake"),
                                                      resourceTemplate,
                                                      pluginCM,
                                                      this);
    pluginCM->setFileGenPtr(cmPluginGen);
    cmPluginGen->setDisplaySuffix("SourceList.cmake");
    cmPluginGen->setDoesGenerateOutput(true);
    cmPluginGen->setNameChangeable(false);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            cmPluginGen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            cmPluginGen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            cmPluginGen, SLOT(generateOutput()));
    connect(cmPluginGen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));

  }
  
  //// This adds a "Directory" TreeItem for the Gui folder
  QTreeWidgetItem* fpw_gui = new PMGeneratorTreeItem(F_main);
  fpw_gui->setText(0, "Gui");
  {
      
    //// This is for the CMakeLists.txt File Generation
    PMGeneratorTreeItem* pluginCM = new PMGeneratorTreeItem(fpw_gui);
    pluginCM->setText(0, "CMakeLists.txt");
    pathTemplate = "@PluginName@/Gui/";
    resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Gui/CMakeLists.txt.in");
    PMFileGenerator* cmPluginGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                      pathTemplate,
                                                      QString("CMakeLists.txt"),
                                                      resourceTemplate,
                                                      pluginCM,
                                                      this);
    pluginCM->setFileGenPtr(cmPluginGen);
    cmPluginGen->setDisplaySuffix("CMakeLists.txt");
    cmPluginGen->setDoesGenerateOutput(true);
    cmPluginGen->setNameChangeable(false);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            cmPluginGen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            cmPluginGen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            cmPluginGen, SLOT(generateOutput()));
    connect(cmPluginGen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
            
            
    //// This is for the @PluginName@GuiPlugin.cpp File Generation
    PMGeneratorTreeItem* pluginCPP = new PMGeneratorTreeItem(fpw_gui);
    pluginCPP->setText(0, "Unknown Plugin Name");
    pathTemplate = "@PluginName@/Gui/";
    resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Gui/GuiPlugin.cpp.in");
    PMFileGenerator* cppPluginGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                        pathTemplate,
                                                        QString(""),
                                                        resourceTemplate,
                                                        pluginCPP,
                                                        this);
    pluginCPP->setFileGenPtr(cppPluginGen);
    cppPluginGen->setDisplaySuffix("GuiPlugin.cpp");
    cppPluginGen->setDoesGenerateOutput(true);
    cppPluginGen->setNameChangeable(true);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            cppPluginGen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            cppPluginGen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            cppPluginGen, SLOT(generateOutput()));
    connect(cppPluginGen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
    
    
    //// This is for the @PluginName@GuiPlugin.h File Generation
    PMGeneratorTreeItem* pluginH = new PMGeneratorTreeItem(fpw_gui);
    pluginH->setText(0, "Unknown Plugin Name");
    pathTemplate = "@PluginName@/Gui/";
    resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Gui/GuiPlugin.h.in");
    PMFileGenerator* hPluginGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                      pathTemplate,
                                                      QString(""),
                                                      resourceTemplate,
                                                      pluginH,
                                                      this);
    pluginH->setFileGenPtr(hPluginGen);
    hPluginGen->setDisplaySuffix("GuiPlugin.h");
    hPluginGen->setDoesGenerateOutput(true);
    hPluginGen->setNameChangeable(true);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            hPluginGen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            hPluginGen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            hPluginGen, SLOT(generateOutput()));
    connect(hPluginGen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
  
  
  }

  //// This adds a "Directory" TreeItem for the FilterParameterWidgets folder
  {
    QTreeWidgetItem* fpw_Name = new PMGeneratorTreeItem(fpw_gui);
    fpw_Name->setText(0, "FilterParameterWidgets");

    pathTemplate = "@PluginName@/Gui/FilterParameterWidgets/";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_Ui->m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             fpw_Name,
                                             this);
    gen->setDisplaySuffix("FilterParameterWidgets");
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);

    /// Add TreeItem for the FilterParameterWidgets/SourceList.cmake file
    PMGeneratorTreeItem* fpw_SourceList = new PMGeneratorTreeItem(fpw_Name);
    fpw_SourceList->setText(0, "SourceList.cmake");
    pathTemplate = "@PluginName@/Gui/FilterParameterWidgets/";
    resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Gui/FilterParameterWidgets/SourceList.cmake.in");
    PMFileGenerator* fpwFilterGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                        pathTemplate,
                                                        QString("SourceList.cmake"),
                                                        resourceTemplate,
                                                        fpw_SourceList,
                                                        this);
    fpw_SourceList->setFileGenPtr(fpwFilterGen);
    fpwFilterGen->setDisplaySuffix("");
    fpwFilterGen->setDoesGenerateOutput(true);
    fpwFilterGen->setNameChangeable(false);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            fpwFilterGen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            fpwFilterGen, SLOT(outputDirChanged(QString)));
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            fpwFilterGen, SLOT(generateOutput()));
    connect(fpwFilterGen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));

  }


  //// This adds a "Directory" TreeItem for the Widgets folder
  {
    QTreeWidgetItem* fpw_Name = new PMGeneratorTreeItem(fpw_gui);
    fpw_Name->setText(0, "Widgets");

    pathTemplate = "@PluginName@/Gui/Widgets/";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_Ui->m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             fpw_Name,
                                             this);
    gen->setDisplaySuffix("Widgets");
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);

    /// Add TreeItem for the FilterParameterWidgets/SourceList.cmake file
    PMGeneratorTreeItem* fpw_SourceList = new PMGeneratorTreeItem(fpw_Name);
    fpw_SourceList->setText(0, "SourceList.cmake");
    pathTemplate = "@PluginName@/Gui/Widgets/";
    resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Gui/Widgets/SourceList.cmake.in");
    PMFileGenerator* fpwFilterGen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                        pathTemplate,
                                                        QString("SourceList.cmake"),
                                                        resourceTemplate,
                                                        fpw_SourceList,
                                                        this);
    fpw_SourceList->setFileGenPtr(fpwFilterGen);
    fpwFilterGen->setDisplaySuffix("");
    fpwFilterGen->setDoesGenerateOutput(true);
    fpwFilterGen->setNameChangeable(false);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            fpwFilterGen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            fpwFilterGen, SLOT(outputDirChanged(QString)));
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            fpwFilterGen, SLOT(generateOutput()));
    connect(fpwFilterGen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));

  }


  //// TreeItems for the Plugin Documentation
  F_namefilters = new PMGeneratorTreeItem(F_doc);
  F_namefilters->setText(0, "Unknown Plugin Name");
  {
    pathTemplate = "@PluginName@/Documentation/@PluginName@Filters";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_Ui->m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             F_namefilters,
                                             this);
    gen->setDisplaySuffix("Filters");
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            gen, SLOT(pluginNameChanged(QString)));
  }

  //// TreeItem for the actual filter Documentation
  PMGeneratorTreeItem* filterHTML = new PMGeneratorTreeItem(F_namefilters);
  filterHTML->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@/Documentation/@PluginName@Filters/";
  resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Documentation/Filter/Documentation.md.in");
  PMFileGenerator* htmlFilterDoc = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                       pathTemplate,
                                                       QString(""),
                                                       resourceTemplate,
                                                       filterHTML,
                                                       this);
  filterHTML->setFileGenPtr(htmlFilterDoc);
  htmlFilterDoc->setDisplaySuffix("Filter.md");
  htmlFilterDoc->setDoesGenerateOutput(true);
  htmlFilterDoc->setNameChangeable(true);
  connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
          htmlFilterDoc, SLOT(pluginNameChanged(QString)));
  connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
          htmlFilterDoc, SLOT(outputDirChanged(QString)));
  // For "Directories" this probably isn't needed
  connect(m_Ui->generateButton, SIGNAL(clicked()),
          htmlFilterDoc, SLOT(generateOutput()));
  connect(htmlFilterDoc, SIGNAL(outputError(QString)),
          this, SLOT(generationError(QString)));

  FilterBundler fb2(cppFilterGen, hFilterGen, htmlFilterDoc, testgen, true);
  m_FilterBundles.push_back(fb2);

  m_Ui->m_PluginName->setText("Unknown Plugin Name");
  m_Ui->m_PluginName->selectAll();
  m_Ui->treeWidget->expandAll();
  emit updateStatusBar("Ready");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_selectButton_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Directory"), outputFile);
  if (!outputFile.isNull())
  {
    m_Ui->m_OutputDir->setText(QDir::toNativeSeparators(outputFile));
    m_OpenDialogLastDirectory = outputFile;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_generateButton_clicked()
{
  QString pluginName = m_Ui->m_PluginName->text();
  QString pluginDir = m_Ui->m_OutputDir->text();

  // Delete the old plugin folder, if it is there
  QString pluginPath = pluginDir + "/" + pluginName;
  pluginPath = QDir::toNativeSeparators(pluginPath);
  QFileInfo fi(pluginPath);
  if (fi.exists())
  {
    QDir dir(pluginPath);
    dir.removeRecursively();
  }

  pluginName = cleanName(pluginName);

  if (pluginName == "")
  {
    emit updateStatusBar("Generation Failed --- Please provide a plugin name");
    QMessageBox::critical(this, tr("PluginMaker"), tr("The file generation was unsuccessful.\n"
                                                      "Please enter a Plugin Name."));
    return;
  }
  if(pluginDir == "")
  {
    emit updateStatusBar("Generation Failed --- Please provide a plugin directory");
    QMessageBox::critical(this, tr("PluginMaker"), tr("The file generation was unsuccessful.\n"
                                                      "Please enter a Plugin Directory."));
    return;
  }

  // We need to generate the SourceList.cmake file here because we possibly have
  // more than a single filter
  QString text = generateCmakeContents();

  QString pathTemplate = "@PluginName@/@PluginName@Filters/";
  QString parentPath = m_Ui->m_OutputDir->text() + QDir::separator() + pathTemplate.replace("@PluginName@", pluginName);
  parentPath = QDir::toNativeSeparators(parentPath);

  QDir dir(parentPath);
  dir.mkpath(parentPath);

  parentPath = parentPath + QDir::separator() + "SourceList.cmake";
  //Write to file
  {
    QFile f(parentPath);
    if(f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      QTextStream out(&f);
      out << text;
    }
  }

  pathTemplate = "@PluginName@/Documentation/";
  parentPath = m_Ui->m_OutputDir->text() + QDir::separator() + pathTemplate.replace("@PluginName@", pluginName);
  parentPath = QDir::toNativeSeparators(parentPath);

  QDir dir2(parentPath);
  dir2.mkpath(parentPath);

  // Data Folder
  pathTemplate = "@PluginName@/Data/@PluginName@";
  parentPath = m_Ui->m_OutputDir->text() + QDir::separator() + pathTemplate.replace("@PluginName@", pluginName);
  parentPath = QDir::toNativeSeparators(parentPath);
  dir2 = QDir(parentPath);
  dir2.mkpath(parentPath);
  // Write to file
  parentPath = parentPath + QDir::separator() + "ReadMe.txt";
  {
    QFile f(parentPath);
    if(f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      QTextStream out(&f);
      out << "Place Example Data in this folder. It will be copied to the Data directory of the DREAM.3D distribution.";
    }
  }

  // ExamplePipelines Folder
  pathTemplate = "@PluginName@/ExamplePipelines/@PluginName@";
  parentPath = m_Ui->m_OutputDir->text() + QDir::separator() + pathTemplate.replace("@PluginName@", pluginName);
  parentPath = QDir::toNativeSeparators(parentPath);
  dir2 = QDir(parentPath);
  dir2.mkpath(parentPath);
  // Write to file
  parentPath = parentPath + QDir::separator() + "ReadMe.txt";
  {
    QFile f(parentPath);
    if(f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      QTextStream out(&f);
      out << "Place Example Pipelines in this folder. It will be copied to the Example Pipelines directory of the DREAM.3D distribution.";
    }
  }

  emit updateStatusBar("Generation Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_m_PluginName_textChanged(const QString& text)
{
  validityCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_m_OutputDir_textChanged(const QString& text)
{
  validityCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::generationError(const QString& test)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_addFilterBtn_clicked()
{
  AddFilterWidget* addFilterDialog = new AddFilterWidget(this);
  addFilterDialog->exec();

  QString filterTitle = addFilterDialog->getFilterName();

  if ( addFilterDialog->getBtnClicked() )
  {
    filterTitle = cleanName_filters(filterTitle);

    /* This simulates the user clicking on the "Add Filter" button */
    PMGeneratorTreeItem* filt2cpp = new PMGeneratorTreeItem(F_name);
    filt2cpp->setText(0, filterTitle + ".cpp");
    QString pathTemplate = "@PluginName@/@PluginName@Filters/";
    QString resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.cpp.in");
    PMFileGenerator* cppgen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                  pathTemplate,
                                                  QString(filterTitle + ".cpp"),
                                                  resourceTemplate,
                                                  filt2cpp,
                                                  this);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            cppgen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            cppgen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            cppgen, SLOT(generateOutput()));
    connect(cppgen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
    cppgen->setDoesGenerateOutput(true);
    cppgen->setNameChangeable(false);
    QString tempPluginName = cppgen->cleanName(m_Ui->m_PluginName->text());
    cppgen->setPluginName(tempPluginName);
    cppgen->setInitListContents("");
    filt2cpp->setFileGenPtr(cppgen);

    //m_itemMap[filt2cpp] = cppgen;


    /* This simulates the user clicking on the "Add Filter" button */
    PMGeneratorTreeItem* filt2h = new PMGeneratorTreeItem(F_name);
    filt2h->setText(0, filterTitle + ".h");
    pathTemplate = "@PluginName@/@PluginName@Filters/";
    resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.h.in");
    PMFileGenerator* hgen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                pathTemplate,
                                                QString(filterTitle + ".h"),
                                                resourceTemplate,
                                                filt2h,
                                                this);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            hgen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            hgen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            hgen, SLOT(generateOutput()));
    connect(hgen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
    hgen->setDoesGenerateOutput(true);
    hgen->setNameChangeable(false);
    tempPluginName = hgen->cleanName(m_Ui->m_PluginName->text());
    hgen->setPluginName(tempPluginName);
    filt2h->setFileGenPtr(hgen);

    //m_itemMap[filt2h] = hgen;


    /* This simulates the user clicking on the "Add Filter" button */
    PMGeneratorTreeItem* filt2html = new PMGeneratorTreeItem(F_namefilters);
    filt2html->setText(0, filterTitle + ".md");
    pathTemplate = "@PluginName@/Documentation/@PluginName@Filters/";
    resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Documentation/Filter/Documentation.md.in");
    PMFileGenerator* htmlgen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                   pathTemplate,
                                                   QString(filterTitle + ".md"),
                                                   resourceTemplate,
                                                   filt2html,
                                                   this);
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            htmlgen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            htmlgen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            htmlgen, SLOT(generateOutput()));
    connect(htmlgen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));
    htmlgen->setDoesGenerateOutput(true);
    htmlgen->setNameChangeable(false);
    tempPluginName = htmlgen->cleanName(m_Ui->m_PluginName->text());
    htmlgen->setPluginName(tempPluginName);
    filt2html->setFileGenPtr(htmlgen);
    //m_itemMap[filt2html] = htmlgen;


    PMGeneratorTreeItem* filt2test = new PMGeneratorTreeItem(F_test);
    filt2test->setText(0, tr("Unknown Plugin Name"));
    pathTemplate = "@PluginName@/Test";
    resourceTemplate = QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Test/FilterTest.cpp.in");
    PMFileGenerator* testgen = new PMFileGenerator(m_Ui->m_OutputDir->text(),
                                                   pathTemplate,
                                                   QString(filterTitle + "Test.cpp"),
                                                   resourceTemplate,
                                                   filt2test,
                                                   this);
    filt2test->setFileGenPtr(testgen);
    testgen->setDoesGenerateOutput(true);
    testgen->setNameChangeable(true);
    testgen->setDisplaySuffix("FilterTest.cpp");
    testgen->setFilterName(filterTitle);

    tempPluginName = testgen->cleanName(m_Ui->m_PluginName->text());
    testgen->setPluginName(tempPluginName);

    m_TestFileLocationNames.insert(filterTitle);    // This name needs to be used in the TestFileLocations.h.in file
    connect(m_Ui->m_PluginName, SIGNAL(textChanged(QString)),
            testgen, SLOT(pluginNameChanged(QString)));
    connect(m_Ui->m_OutputDir, SIGNAL(textChanged(QString)),
            testgen, SLOT(outputDirChanged(QString)));
    // For "Directories" this probably isn't needed
    connect(m_Ui->generateButton, SIGNAL(clicked()),
            testgen, SLOT(generateOutput()));
    connect(testgen, SIGNAL(outputError(QString)),
            this, SLOT(generationError(QString)));

    //htmlgen->setNameChangeable(false);
    filt2test->setFileGenPtr(testgen);


    FilterBundler filterpack(cppgen, hgen, htmlgen, testgen, addFilterDialog->isPublic());
    m_FilterBundles.push_back(filterpack);
  }
  //  for(int i = 0;i < m_FilterBundles.count(); ++i)
  //  {
  //    std::cout  << "CPP: " << m_FilterBundles[i].getCPPGenerator()->getTreeWidgetItem()->text(0)() << "\n";
  //  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_removeFilterBtn_clicked()
{
  QTreeWidgetItem* ptr(m_Ui->treeWidget->currentItem());
  if (ptr == nullptr)
  {
    return;
  }
  QString filePath = ptr->text(0);
  QFileInfo fi(filePath);
  QString namecheck = fi.baseName();

  for (int i = 0; i < m_FilterBundles.size(); i++)
  {
    if (m_FilterBundles[i].containsTreeWidgetItem(ptr))
    {
      // Remove the entry from the TestFileLocations.h.in cache
      QString pluginName = m_FilterBundles[i].getTestGenerator()->getPluginName();
      if (namecheck == pluginName + "Filter")
      {
        namecheck = "@PluginName@Filter";
      }

      m_TestFileLocationNames.remove(namecheck);

      //Remove the TreeWidgetItems from the tree widget
      m_Ui->treeWidget->removeItemWidget(m_FilterBundles[i].getCPPGenerator()->getTreeWidgetItem(), 0);
      m_Ui->treeWidget->removeItemWidget(m_FilterBundles[i].getHGenerator()->getTreeWidgetItem(), 0);
      m_Ui->treeWidget->removeItemWidget(m_FilterBundles[i].getHTMLGenerator()->getTreeWidgetItem(), 0);
      m_Ui->treeWidget->removeItemWidget(m_FilterBundles[i].getTestGenerator()->getTreeWidgetItem(), 0);

      //Delete the TreeWidgetItems
      delete ( m_FilterBundles[i].getCPPGenerator()->getTreeWidgetItem() );
      delete ( m_FilterBundles[i].getHGenerator()->getTreeWidgetItem() );
      delete ( m_FilterBundles[i].getHTMLGenerator()->getTreeWidgetItem() );
      delete (m_FilterBundles[i].getTestGenerator()->getTreeWidgetItem() );

      //Delete all 3 instances of PMFileGenerator
      delete ( m_FilterBundles[i].getCPPGenerator() );
      delete ( m_FilterBundles[i].getHGenerator() );
      delete ( m_FilterBundles[i].getHTMLGenerator() );
      delete (m_FilterBundles[i].getTestGenerator() );

      //Remove the instance of FilterBundler from the m_FilterBundles QVector
      m_FilterBundles.remove(i);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_treeWidget_itemSelectionChanged()
{
  QString pluginName = cleanName(m_Ui->m_PluginName->text());

  PMGeneratorTreeItem* currentFile = dynamic_cast<PMGeneratorTreeItem*>(m_Ui->treeWidget->currentItem());
  if (currentFile == nullptr)
  {
    return;
  }

  QString text = "";
  QTreeWidgetItem* parent = m_Ui->treeWidget->currentItem()->parent();
  if(nullptr != parent
      && parent->text(0).endsWith("Filters")
      && currentFile->text(0).compare("SourceList.cmake") == 0 )
  {
    text = generateCmakeContents();
  }
  else if ( nullptr != parent
            && parent->text(0) == "Test"
            && currentFile->text(0) == "TestFileLocations.h.in")
  {
    PMFileGenerator* fileGen = qobject_cast<PMFileGenerator*> (currentFile->getFileGenPtr());
    if (!fileGen)
    {
      return;
    }

    QString replaceStr = fileGen->createReplacementString(TESTFILELOCATIONS, m_TestFileLocationNames);
    text = fileGen->generateFileContents(replaceStr);
  }
  else if (nullptr != parent
           && parent->text(0) == "Test"
           && currentFile->text(0) == "CMakeLists.txt")
  {
    PMFileGenerator* fileGen = qobject_cast<PMFileGenerator*> (currentFile->getFileGenPtr());
    if (!fileGen)
    {
      return;
    }

    QString replaceStr = fileGen->createReplacementString(CMAKELISTS, m_TestFileLocationNames);
    text = fileGen->generateFileContents(replaceStr);
  }
  else
  {
    PMFileGenerator* fileGen = qobject_cast<PMFileGenerator*> ( currentFile->getFileGenPtr() );
    if (!fileGen)
    {
      return;
    }
    text = fileGen->generateFileContents();
  }

  m_Ui->m_fileEditor->setPlainText(text);

  emit updateStatusBar("Currently viewing " + currentFile->text(0));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 PluginMaker::checkDirtyDocument()
{
  qint32 err = -1;

  if (this->isWindowModified() == true)
  {
    int r = QMessageBox::warning(this, BrandedStrings::ApplicationName,
                                 tr("The Data has been modified.\nDo you want to save your changes?"),
                                 QMessageBox::Save | QMessageBox::Default,
                                 QMessageBox::Discard,
                                 QMessageBox::Cancel | QMessageBox::Escape);
    if (r == QMessageBox::Save)
    {
      //TODO: Save the current document or otherwise save the state.
    }
    else if (r == QMessageBox::Discard)
    {
      err = 1;
    }
    else if (r == QMessageBox::Cancel)
    {
      err = -1;
    }
  }
  else
  {
    err = 1;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginMaker::generateCmakeContents() const
{
  QString pluginName = m_Ui->m_PluginName->text();
  pluginName = cleanName(pluginName);

  QString cmakeHdrCode("${@PluginName@Plugin_SOURCE_DIR}/@PluginName@Filters/");
  cmakeHdrCode.replace("@PluginName@", pluginName);

  QString publicFilterList;
  QString privateFilterList;
  QString hdrContents;
  for (int i = 0; i < m_FilterBundles.count(); ++i)
  {
    // PMFileGenerator* cppGen = m_FilterBundles[i].getCPPGenerator();
    PMFileGenerator* hGen = m_FilterBundles[i].getHGenerator();
    bool isPublic = m_FilterBundles[i].isPublic();

    //qDebug() << cppGen->getFileName() << "\n";
    //qDebug() << hGen->getFileName() << "\n";

    //  hdrContents.append("    ").append(cmakeHdrCode).append(hGen->getFileName()).append("\n    ");
    //  srcContents.append("    ").append(cmakeHdrCode).append(cppGen->getFileName()).append("\n    ");

    QFileInfo fi(hGen->getFileName());
    QString className = fi.baseName();
    if (isPublic == true)
    {
      publicFilterList.append("  ").append(className).append("\n");
    }
    else
    {
      privateFilterList.append("  ").append(className).append("\n");
    }
    pluginName = m_Ui->m_PluginName->text();
    pluginName = cleanName(pluginName);
  }

  QString text = "";

  // Create SourceList File

  QFile rfile(QtSApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/SourceList.cmake.in"));
  if ( rfile.open(QIODevice::ReadOnly | QIODevice::Text) )
  {
    QTextStream in(&rfile);
    text = in.readAll();
    text.replace("@PluginName@", pluginName);
    text.replace("@GENERATED_CMAKE_HEADERS_CODE@", hdrContents);
    text.replace("@PUBLIC_FILTERS@", publicFilterList);
    text.replace("@PRIVATE_FILTERS@", privateFilterList);
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::testFileLocationsHandler()
{
  emit clicked(m_TestFileLocationNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PluginMaker::validityCheck()
{
  QString filePath = m_Ui->m_OutputDir->text();
  QFileInfo fi(filePath);
  if(m_Ui->m_PluginName->text().isEmpty() || m_Ui->m_OutputDir->text().isEmpty() || fi.exists() == false)
  {
    m_Ui->generateButton->setDisabled(true);
    if(m_Ui->m_PluginName->text().isEmpty())
    {
      m_Ui->errorString->setText("The plugin name has not been chosen.");
    }
    else if(m_Ui->m_OutputDir->text().isEmpty())
    {
      m_Ui->errorString->setText("The output directory has not been selected.");
    }
    else
    {
      m_Ui->errorString->setText("The currently selected output directory does not exist.");
    }

    return false;
  }
  else
  {
    m_Ui->generateButton->setEnabled(true);
    m_Ui->errorString->setText("");
    emit updateStatusBar("Ready");
    return true;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginMaker::getPluginName() const
{
  return m_Ui->m_PluginName->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::setPluginName(const QString& value)
{
  m_Ui->m_PluginName->setText(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginMaker::getOutputDir() const
{
  return m_Ui->m_OutputDir->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::setOutputDir(const QString& value)
{
  m_Ui->m_OutputDir->setText(value);
}
