// Copyright (c) 2006  Tel-Aviv University (Israel).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
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
// Author(s)     : Ron Wein   <wein@post.tau.ac.il>

#ifndef CGAL_MINKOWSKI_SUM_DECOMP_2_H
#define CGAL_MINKOWSKI_SUM_DECOMP_2_H

#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Polygon_with_holes_2.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/General_polygon_set_2.h>
#include <CGAL/Gps_segment_traits_2.h>
#include <list>

namespace CGAL {

/*! \class
 * A class for computing the Minkowski sum of two simple polygons based on
 * their decomposition two convex sub-polygons, taking the pairwise sums and
 * computing the union of the sub-sums.
 */

template <class DecompStrategy_, class Container_>
class Minkowski_sum_by_decomposition_2 {
public:
  typedef DecompStrategy_                              Decomposition_strategy;
  typedef Container_                                   Container;
  typedef typename Decomposition_strategy::Polygon_2   Polygon_2;

  typedef typename Decomposition_strategy::Kernel      Kernel;
  typedef CGAL::Arr_segment_traits_2<Kernel>           Arr_segment_traits;
  typedef CGAL::Gps_segment_traits_2<Kernel, Container, Arr_segment_traits>
                                                       Traits_2;
  typedef CGAL::General_polygon_set_2<Traits_2>        General_polygon_set_2;
  typedef CGAL::Polygon_with_holes_2<Kernel,Container> Polygon_with_holes_2;

private:
  // Kernel types:
  typedef typename Kernel::Point_2                       Point_2;
  typedef typename Kernel::Vector_2                      Vector_2;
  typedef typename Kernel::Direction_2                   Direction_2;

  // Kernel functors:
  typedef typename Kernel::Compare_angle_with_x_axis_2   Compare_angle_2;
  typedef typename Kernel::Construct_translated_point_2  Translate_point_2;
  typedef typename Kernel::Construct_vector_2            Construct_vector_2;
  typedef typename Kernel::Construct_direction_2         Construct_direction_2;
  typedef typename Kernel::Compare_x_2                   Compare_x_2;
  typedef typename Kernel::Compare_y_2                   Compare_y_2;
  typedef typename Kernel::Compare_xy_2                  Compare_xy_2;

  // Polygon-related types:
  typedef typename Polygon_2::Vertex_circulator          Vertex_circulator;
  typedef std::list<Polygon_2>                           Polygons_list;
  typedef typename Polygons_list::iterator               Polygons_iterator;

  typedef std::list<Polygon_with_holes_2>
    Polygon_with_holes_list;

  // Data members:
  const Decomposition_strategy* m_decomposition_strategy;
  bool m_own_strategy;    // inidicates whether the stategy should be freed up.

  const Traits_2* m_traits;
  bool m_own_traits;      // inidicates whether the kernel should be freed up.

  Compare_angle_2         f_compare_angle;
  Translate_point_2       f_add;
  Construct_vector_2      f_vector;
  Construct_direction_2   f_direction;
  Compare_x_2             f_compare_x;
  Compare_y_2             f_compare_y;
  Compare_xy_2            f_compare_xy;

public:
  //! Default constructor.
  Minkowski_sum_by_decomposition_2() :
    m_decomposition_strategy(NULL),
    m_own_strategy(false),
    m_traits(NULL),
    m_own_traits(false)
  { init(); }

  //! Constructor.
  Minkowski_sum_by_decomposition_2(const Decomposition_strategy& strategy,
                                   const Traits_2& traits) :
    m_decomposition_strategy(&strategy),
    m_own_strategy(false),
    m_traits(&traits),
    m_own_traits(false)
  { init(); }

  //! Constructor.
  Minkowski_sum_by_decomposition_2(const Decomposition_strategy& strategy) :
    m_decomposition_strategy(&strategy),
    m_own_strategy(false),
    m_traits(NULL),
    m_own_traits(false)
  { init(); }

  //! Constructor.
  Minkowski_sum_by_decomposition_2(const Traits_2& traits) :
    m_decomposition_strategy(NULL),
    m_own_strategy(false),
    m_traits(&traits),
    m_own_traits(false)
  { init(); }

  /*! Destructor */
  ~Minkowski_sum_by_decomposition_2()
  {
    if (m_own_traits) {
      if (m_traits != NULL) {
        delete m_traits;
        m_traits = NULL;
      }
      m_own_traits = false;
    }

    if (m_own_strategy) {
      if (m_decomposition_strategy != NULL) {
        delete m_decomposition_strategy;
        m_decomposition_strategy = NULL;
      }
      m_own_strategy = false;
    }
  }

  //! Initialize
  void init()
  {
    // Allocate the traits if not provided.
    if (m_traits == NULL) {
      m_traits = new Traits_2;
      m_own_traits = true;
    }
    // Allocate the strategy if not provided.
    if (m_decomposition_strategy == NULL) {
      m_decomposition_strategy = new Decomposition_strategy;
      m_own_strategy = true;
    }
    // Obtain kernel functors.
    const Kernel* kernel = m_traits;
    f_compare_angle = kernel->compare_angle_with_x_axis_2_object();
    f_add = kernel->construct_translated_point_2_object();
    f_vector = kernel->construct_vector_2_object();
    f_direction = kernel->construct_direction_2_object();
    f_compare_x = kernel->compare_x_2_object();
    f_compare_y = kernel->compare_y_2_object();
    f_compare_xy = kernel->compare_xy_2_object();
  }

  /*!
   * Obtain the traits
   * \return the traits
   */
  const Traits_2* traits() const { return m_traits; }

  /*!
   * Obtain the decomposition strategy
   * \return the decomposition strategy
   */
  const Decomposition_strategy* decomposition_strategy() const
  { return m_decomposition_strategy; }

  /*!
   * Compute the Minkowski sum of two simple polygons.
   * \param pgn1 The first polygon.
   * \param pgn2 The second polygon.
   * \pre Both input polygons are simple.
   * \return The resulting polygon with holes, representing the sum.
   */
  Polygon_with_holes_2
  operator()(const Polygon_2& pgn1, const Polygon_2& pgn2) const
  {
    CGAL_precondition(pgn1.is_simple());
    CGAL_precondition(pgn2.is_simple());

    // Decompose both input polygons to convex sub-polygons.
    Polygons_list sub_pgns1;
    Polygons_list sub_pgns2;

    (*m_decomposition_strategy)(pgn1, std::back_inserter(sub_pgns1));
    (*m_decomposition_strategy)(pgn2, std::back_inserter(sub_pgns2));

    return operator()(sub_pgns1.begin(), sub_pgns1.end(),
                      sub_pgns2.begin(), sub_pgns2.end());
  }

  /*!
   * Compute the Minkowski sum of two polygon-with-holes.
   * \param pgn1 The first polygon.
   * \param pgn2 The second polygon.
   * \pre Both input polygons are simple.
   * \return The resulting polygon with holes, representing the sum.
   */
  Polygon_with_holes_2
  operator()(const Polygon_with_holes_2& pgn1,
             const Polygon_with_holes_2& pgn2) const
  {
    // Decompose both input polygons to convex sub-polygons.
    Polygons_list sub_pgns1;
    Polygons_list sub_pgns2;

    (*m_decomposition_strategy)(pgn1, std::back_inserter(sub_pgns1));
    (*m_decomposition_strategy)(pgn2, std::back_inserter(sub_pgns2));

    return operator()(sub_pgns1.begin(), sub_pgns1.end(),
                      sub_pgns2.begin(), sub_pgns2.end());
  }

  /*!
   * Compute the Minkowski sum of a simple polygon and a polygon-with-holes.
   * \param pgn1 The simple polygon.
   * \param pgn2 The polygon with holes.
   * \pre Both input polygons are simple.
   * \return The resulting polygon with holes, representing the sum.
   */
  Polygon_with_holes_2
  operator()(const Polygon_2& pgn1, const Polygon_with_holes_2& pgn2) const
  {
    CGAL_precondition(pgn1.is_simple());

    // Decompose both input polygons to convex sub-polygons.
    Polygons_list sub_pgns1;
    Polygons_list sub_pgns2;

    (*m_decomposition_strategy)(pgn1, std::back_inserter(sub_pgns1));
    (*m_decomposition_strategy)(pgn2, std::back_inserter(sub_pgns2));

    return operator()(sub_pgns1.begin(), sub_pgns1.end(),
                      sub_pgns2.begin(), sub_pgns2.end());
  }

  /*! Compute the union of the pairwise Minkowski sum of two sets of
   * convex polygons.
   * \param begin1 the iterator to the begin element of the first set.
   * \param end1 the iterator to the past-the-end element of the first set.
   * \param begin2 the iterator to the begin element of the second set.
   * \param end2 the iterator to the past-the-end element of the second set.
   */
  template <class InputIterator1, class InputIterator2>
  Polygon_with_holes_2 operator()(InputIterator1 begin1, InputIterator1 end1,
                                  InputIterator2 begin2, InputIterator1 end2)
    const
  {
    Polygons_list sub_sum_polygons;

    // Compute the sub-sums of all pairs of sub-polygons.
    for (InputIterator1 curr1 = begin1; curr1 != end1; ++curr1) {
      for (InputIterator2 curr2 = begin2; curr2 != end2; ++curr2) {
        // Compute the sum of the current pair of convex sub-polygons.
        Polygon_2 sub_sum;
        _compute_sum_of_convex(*curr1, *curr2, sub_sum);
        sub_sum_polygons.push_back(sub_sum);
      }
    }

    General_polygon_set_2 gps(*m_traits);
    gps.join(sub_sum_polygons.begin(), sub_sum_polygons.end());
    Polygon_with_holes_list sum;
    gps.polygons_with_holes(std::back_inserter(sum));
    CGAL_assertion(sum.size() == 1);
    return (*(sum.begin()));
  }

private:
  /*!
   * Compute the Minkowski sum of two convex polygons.
   * \param pgn1 The first convex polygon.
   * \param pgn2 The second convex polygon.
   * \param sub_sum Output: Polygon which is the sub sum of the two convex
   *        polygons
   */
  void _compute_sum_of_convex(const Polygon_2& pgn1,
                              const Polygon_2& pgn2,
                              Polygon_2& sub_sum) const
  {
    // Find the bottom-left vertex in both polygons.
    Vertex_circulator first1, curr1, next1;
    Vertex_circulator bottom_left1;
    Comparison_result res;
    bottom_left1 = curr1 = first1 = pgn1.vertices_circulator();
    ++curr1;
    while (curr1 != first1) {
      res = f_compare_y(*curr1, *bottom_left1);
      if ((res == SMALLER) ||
          ((res == EQUAL) && (f_compare_x(*curr1, *bottom_left1) == SMALLER)))
      {
        bottom_left1 = curr1;
      }
      ++curr1;
    }

    // Find the bottom-left vertex in both polygons.
    Vertex_circulator         first2, curr2, next2;
    Vertex_circulator         bottom_left2;

    bottom_left2 = curr2 = first2 = pgn2.vertices_circulator();
    ++curr2;
    while (curr2 != first2) {
      res = f_compare_y(*curr2, *bottom_left2);
      if ((res == SMALLER) ||
          ((res == EQUAL) && (f_compare_x(*curr2, *bottom_left2) == SMALLER)))
      {
        bottom_left2 = curr2;
      }
      ++curr2;
    }

    // Start from the bottom-left vertices.
    next1 = curr1 = bottom_left1;
    ++next1;
    next2 = curr2 = bottom_left2;
    ++next2;

    // Compute the Minkowski sum.
    Point_2 first_pt;
    Point_2 curr_pt;
    Point_2 prev_pt;
    bool is_first = true;
    bool inc1, inc2;
    bool moved_on1 = false;
    bool moved_on2 = false;

    do {
      // Compute the sum of the two current vertices (we actually translate
      // the point curr1 by a vector equivalent to the point curr2).
      curr_pt = f_add(*curr1, f_vector(CGAL::ORIGIN, *curr2));

      if (is_first) {
        // This is the first point we have computed.
        first_pt = prev_pt = curr_pt;
        is_first = false;
        sub_sum.push_back(first_pt);
      }
      else {
        // Add a segment from the previously summed point to the current one.
        res = f_compare_xy(prev_pt, curr_pt);
        CGAL_assertion (res != EQUAL);
        prev_pt = curr_pt;
        sub_sum.push_back(curr_pt);
      }

      // Compare the angles the current edges form with the x-axis.
      res = f_compare_angle(f_direction(f_vector(*curr1, *next1)),
                            f_direction(f_vector(*curr2, *next2)));

      // Proceed to the next vertex according to the result.
      inc1 = (res != LARGER);
      inc2 = (res != SMALLER);

      if (inc1 && moved_on1 && (curr1 == bottom_left1)) {
        inc1 = false;
        inc2 = true;
      }

      if (inc2 && moved_on2 && (curr2 == bottom_left2)) {
        inc1 = true;
        inc2 = false;
      }

      if (inc1) {
        curr1 = next1;
        ++next1;
        moved_on1 = true;
      }

      if (inc2) {
        curr2 = next2;
        ++next2;
         moved_on2 = true;
     }
    } while ((curr1 != bottom_left1) || (curr2 != bottom_left2));
  }
};

} //namespace CGAL

#endif
