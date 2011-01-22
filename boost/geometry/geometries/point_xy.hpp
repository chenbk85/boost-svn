// Boost.Geometry (aka GGL, Generic Geometry Library)
//
// Copyright Barend Gehrels 2007-2009, Geodan, Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_GEOMETRIES_POINT_XY_HPP
#define BOOST_GEOMETRY_GEOMETRIES_POINT_XY_HPP

#include <cstddef>

#include <boost/mpl/int.hpp>

#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/point.hpp>

namespace boost { namespace geometry
{

namespace model { namespace d2
{

/*!
\brief 2D point in Cartesian coordinate system
\tparam CoordinateType numeric type, for example, double, float, int
\tparam CoordinateSystem coordinate system, defaults to cs::cartesian
*/
template<typename CoordinateType, typename CoordinateSystem = cs::cartesian>
class point_xy : public model::point<CoordinateType, 2, CoordinateSystem>
{
public:

    /// Default constructor, does not initialize anything
    inline point_xy()
        : model::point<CoordinateType, 2, CoordinateSystem>()
    {}

    /// Constructor with x/y values
    inline point_xy(CoordinateType const& x, CoordinateType const& y)
        : model::point<CoordinateType, 2, CoordinateSystem>(x, y)
    {}

    /// Get x-value
    inline CoordinateType const& x() const
    { return this->template get<0>(); }

    /// Get y-value
    inline CoordinateType const& y() const
    { return this->template get<1>(); }

    /// Set x-value
    inline void x(CoordinateType const& v)
    { this->template set<0>(v); }

    /// Set y-value
    inline void y(CoordinateType const& v)
    { this->template set<1>(v); }
};


}} // namespace model::d2


// Adapt the point_xy to the concept
#ifndef DOXYGEN_NO_TRAITS_SPECIALIZATIONS
namespace traits
{

template <typename CoordinateType, typename CoordinateSystem>
struct tag<model::d2::point_xy<CoordinateType, CoordinateSystem> >
{
    typedef point_tag type;
};

template<typename CoordinateType, typename CoordinateSystem>
struct coordinate_type<model::d2::point_xy<CoordinateType, CoordinateSystem> >
{
    typedef CoordinateType type;
};

template<typename CoordinateType, typename CoordinateSystem>
struct coordinate_system<model::d2::point_xy<CoordinateType, CoordinateSystem> >
{
    typedef CoordinateSystem type;
};

template<typename CoordinateType, typename CoordinateSystem>
struct dimension<model::d2::point_xy<CoordinateType, CoordinateSystem> >
    : boost::mpl::int_<2>
{};

template<typename CoordinateType, typename CoordinateSystem, std::size_t Dimension>
struct access<model::d2::point_xy<CoordinateType, CoordinateSystem>, Dimension >
{
    static inline CoordinateType get(
        model::d2::point_xy<CoordinateType, CoordinateSystem> const& p)
    {
        return p.template get<Dimension>();
    }

    static inline void set(model::d2::point_xy<CoordinateType, CoordinateSystem>& p,
        CoordinateType const& value)
    {
        p.template set<Dimension>(value);
    }
};

} // namespace traits
#endif // DOXYGEN_NO_TRAITS_SPECIALIZATIONS

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_GEOMETRIES_POINT_XY_HPP