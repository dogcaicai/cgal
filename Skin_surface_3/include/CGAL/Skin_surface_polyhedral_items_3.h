// Copyright (c) 2005 Rijksuniversiteit Groningen (Netherlands)
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Nico Kruithof <Nico@cs.rug.nl>

#ifndef CGAL_SKIN_SURFACE_POLYHEDRAL_ITEMS_3_H
#define CGAL_SKIN_SURFACE_POLYHEDRAL_ITEMS_3_H

#include <CGAL/HalfedgeDS_face_base.h>
#include <CGAL/Polyhedron_items_3.h>

CGAL_BEGIN_NAMESPACE

template <class Refs, class SkinSurface3>
struct Skin_Surface_polyhedral_face : public CGAL::HalfedgeDS_face_base<Refs> {
  typedef SkinSurface3                                      Skin_surface;
//   typedef typename Skin_surface::Triangulated_mixed_complex TMC;
//   typedef typename TMC::Cell_handle                         Triang_Cell_handle;

//   Triang_Cell_handle triang_ch;
};

template < class SkinSurface3 >
struct Skin_surface_polyhedral_items_3 : public Polyhedron_items_3 {
  
  template <class Refs, class Traits>
  struct Face_wrapper {
    typedef Skin_Surface_polyhedral_face<Refs, SkinSurface3> Face;
  };
};

CGAL_END_NAMESPACE

#endif // CGAL_SKIN_SURFACE_POLYHEDRAL_ITEMS_3_H
