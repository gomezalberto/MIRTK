/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2013-2015 Imperial College London
 * Copyright 2013-2015 Andreas Schuh
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MIRTK_LieBracketImageFilter2D_H
#define MIRTK_LieBracketImageFilter2D_H

#include <mirtkLieBracketImageFilter.h>

#include <mirtkMatrix.h>
#include <mirtkGenericImage.h>
#include <mirtkParallel.h>
#include <mirtkProfiling.h>


namespace mirtk {


/**
 * Image filter for computation of Lie bracket of two 2D vector fields.
 */
template <class VoxelType>
class LieBracketImageFilter2D : public LieBracketImageFilter<VoxelType>
{
  mirtkImageFilterMacro(LieBracketImageFilter2D);

  /// World to image matrix (excluding translation)
  mirtkAttributeMacro(Matrix, MatW2I);

protected:

  /// Compute 1st order derivatives of given vector field
  void Jacobian(Matrix &, const GenericImage<VoxelType> &, int, int);

  /// Initialize filter
  virtual void Initialize();

public:

  /// Constructor
  LieBracketImageFilter2D();

  /// Destructor
  virtual ~LieBracketImageFilter2D();

  // Import other overloads
  using LieBracketImageFilter<VoxelType>::Output;

  /// Set output
  virtual void Output(GenericImage<VoxelType> *);

  /// Run filter on every voxel
  virtual void Run();

  /// Run filter on single voxel
  virtual void Run(double [2], int, int);

  /// Run filter on single voxel and component
  virtual double Run(int, int, int, int);

};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions
///////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
template <class VoxelType>
LieBracketImageFilter2D<VoxelType>::LieBracketImageFilter2D()
:
  LieBracketImageFilter<VoxelType>(),
  _MatW2I(2, 2)
{
  _MatW2I.Ident();
}

// --------------------------------------------------------------------------
template <class VoxelType>
LieBracketImageFilter2D<VoxelType>::~LieBracketImageFilter2D()
{
}

// --------------------------------------------------------------------------
template <class VoxelType>
void LieBracketImageFilter2D<VoxelType>::Output(GenericImage<VoxelType> *output)
{
  LieBracketImageFilter<VoxelType>::Output(output);
  _MatW2I = output->GetWorldToImageMatrix()(0, 0, 2, 2);
}

// --------------------------------------------------------------------------
template <class VoxelType>
void LieBracketImageFilter2D<VoxelType>::Initialize()
{
  // Initialize base class
  LieBracketImageFilter<VoxelType>::Initialize();

  // Ensure that input vector fields are 2D
  // Note that base class already ensures that inputs and output have same attributes
  if (this->GetInput(0)->Z() > 1 || this->GetInput(0)->T() != 2) {
    cerr << this->NameOfClass() << "::Initialize: Input images are no 2D vector fields" << endl;
    exit(1);
  }
}

// ---------------------------------------------------------------------------
// Note: Using NN extrapolation at boundary
template <class VoxelType>
void
LieBracketImageFilter2D<VoxelType>
::Jacobian(Matrix &jac, const GenericImage<VoxelType> &v, int i, int j)
{
  int a, b;
  // Finite difference in x dimension
  if (i <= 0) {
    a = i;
    b = i + 1;
  } else if (i >= v.X() - 1) {
    a = i - 1;
    b = i;
  } else {
    a = i - 1;
    b = i + 1;
  }
  jac(0, 0) = 0.5 * (v(b, j, 0, 0) - v(a, j, 0, 0));
  jac(1, 0) = 0.5 * (v(b, j, 0, 1) - v(a, j, 0, 1));
  // Finite difference in y dimension
  if (j <= 0) {
    a = j;
    b = j + 1;
  } else if (j >= v.Y() - 1) {
    a = j - 1;
    b = j;
  } else {
    a = j - 1;
    b = j + 1;
  }
  jac(0, 1) = 0.5 * (v(i, b, 0, 0) - v(i, a, 0, 0));
  jac(1, 1) = 0.5 * (v(i, b, 0, 1) - v(i, a, 0, 1));
  // Project derivative from image to world space
  jac *= _MatW2I;
}

// ---------------------------------------------------------------------------
template <class VoxelType>
double LieBracketImageFilter2D<VoxelType>::Run(int i, int j, int, int t)
{
  Matrix lJ(2, 2), rJ(2, 2);

  const GenericImage<VoxelType> &lv = *this->GetInput(0);
  const GenericImage<VoxelType> &rv = *this->GetInput(1);

  const VoxelType &lx = lv(i, j, 0, 0);
  const VoxelType &ly = lv(i, j, 0, 1);
  const VoxelType &rx = rv(i, j, 0, 0);
  const VoxelType &ry = rv(i, j, 0, 1);

  Jacobian(lJ, lv, i, j);
  Jacobian(rJ, rv, i, j);

  return (lJ(t, 0) * rx - lx * rJ(t, 0)) + (lJ(t, 1) * ry - ly * rJ(t, 1));
}

// ---------------------------------------------------------------------------
template <class VoxelType>
void LieBracketImageFilter2D<VoxelType>::Run(double vec[2], int i, int j)
{
  Matrix lJ(2, 2), rJ(2, 2);

  const GenericImage<VoxelType> &lv = *this->GetInput(0);
  const GenericImage<VoxelType> &rv = *this->GetInput(1);

  const VoxelType &lx = lv(i, j, 0, 0);
  const VoxelType &ly = lv(i, j, 0, 1);
  const VoxelType &rx = rv(i, j, 0, 0);
  const VoxelType &ry = rv(i, j, 0, 1);

  Jacobian(lJ, lv, i, j);
  Jacobian(rJ, rv, i, j);

  vec[0] = (lJ(0, 0) * rx - lx * rJ(0, 0)) + (lJ(0, 1) * ry - ly * rJ(0, 1));
  vec[1] = (lJ(1, 0) * rx - lx * rJ(1, 0)) + (lJ(1, 1) * ry - ly * rJ(1, 1));
}

// ---------------------------------------------------------------------------
/// Parallelizable body of LieBracketImageFilter2D::Run.
template <class VoxelType>
class LieBracketImageFilter2DRun
{
private:
  LieBracketImageFilter2D<VoxelType> *_LieBracketFilter; ///< Lie bracket filter
  GenericImage<VoxelType>            *_Output;           ///< Output vector field

public:

  /// Default constructor
  LieBracketImageFilter2DRun(LieBracketImageFilter2D<VoxelType> *filter,
                             GenericImage<VoxelType>            *output)
  :
    _LieBracketFilter(filter),
    _Output(output)
  {
  }

  /// Copy constructor
  LieBracketImageFilter2DRun(const LieBracketImageFilter2DRun<VoxelType> &other)
  :
    _LieBracketFilter(other._LieBracketFilter),
    _Output          (other._Output)
  {
  }

  /// Run Lie bracket filter for each voxel in specified range
  void operator ()(const blocked_range2d<int> &r) const
  {
    double vec[2];
    for (int j = r.rows().begin(); j != r.rows().end(); j++) {
      for (int i = r.cols().begin(); i != r.cols().end(); i++) {
        _LieBracketFilter->Run(vec, i, j);
        _Output->Put(i, j, 0, 0, vec[0]);
        _Output->Put(i, j, 0, 1, vec[1]);
      }
    }
  }
};

// ---------------------------------------------------------------------------
template <class VoxelType>
void LieBracketImageFilter2D<VoxelType>::Run()
{
  blocked_range2d<int> voxels(0, this->GetInput(0)->Y(), 1,
                              0, this->GetInput(0)->X(), 1);
  LieBracketImageFilter2DRun<VoxelType> body(this, this->Output());
  MIRTK_START_TIMING();
  this->Initialize();
  parallel_for(voxels, body);
  this->Finalize();
  MIRTK_DEBUG_TIMING(2, "LieBracketImageFilter");
}


} // namespace mirtk

#endif // MIRTK_LieBracketImageFilter2D_H
