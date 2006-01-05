// Copyright (c) 2005  Tel-Aviv University (Israel).
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
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Baruch Zukerman <baruchzu@post.tau.ac.il>

#ifndef BSO_UTILS
#define BSO_UTILS

#include <CGAL/Unique_hash_map.h>
#include <CGAL/iterator.h>
#include <CGAL/function_objects.h>
#include <CGAL/circulator.h> 
#include <queue>


template <class Arrangement>
class Curve_creator : 
  public CGAL::Creator_1<typename Arrangement::Halfedge,
                   typename Arrangement::X_monotone_curve_2>
{
public:

  typedef typename Arrangement::Halfedge              Halfedge;
  typedef typename Arrangement::X_monotone_curve_2    X_monotone_curve_2;
  typedef typename Arrangement::Traits_2              Traits_2;   
  typedef CGAL::Creator_1<Halfedge, X_monotone_curve_2>     Base;

protected:
  Traits_2*    m_traits;



public:
  Curve_creator(Traits_2* tr): Base(),
                               m_traits(tr)
  {}


  X_monotone_curve_2 operator()(Halfedge he) const
  {
    if(he.direction() != 
       m_traits->compare_endpoints_xy_2_object()(he.curve()))
       return (m_traits->construct_opposite_2_object()(he.curve()));
    return (he.curve());
  }
};

template <class Traits_>
 void General_polygon_set_2<Traits_>::
  construct_polygon(Ccb_halfedge_const_circulator ccb,
                    Polygon_2&              pgn,
                    Traits_*                tr)
{
  typedef CGAL::Container_from_circulator<Ccb_halfedge_const_circulator> Ccb_container;
  typedef typename Ccb_container::iterator                   Ccb_iterator;
  Ccb_container container(ccb);

  typedef CGAL::Join_input_iterator_1<Ccb_iterator, Curve_creator<Arrangement_2> > Curve_iterator;
  Curve_creator<Arrangement_2> cv_creator(tr);

  Curve_iterator  begin(container.begin(), cv_creator);
  Curve_iterator  end  (container.end()  , cv_creator);

  tr->construct_polygon_2_object()(begin, end, pgn);
}

template <class Arrangement, class OutputIterator>
class Arr_bfs_scanner
{
public:

  typedef typename Arrangement::Traits_2             Gps_traits;
  typedef typename Gps_traits::Polygon_2             Polygon_2;
  typedef typename Gps_traits::Polygon_with_holes_2  Polygon_with_holes_2;
  typedef typename Arrangement::Ccb_halfedge_const_circulator 
                                                     Ccb_halfedge_const_circulator;
  typedef typename Arrangement::Face_const_iterator        Face_const_iterator;
  typedef typename Arrangement::Halfedge_const_iterator    Halfedge_const_iterator;
  typedef typename Arrangement::Holes_const_iterator       Holes_const_iterator;


protected:

  Gps_traits*                      m_traits;
  std::queue<Face_const_iterator>        m_holes_q;
  std::list<Polygon_2>  m_pgn_holes;
  OutputIterator                   m_oi;

public:

  /*! Constructor */
  Arr_bfs_scanner(Gps_traits* tr, OutputIterator oi) : m_traits(tr), m_oi(oi)
  {}
                             

  void scan(Arrangement& arr)
  {
    Face_const_iterator   ubf = arr.unbounded_face();  
    Holes_const_iterator  holes_it;
    Face_const_iterator   fit;

    if(!ubf->contained())
    {
      ubf->set_visited(true);
      for (holes_it = ubf->holes_begin();
	   holes_it != ubf->holes_end(); ++holes_it)
      {
        scan_ccb (*holes_it);
      }
    }
    else
    {
      // ubf is contained -> unbounded polygon !!
      scan_contained_ubf(ubf);

    }

    while(!m_holes_q.empty())
    {
      Face_const_iterator top_f = m_holes_q.front();
      m_holes_q.pop();
      top_f->set_visited(true);
      for (holes_it = top_f->holes_begin();
	   holes_it != top_f->holes_end(); ++holes_it)
      {
        scan_ccb(*holes_it);
      }

      //scan_uncontained_face(top_f->outer_ccb());
    }

    for (fit = arr.faces_begin(); fit != arr.faces_end(); ++fit)
    {
      fit->set_visited(false);
    }
  }

  OutputIterator output_iterator() const
  {
    return m_oi;
  }

  void scan_ccb(Ccb_halfedge_const_circulator ccb)
  {
    Polygon_2 pgn_boundary;
    General_polygon_set_2<Gps_traits>::construct_polygon(ccb, pgn_boundary, m_traits);
    Halfedge_const_iterator he = ccb;
    if(!he->twin()->face()->visited())
      all_incident_faces(he->twin()->face());
    Polygon_with_holes_2 pgn(pgn_boundary,
                              m_pgn_holes.begin(),
                              m_pgn_holes.end());
    *m_oi = pgn;
    ++m_oi;
    m_pgn_holes.clear();
  }

  void scan_contained_ubf(Face_const_iterator ubf)
  {
    CGAL_assertion(ubf->is_unbounded() && ubf->contained());
    // ubf is contained -> unbounded polygon !!
    all_incident_faces(ubf);
    Polygon_2 boundary;
    Polygon_with_holes_2 pgn(boundary,
                              m_pgn_holes.begin(),
                              m_pgn_holes.end());
    *m_oi = pgn;
    ++m_oi;
    m_pgn_holes.clear();
  }


  void all_incident_faces(Face_const_iterator f)
  {
    CGAL_assertion(!f->visited());
    f->set_visited(true);
    if(!f->is_unbounded())
    {
      if(!f->contained())
      {
        m_pgn_holes.push_back(Polygon_2());
        General_polygon_set_2<Gps_traits>::construct_polygon(f->outer_ccb(), m_pgn_holes.back(), m_traits);
        m_holes_q.push(f);
      }

    
      Ccb_halfedge_const_circulator ccb_end = f->outer_ccb();
      Ccb_halfedge_const_circulator ccb_circ = ccb_end;
      do
      { 
        //get the current halfedge on the face boundary
        Halfedge_const_iterator he  = ccb_circ;
        Face_const_iterator new_f = he->twin()->face();
        if(!new_f->visited())
        {
          all_incident_faces(new_f);
        }
        ++ccb_circ;
      }
      while(ccb_circ != ccb_end);
    }

    Holes_const_iterator hit;
    for(hit = f->holes_begin(); hit != f->holes_end(); ++hit)
    {
      Ccb_halfedge_const_circulator ccb_of_hole = *hit;
      Halfedge_const_iterator he = ccb_of_hole;
      if(is_single_face(ccb_of_hole))
      {
        if(!he->twin()->face()->contained())
        {
          m_pgn_holes.push_back(Polygon_2());
          General_polygon_set_2<Gps_traits>::construct_polygon
	    (he->twin()->face()->outer_ccb(), m_pgn_holes.back(), m_traits);
          m_holes_q.push(he->twin()->face());
        }
      }
      else
        if(!he->twin()->face()->visited())
          all_incident_faces(he->twin()->face());
    }
  }

  bool is_single_face(Ccb_halfedge_const_circulator ccb)
  {
    Ccb_halfedge_const_circulator ccb_end = ccb;
    Ccb_halfedge_const_circulator ccb_circ = ccb_end;
    Halfedge_const_iterator he = ccb;
    Face_const_iterator curr_f = he->twin()->face();
    do
    { 
      //get the current halfedge on the face boundary
      Halfedge_const_iterator he  = ccb_circ;
      if(he->twin()->face() != curr_f)
        return false;
      if(he->twin()->target()->degree() != 2)
        return false;
      ++ccb_circ;
    }
    while(ccb_circ != ccb_end);
    return true;
  }
};


template<class Arrangement>
class Init_faces_visitor
{
  typedef typename Arrangement::Traits_2             Traits_2;
  typedef typename Arrangement::Ccb_halfedge_const_circulator 
                                                     Ccb_halfedge_const_circulator;
  typedef typename Arrangement::Face_const_iterator        Face_const_iterator;
  typedef typename Arrangement::Halfedge_const_iterator    Halfedge_const_iterator;
  typedef typename Arrangement::Holes_const_iterator       Holes_const_iterator;

public:

  void init(Arrangement& arr)
  {
  }
};
  
template < class Polygon_2, class Arrangement_2>
void pgn2arr(const Polygon_2& pgn, Arrangement_2& arr)
{
  typedef typename Arrangement_2::Traits_2             Traits_2;
  typedef typename Traits_2::Compare_endpoints_xy_2    Compare_endpoints_xy_2;
  typedef typename Traits_2::X_monotone_curve_2        X_monotone_curve_2;
  typedef typename Traits_2::Curve_const_iterator      Curve_const_iterator;
  typedef typename Arrangement_2::Halfedge_handle      Halfedge_handle;

  Compare_endpoints_xy_2  cmp_ends = 
    arr.get_traits()->compare_endpoints_xy_2_object();

  std::pair<Curve_const_iterator,
            Curve_const_iterator> itr_pair =
    arr.get_traits()->construct_curves_2_object()(pgn);

  if(itr_pair.first == itr_pair.second)
    return;

  Curve_const_iterator curr = itr_pair.first;
  Curve_const_iterator end  = itr_pair.second;

  Halfedge_handle first_he = 
    arr.insert_in_face_interior(*curr, arr.unbounded_face());
  //first_he is directed from left to right (see insert_in_face_interior)
  
  Halfedge_handle curr_he;
  if(cmp_ends(*curr) == CGAL::SMALLER)
  {
    // curr curve and first_he have the same direction
    curr_he = first_he;
    first_he = first_he->twin();
  }
  else
  {
    // curr curve and first_he have opposite directions
    CGAL_assertion(cmp_ends(*curr) == CGAL::LARGER);
    curr_he = first_he->twin();
  }

  Curve_const_iterator temp = curr;
  ++temp;
  if(temp == end) // a polygon with circular arcs may have only
                  // two edges (full circle for example)
  {
    Halfedge_handle he = 
      arr.insert_at_vertices(*temp, curr_he, first_he);
    if(he->face() == arr.unbounded_face())
      he->twin()->face()->set_contained(true);
    else
      he->face()->set_contained(true);
    return;
  }

  //The polygon has 3 or more edges
  Curve_const_iterator last = end;
  --last;
  for(++curr ; curr != last; ++curr)
  {
    const X_monotone_curve_2& curr_cv = *curr;
    if(cmp_ends(curr_cv) == CGAL::SMALLER)
      curr_he = arr.insert_from_left_vertex(curr_cv, curr_he);
    else
    {
      CGAL_assertion(cmp_ends(curr_cv) == CGAL::LARGER);
      curr_he = arr.insert_from_right_vertex(curr_cv, curr_he);
    }
  }

  const X_monotone_curve_2& last_cv = *last;
  Halfedge_handle last_he =
    arr.insert_at_vertices(last_cv, curr_he, first_he); 

  if(last_he->face() == arr.unbounded_face())
    last_he->twin()->face()->set_contained(true);
  else
    last_he->face()->set_contained(true);
}



//insert a range of simple polygons to the arrangement
template < class Traits_ >
  template< class PolygonIter >
void General_polygon_set_2<Traits_>::pgns2arr(PolygonIter p_begin,
                                              PolygonIter p_end,
                                              Arrangement_2&   arr)
{  
  typedef std::list<X_monotone_curve_2>                XCurveList;

  typedef Init_faces_visitor<Arrangement_2>              My_visitor;
  typedef Arr_bfs_scanner<Arrangement_2, My_visitor>     Arr_bfs_scanner;


  XCurveList xcurve_list;

  for(PolygonIter pitr = p_begin; pitr != p_end; ++pitr)
  {
    std::pair<Curve_const_iterator,
              Curve_const_iterator> itr_pair = 
      arr.get_traits()->construct_curves_2_object()(*pitr);
    std::copy(itr_pair.first, itr_pair.second, std::back_inserter(xcurve_list));
  }
  
  insert_non_intersecting_curves(arr, xcurve_list.begin(), xcurve_list.end());

  My_visitor v;
  Arr_bfs_scanner scanner(v);
  scanner.scan(arr);
}




 //insert non-sipmle poloygons with holes (non incident edges may have
// common vertex,  but they dont intersect at their interior
template < class Polygon_with_holes_2, class Arrangement_2 >
void pgn_with_holes2arr (const Polygon_with_holes_2& pgn, Arrangement_2& arr)
{
  typedef typename Arrangement_2::Traits_2             Traits_2;
  typedef typename Traits_2::Polygon_2                 Polygon_2;
  typedef typename Traits_2::Compare_endpoints_xy_2    Compare_endpoints_xy_2;
  typedef typename Traits_2::X_monotone_curve_2        X_monotone_curve_2;
  typedef typename Traits_2::Curve_const_iterator      Curve_const_iterator;
  typedef typename Arrangement_2::Halfedge_handle      Halfedge_handle;
  
  typedef typename Polygon_with_holes_2::Holes_const_iterator
                                                       GP_Holes_const_iterator;

  typedef std::list<X_monotone_curve_2>                XCurveList;
  typedef Init_faces_visitor<Arrangement_2>              My_visitor;
  typedef Arr_bfs_scanner<Arrangement_2, My_visitor>     Arr_bfs_scanner;

  XCurveList xcurve_list;
  
  bool is_bounded = !pgn.is_unbounded();
  if(is_bounded)
  {
    const Polygon_2& pgn_boundary = pgn.outer_boundary();
    std::pair<Curve_const_iterator,
              Curve_const_iterator> itr_pair = 
              arr.get_traits()->construct_curves_2_object()(pgn_boundary);
    std::copy (itr_pair.first, itr_pair.second, 
	       std::back_inserter(xcurve_list));
  }

  GP_Holes_const_iterator hit;
  for (hit = pgn.holes_begin(); hit != pgn.holes_end(); ++hit)
  {
    const Polygon_2& pgn_hole = *hit;
    std::pair<Curve_const_iterator,
              Curve_const_iterator> itr_pair =
              arr.get_traits()->construct_curves_2_object()(pgn_hole);
    std::copy (itr_pair.first, itr_pair.second,
	       std::back_inserter(xcurve_list));
  }
  
  insert_non_intersecting_curves(arr, xcurve_list.begin(), xcurve_list.end());

  if(!is_bounded)
    arr.unbounded_face()->set_contained(true);

  My_visitor v;
  Arr_bfs_scanner scanner(v);
  scanner.scan(arr);
}



//insert a range of  non-sipmle poloygons with holes (as decribed above)
template < class Traits_ >
  template< class InputIterator >
void General_polygon_set_2<Traits_>::pgns_with_holes2arr (InputIterator begin,
                                                         InputIterator end,
                                                         Arrangement_2& arr)
{
  typedef std::list<X_monotone_curve_2>                XCurveList;

  typedef Init_faces_visitor<Arrangement_2>              My_visitor;
  typedef Arr_bfs_scanner<Arrangement_2, My_visitor>     Arr_bfs_scanner;

  XCurveList              xcurve_list;
  GP_Holes_const_iterator holes_it;
  bool                    is_bounded = true;
  InputIterator           itr;
 
  for(itr = begin; itr != end; ++itr)
  {
    if(!itr->is_unbounded())
    {
      const Polygon_2& pgn = itr->outer_boundary();
      arr.get_traits()->construct_curves_2_object()
        (pgn, std::back_inserter(xcurve_list));
    }
    else
      is_bounded = false;


    for(holes_it = itr->holes_begin();
        holes_it != itr->holes_end();
        ++holes_it)
    {
      const Polygon_2& pgn_hole = *holes_it;
      arr.get_traits()->construct_curves_2_object()
	(pgn_hole, std::back_inserter(xcurve_list));
    }
  }

  insert_non_intersecting_curves(arr, xcurve_list.begin(), xcurve_list.end());

  if(!is_bounded)
    arr.unbounded_face()->set_contained(true);

  My_visitor v;
  Arr_bfs_scanner scanner(v);
  scanner.scan(arr);
}


template <class Traits_>
  template< class OutputIterator >
  OutputIterator
  General_polygon_set_2<Traits_>::polygons_with_holes(OutputIterator out) const
{
  typedef Arr_bfs_scanner<Arrangement_2, OutputIterator>     Arr_bfs_scanner;
  Arr_bfs_scanner scanner(this->m_traits, out);
  scanner.scan(*(this->m_arr));
  return (scanner.output_iterator());
}


template < class Traits_ >
typename  General_polygon_set_2<Traits_>::Size 
General_polygon_set_2<Traits_>::number_of_polygons_with_holes() const
{
  typedef Arr_bfs_scanner<Arrangement_2, Counting_output_iterator>     Arr_bfs_scanner;
  Counting_output_iterator coi;
  Arr_bfs_scanner scanner(this->m_traits, coi);
  scanner.scan(*(this->m_arr));
  return (scanner.output_iterator().current_counter());
}

template < class Traits_ >
bool General_polygon_set_2<Traits_>::locate(const Point_2& q, Polygon_with_holes_2& pgn) const
{
  Walk_pl pl(*m_arr);

  Object obj = pl.locate(q);
  Face_const_iterator f;
  if(CGAL::assign(f, obj))
  {
    if(!f->contained())
      return false;
  }
  else
  {
    Halfedge_const_handle he;
    if(CGAL::assign(he, obj))
    {
      if(he->face()->contained())
        f = he->face();
      else
      {
        CGAL_assertion(he->twin()->face()->contained());
        f = he->twin()->face();
      }
    }
    else
    {
      Vertex_const_handle v;
      CGAL_assertion(CGAL::assign(v, obj));
      CGAL::assign(v, obj);
      Halfedge_around_vertex_const_circulator hav = v->incident_halfedges();
      Halfedge_const_handle he = hav;
      if(he->face()->contained())
        f = he->face();
      else
      {
        CGAL_assertion(he->twin()->face()->contained());
        f = he->twin()->face();
      }
    }
  }

  typedef Oneset_iterator<Polygon_with_holes_2>    OutputItr;
  typedef Arr_bfs_scanner<Arrangement_2, OutputItr>     Arr_bfs_scanner;

  OutputItr oi (pgn);
  Arr_bfs_scanner scanner(this->m_traits, oi);
  
  
  Ccb_halfedge_const_circulator ccb_of_pgn = get_boundary_of_polygon(f);
  for(Face_const_iterator fit = m_arr->faces_begin();
      fit != m_arr->faces_end();
      ++fit)
  {
    fit->set_visited(false);
  }
  if(ccb_of_pgn == Ccb_halfedge_const_circulator()) // the polygon has no boundary
  {
    // f is unbounded 
    scanner.scan_contained_ubf(m_arr->unbounded_face());
  }
  else
  {
    Halfedge_const_handle he_of_pgn = ccb_of_pgn;
    for(Face_const_iterator fit = m_arr->faces_begin();
      fit != m_arr->faces_end();
      ++fit)
    {
      fit->set_visited(false);
    }
    he_of_pgn->face()->set_visited(true);
    scanner.scan_ccb(ccb_of_pgn);
  }

  for(Face_const_iterator fit = m_arr->faces_begin();
      fit != m_arr->faces_end();
      ++fit)
  {
    fit->set_visited(false);
  }
  return true;
}

template < class Traits_ >
typename General_polygon_set_2<Traits_>::Ccb_halfedge_const_circulator
  General_polygon_set_2<Traits_>::get_boundary_of_polygon(Face_const_iterator f) const
{
  CGAL_assertion(!f->visited());
  f->set_visited(true);
  
  if(f->is_unbounded())
  {
    return Ccb_halfedge_const_circulator();
  }
  Ccb_halfedge_const_circulator ccb_end = f->outer_ccb();
  Ccb_halfedge_const_circulator ccb_circ = ccb_end;
  do
  { 
    //get the current halfedge on the face boundary
    Halfedge_const_iterator he  = ccb_circ;
    Face_const_iterator new_f = he->twin()->face();
    if(!new_f->visited())
    {
      if(is_hole_of_face(new_f, he) && !new_f->contained())
        return (he->twin());
      return (get_boundary_of_polygon(new_f));
    }
    ++ccb_circ;
  }
  while(ccb_circ != ccb_end);
  CGAL_assertion(false);
  return Ccb_halfedge_const_circulator();
  
}

template < class Traits_ >
bool General_polygon_set_2<Traits_>::
  is_hole_of_face(Face_const_handle f,
                  Halfedge_const_handle he) const
{
  Holes_const_iterator   holes_it;
  for (holes_it = f->holes_begin(); holes_it != f->holes_end(); ++holes_it)
  {
    Ccb_halfedge_const_circulator ccb = *holes_it;
    Ccb_halfedge_const_circulator ccb_end = ccb;
    do
    {
      Halfedge_const_handle he_inside_hole = ccb;
      he_inside_hole = he_inside_hole->twin();
      if(he == he_inside_hole)
        return true;

      ++ccb;
    }
    while(ccb != ccb_end);
  }

  return false;
}

#endif
