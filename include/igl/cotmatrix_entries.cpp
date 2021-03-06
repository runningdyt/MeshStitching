// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "cotmatrix_entries.h"
#include "doublearea.h"
#include "edge_lengths.h"
#include "face_areas.h"
#include "volume.h"
#include "dihedral_angles.h"

#include "verbose.h"


template <typename DerivedV, typename DerivedF, typename DerivedC>
IGL_INLINE void igl::cotmatrix_entries(
  const Eigen::PlainObjectBase<DerivedV>& V,
  const Eigen::PlainObjectBase<DerivedF>& F,
  Eigen::PlainObjectBase<DerivedC>& C)
{
  using namespace std;
  using namespace Eigen;
  // simplex size (3: triangles, 4: tetrahedra)
  int simplex_size = F.cols();
  // Number of elements
  int m = F.rows();

  // Law of cosines + law of sines
  switch(simplex_size)
  {
    case 3:
    {
      // Triangles
      //Eigen::Matrix<typename DerivedC::Scalar,Dynamic,3> l;
      //edge_lengths(V,F,l);
      // edge lengths numbered same as opposite vertices
      Eigen::Matrix<typename DerivedC::Scalar,Dynamic,3> l;
      igl::edge_lengths(V,F,l);
      // double area
      Eigen::Matrix<typename DerivedC::Scalar,Dynamic,1> dblA;
      doublearea(l,dblA);
      // cotangents and diagonal entries for element matrices
      // correctly divided by 4 (alec 2010)
      C.resize(m,3);
      for(int i = 0;i<m;i++)
      {
        C(i,0) = (l(i,1)*l(i,1) + l(i,2)*l(i,2) - l(i,0)*l(i,0))/dblA(i)/4.0;
        C(i,1) = (l(i,2)*l(i,2) + l(i,0)*l(i,0) - l(i,1)*l(i,1))/dblA(i)/4.0;
        C(i,2) = (l(i,0)*l(i,0) + l(i,1)*l(i,1) - l(i,2)*l(i,2))/dblA(i)/4.0;
      }
      break;
    }
    case 4:
    {

      // edge lengths numbered same as opposite vertices
      Eigen::Matrix<typename DerivedC::Scalar,Dynamic,6> l;
      edge_lengths(V,F,l);
      Eigen::Matrix<typename DerivedC::Scalar,Dynamic,4> s;
      face_areas(l,s);
      Eigen::Matrix<typename DerivedC::Scalar,Dynamic,6> cos_theta,theta;
      dihedral_angles_intrinsic(l,s,theta,cos_theta);

      // volume
      Eigen::Matrix<typename DerivedC::Scalar,Dynamic,1> vol;
      volume(l,vol);


      // Law of sines
      // http://mathworld.wolfram.com/Tetrahedron.html
      Eigen::Matrix<typename DerivedC::Scalar,Dynamic,6> sin_theta(m,6);
      sin_theta.col(0) = vol.array() / ((2./(3.*l.col(0).array())).array() * s.col(1).array() * s.col(2).array());
      sin_theta.col(1) = vol.array() / ((2./(3.*l.col(1).array())).array() * s.col(2).array() * s.col(0).array());
      sin_theta.col(2) = vol.array() / ((2./(3.*l.col(2).array())).array() * s.col(0).array() * s.col(1).array());
      sin_theta.col(3) = vol.array() / ((2./(3.*l.col(3).array())).array() * s.col(3).array() * s.col(0).array());
      sin_theta.col(4) = vol.array() / ((2./(3.*l.col(4).array())).array() * s.col(3).array() * s.col(1).array());
      sin_theta.col(5) = vol.array() / ((2./(3.*l.col(5).array())).array() * s.col(3).array() * s.col(2).array());


      // http://arxiv.org/pdf/1208.0354.pdf Page 18
      C = (1./6.) * l.array() * cos_theta.array() / sin_theta.array();

      break;
    }
    default:
    {
      fprintf(stderr,
          "cotmatrix_entries.h: Error: Simplex size (%d) not supported\n", simplex_size);
      assert(false);
    }
  }
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template specialization
// generated by autoexplicit.sh
template void igl::cotmatrix_entries<Eigen::Matrix<double, -1, 3, 0, -1, 3>, Eigen::Matrix<int, -1, 4, 0, -1, 4>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 4, 0, -1, 4> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
// generated by autoexplicit.sh
template void igl::cotmatrix_entries<Eigen::Matrix<double, -1, 3, 0, -1, 3>, Eigen::Matrix<int, -1, 3, 0, -1, 3>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 0, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::cotmatrix_entries<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
#endif
