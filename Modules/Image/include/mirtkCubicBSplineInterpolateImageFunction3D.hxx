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

#ifndef MIRTK_CubicBSplineInterpolateImageFunction3D_HXX
#define MIRTK_CubicBSplineInterpolateImageFunction3D_HXX

#include <mirtkCubicBSplineInterpolateImageFunction3D.h>
#include <mirtkCubicBSplineInterpolateImageFunction.hxx>


namespace mirtk {


// -----------------------------------------------------------------------------
template <class TImage>
GenericCubicBSplineInterpolateImageFunction3D<TImage>
::GenericCubicBSplineInterpolateImageFunction3D()
{
  this->NumberOfDimensions(3);
}

// -----------------------------------------------------------------------------
template <class TImage>
inline typename GenericCubicBSplineInterpolateImageFunction3D<TImage>::VoxelType
GenericCubicBSplineInterpolateImageFunction3D<TImage>
::Get(double x, double y, double z, double t) const
{
  return this->Get3D(x, y, z, t);
}

// -----------------------------------------------------------------------------
template <class TImage>
inline typename GenericCubicBSplineInterpolateImageFunction3D<TImage>::VoxelType
GenericCubicBSplineInterpolateImageFunction3D<TImage>
::GetWithPadding(double x, double y, double z, double t) const
{
  return this->GetWithPadding3D(x, y, z, t);
}

// -----------------------------------------------------------------------------
template <class TImage> template <class TOtherImage>
inline typename TOtherImage::VoxelType
GenericCubicBSplineInterpolateImageFunction3D<TImage>
::Get(const TOtherImage *coeff, double x, double y, double z, double t) const
{
  return this->Get3D(coeff, x, y, z, t);
}

// -----------------------------------------------------------------------------
template <class TImage> template <class TOtherImage, class TCoefficient>
inline typename TCoefficient::VoxelType
GenericCubicBSplineInterpolateImageFunction3D<TImage>
::GetWithPadding(const TOtherImage *input, const TCoefficient *coeff,
                double x, double y, double z, double t) const
{
  return this->GetWithPadding3D(input, coeff, x, y, z, t);
}

// -----------------------------------------------------------------------------
template <class TImage>
inline typename GenericCubicBSplineInterpolateImageFunction3D<TImage>::VoxelType
GenericCubicBSplineInterpolateImageFunction3D<TImage>
::GetInside(double x, double y, double z, double t) const
{
  // Use faster coefficient iteration than Get3D(Coefficient(), x, y, z, t)
  return this->GetInside3D(x, y, z, t);
}

// -----------------------------------------------------------------------------
template <class TImage>
inline typename GenericCubicBSplineInterpolateImageFunction3D<TImage>::VoxelType
GenericCubicBSplineInterpolateImageFunction3D<TImage>
::GetOutside(double x, double y, double z, double t) const
{
  if (this->_InfiniteCoefficient) {
    return Get(this->_InfiniteCoefficient, x, y, z, t);
  } else {
    return Get(x, y, z, t);
  }
}

// -----------------------------------------------------------------------------
template <class TImage>
inline typename GenericCubicBSplineInterpolateImageFunction3D<TImage>::VoxelType
GenericCubicBSplineInterpolateImageFunction3D<TImage>
::GetWithPaddingInside(double x, double y, double z, double t) const
{
  return GetWithPadding(this->Input(), &this->_Coefficient, x, y, z, t);
}

// -----------------------------------------------------------------------------
template <class TImage>
inline typename GenericCubicBSplineInterpolateImageFunction3D<TImage>::VoxelType
GenericCubicBSplineInterpolateImageFunction3D<TImage>
::GetWithPaddingOutside(double x, double y, double z, double t) const
{
  if (this->Extrapolator() && this->_InfiniteCoefficient) {
    return GetWithPadding(this->Extrapolator(), this->_InfiniteCoefficient, x, y, z, t);
  } else {
    return GetWithPadding(x, y, z, t);
  }
}


} // namespace mirtk

#endif // MIRTK_CubicBSplineInterpolateImageFunction3D_HXX
