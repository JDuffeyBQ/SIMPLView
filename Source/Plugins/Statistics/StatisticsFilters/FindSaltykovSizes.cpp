/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
 * All rights reserved.
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindSaltykovSizes.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "Statistics/StatisticsConstants.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSaltykovSizes::FindSaltykovSizes() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_EquivalentDiametersArrayPath(DREAM3D::FeatureData::EquivalentDiameters),
  m_SaltykovEquivalentDiametersArrayName(DREAM3D::FeatureData::SaltykovEquivalentDiameters),
  m_EquivalentDiameters(NULL),
  m_SaltykovEquivalentDiameters(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSaltykovSizes::~FindSaltykovSizes()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Equivalent Diameters", "EquivalentDiametersArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getEquivalentDiametersArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Saltykov Equivalent Diameters", "SaltykovEquivalentDiametersArrayName", FilterParameterWidgetType::StringWidget, getSaltykovEquivalentDiametersArrayName(), true, ""));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void FindSaltykovSizes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setEquivalentDiametersArrayPath(reader->readDataArrayPath("EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath() ) );
  setSaltykovEquivalentDiametersArrayName(reader->readString("SaltykovEquivalentDiametersArrayName", getSaltykovEquivalentDiametersArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSaltykovSizes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(EquivalentDiametersArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SaltykovEquivalentDiametersArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_EquivalentDiametersPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getEquivalentDiametersArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getSaltykovEquivalentDiametersArrayName() );
  m_SaltykovEquivalentDiametersPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SaltykovEquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SaltykovEquivalentDiameters = m_SaltykovEquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  find_saltykov_sizes();

  notifyStatusMessage(getHumanLabel(), "Find Feature Saltykov Sizes Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::find_saltykov_sizes()
{
  DREAM3D_RANDOMNG_NEW()

  const int MaxAttempts = 10;
  const unsigned long RandMaxFloat = 0xffffffff;
  // an initial guess of 10 bins is used
  int numberofbins = 10;

  float minEqDia = std::numeric_limits<float>::max();
  float maxEqDia = 0.0f;
  float binLength = 0.0f;
  float temp1 = 0.0f, temp2 = 0.0f, temp3 = 0.0f;
  int binIndex = 0;
  int saltykovLength = 0;
  int saltykovIndex = 1;
  double random = 0.0;
  int attempts = 0;
  int binToAddTo = 0;
  unsigned long randomLong = 0.0;
  double randomFraction = 0.0;
  int difference = 0;

  size_t numfeatures = m_EquivalentDiametersPtr.lock()->getNumberOfTuples();

  std::vector<int> binLengths;
  binLengths.resize(numberofbins);

  std::vector<int> saltykovBinLengths;
  saltykovBinLengths.resize(numberofbins);

  std::vector<float> saltykovEquivalentDiameters;
  saltykovEquivalentDiameters.resize(numfeatures);

  for (size_t i = 1; i < numfeatures; i++)
  {
    // find the min size
    if (m_EquivalentDiameters[i] < minEqDia) { minEqDia = m_EquivalentDiameters[i]; }
    // find the max size
    if (m_EquivalentDiameters[i] > maxEqDia) { maxEqDia = m_EquivalentDiameters[i]; }
  }

  float currentmiminum = minEqDia;
  float nextminimum = maxEqDia;

  // continue performing Saltkov with changing the number of bins
  // until the number of feature eq dia's to be sampled = the
  // number of features.  We do this so the SaltykovArray length jives
  // with the feature-level level array and then we get to be able to
  // compare each Saltkov eq dia one-for-one against the feature eq dia's
  // so, with that, each Saltykov eq dia is matched in ascending order to
  // the feature eq dia.  But, it is important to note that the Saltykov eq dia
  // is not a direct transformation of the particular eq dia that it is matched
  // up with
  while (saltykovLength != numfeatures-1)
  {
    // find the bin length
    binLength = (maxEqDia - minEqDia) / numberofbins;

	// initialize bin lengths to 0
    for (int i = 0; i < numberofbins; i++) { binLengths[i] = 0; }

	// find out how many features are in each bin
    for (size_t i = 1; i < numfeatures; i++)
    {
      temp1 = m_EquivalentDiameters[i] - minEqDia;
      temp2 = 1.0f / binLength;
      temp3 = temp1 * temp2;
      binIndex = int(temp3);
	  // this accounts for if the current feature is
	  // the maximum eq dia feature so its doesn't
	  // run off the end of the array
      if (binIndex == numberofbins) { binIndex--; };
      binLengths[binIndex]++;
    }

	// for formal Saltykov we start at the largest bin then unfold our
	// way down to the smallest bin which is why the loop is set up
	// this way
	// saltykovLength is the total number of features to be sampled from
	// the saltykov bins
	saltykovLength = 0;
	for (int i = numberofbins-1; i >= 0; i--)
    {
      saltykovBinLengths[i] = do_saltykov(binLengths, maxEqDia, i);
	  if (saltykovBinLengths[i] < 0) { saltykovBinLengths[i] = 0; }
      saltykovLength += saltykovBinLengths[i];
    }

	// if the number of features to be sampled from the Saltykov
	// bins (saltykovLength) is not = the number of features
	// then the difference between the two is calculated.  then, either 
	// the number of attempts is incremented, the number of bins is increased/decreased
	// based on the "difference" and the bin arrays are resized accordingly.  This 
	// will approach us to the correct number of features because the increasing
	// number of bins, increases the number of features eq dia's to be sampled and decreasing
	// the number of bins does the opposite. However, if the number of maximum attempts 
	// is reached, hardcoded at 10, then a random Saltykov bin is selected to add one sample to.
	// This is done because it is assumed that after 10 attempts, the solution is oscillating
	// between one less than and one more than feature so, for convienence, only the one less
	// than option triggers this fudge addition to reach our desired number of features
    if (saltykovLength != numfeatures-1)
    {
      difference = saltykovLength - (numfeatures-1);
	  if (attempts >= MaxAttempts && difference < 0)
      {
        for (int i = 0; i > difference; i--)
        {
          binToAddTo = rg.genrand_int32() % numberofbins + 1;
		  if (binToAddTo == numberofbins) { binToAddTo--; };
          saltykovBinLengths[binToAddTo]++;
		  saltykovLength = numfeatures-1;
        }
      }
      else
      {
        attempts++;
        numberofbins -= difference;
        binLengths.resize(numberofbins);
        saltykovBinLengths.resize(numberofbins);
      }
    }
  }

  // only proceed with sampling the Saltkov bins if the Salytkov samples
  // = the number of features, else let the loop cycle again with the
  // new number of bins
  saltykovIndex = 0;
  if (saltykovLength == numfeatures-1)
  {
	for (int i = 0; i < numberofbins; i++)
	{
	  for (int j = 0; j < saltykovBinLengths[i]; j++)
	  {
		// generate a random float between the current bin extents
		randomLong = rg.genrand_int32();
		randomFraction = (double(randomLong) / double(RandMaxFloat));
		saltykovEquivalentDiameters[saltykovIndex] = (float(randomFraction) * binLength) + (minEqDia + (i * binLength));
		saltykovIndex++;
	  }
	}

	// sort the Saltykov eq dia's into ascending order so they can be matched up with their feature eq dia pair
	std::sort(saltykovEquivalentDiameters.begin(), saltykovEquivalentDiameters.end(), std::less<float>());

	// this nested loop matches the Saltykov eq dia's with the feature eq dia's in asecending order
	for (size_t i = 1; i < numfeatures; i++)
	{
	  for (size_t j = 1; j < numfeatures; j++)
	  {
		if (m_EquivalentDiameters[j] == currentmiminum)
		{
		  m_SaltykovEquivalentDiameters[j] = saltykovEquivalentDiameters[i];
		}
		if (m_EquivalentDiameters[j] > currentmiminum && m_EquivalentDiameters[j] < nextminimum)
		{
		  nextminimum = m_EquivalentDiameters[j];
		}
	  }
	  currentmiminum = nextminimum;
	  nextminimum = maxEqDia;
	}
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSaltykovSizes::do_saltykov(std::vector<int> nA, float Dmax, int k)
{
  // the below is formal Saltykov method of spheres with the formal variable names and 12 Saltykov coefs.  It was decided
  // that any further contributions from the coefs is negligible
  double saltyCoefs[12] = {1.6461,-0.4561,-0.1162,-0.0415,-0.0173,-0.0079,-0.0038,-0.0018,-0.0010,-0.0003,-0.0002,-0.0002};
  double Dk = Dmax / k;
  int i = 0;
  double temp1 = 0.0, temp2 = 0.0, temp3 = 0.0;
  int temp4 = 0;

  while ((i < 12) && ((k+1-i) > 0))
  {
    temp1 = double (nA[k-i]);
    temp2 += saltyCoefs[i] * temp1;
    i++;
  }
  temp2 /= Dk;
  temp3 = ceil(temp2);
  temp4 = int(temp3);
  return temp4;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSaltykovSizes::newFilterInstance(bool copyFilterParameters)
{
  FindSaltykovSizes::Pointer filter = FindSaltykovSizes::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getCompiledLibraryName()
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MorphologicalFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getHumanLabel()
{ return "Find Feature Saltykov Sizes"; }
