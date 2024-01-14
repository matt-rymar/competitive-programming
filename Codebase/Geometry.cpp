// WARNING: Geometry is notorious for tricky and numerous edge cases, so many
// tasks are required to gain confidence in correctness of the code templates
// here. At the same time, most algorithms here are quite untested, so don't
// trust them and use them at your own peril. In fact, recently I've lost a few
// hundred positions in a Google Kickstarter because of a stupid bug in
// polarAngleLess, which seems like a relatively simple function to get right.

template <typename T>
struct Point {
  T x, y;

  friend bool operator==(const Point& lhs, const Point& rhs) {
    return std::tie(lhs.x, lhs.y) == std::tie(rhs.x, rhs.y);
  }
  friend bool operator!=(const Point& lhs, const Point& rhs) {
    return !(lhs == rhs);
  }
  Point& operator-=(const Point& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }
  friend Point operator-(Point lhs, const Point& rhs) {
    lhs -= rhs;
    return lhs;
  }

  std::size_t hash() const { return pair_hash::hash(std::make_pair(x, y)); }

  friend std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << p.x << ' ' << p.y;
    return os;
  }
  // Warning: no error handling.
  friend std::istream& operator>>(std::istream& is, Point& p) {
    is >> p.x >> p.y;
    return is;
  }
};

namespace std {

template <typename T>
struct hash<Point<T>> {
  std::size_t operator()(const Point<T>& p) const { return p.hash(); }
};

}  // namespace std

template <typename T>
T crossProduct(const Point<T>& a, const Point<T>& b) {
  return a.x * b.y - a.y * b.x;
}

template <typename T>
T dotProduct(const Point<T>& a, const Point<T>& b) {
  return a.x * b.x + a.y * b.y;
}

template <typename T>
T magSquared(const Point<T>& a) {
  return a.x * a.x + a.y * a.y;
}

template <typename T>
T distSquared(const Point<T>& a, const Point<T>& b) {
  return magSquared(a - b);
}

// ------------------------------------------------------------
// Turn direction stuff.
// ------------------------------------------------------------

enum class TurnDir { left, colinear, right };
template <typename T>
TurnDir turnDirection(const Point<T>& a, Point<T> b, Point<T> c) {
  b -= a;
  c -= a;
  const auto prod = crossProduct(b, c);
  if (prod > 0)
    return TurnDir::left;
  if (prod == 0)
    return TurnDir::colinear;
  return TurnDir::right;
}

// ------------------------------------------------------------
// Line segment computation stuff.
// ------------------------------------------------------------

// Checks whether p is on the line segment from a to b (inclusive both ends).
//
// For an example use, see
//  - Google Kickstart 2021F: Star Trappers
//  https://codingcompetitions.withgoogle.com/kickstart/round/0000000000435bae/0000000000888d45#problem
template <typename T>
bool pointOnSegment(const Point<T>& p, const Point<T>& a, const Point<T> b) {
  if (turnDirection(p, a, b) != TurnDir::colinear)
    return false;

  auto [xLeft, xRight] = std::minmax(a.x, b.x);
  auto [yDown, yUp] = std::minmax(a.y, b.y);
  return (xLeft <= p.x) && (p.x <= xRight) && (yDown <= p.y) && (p.y <= yUp);
}

// ------------------------------------------------------------
// Polygon algorithms stuff.
// ------------------------------------------------------------

// Checks whether p lies on one of the line segments of the polygon.
//
// The polygon can be arbitrary (convex or concave), with vertices in either
// clockwise or counter-clockwise order.
//
// Runs in O(n) time.
bool pointOnBoundary(const Point<double>& p,
                     const std::vector<Point<double>>& points) {
  for (int i = 1; i < points.size(); ++i) {
    if (pointOnSegment(p, points[i - 1], points[i]))
      return true;
  }

  return pointOnSegment(p, points.back(), points.front());
}

// Returns true if p is strictly inside the polygon (i.e., not on its boundary).
// The polygon can be arbitrary (convex or concave), without duplicate points
// and with vertices in either clockwise or counter-clockwise order.
//
// Runs in O(n) time.
//
// Based on: https://stackoverflow.com/a/15599478 and
// https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html.
//
// For an example use, see
//  - Google Kickstart 2021F: Star Trappers
//  https://codingcompetitions.withgoogle.com/kickstart/round/0000000000435bae/0000000000888d45#problem
bool pointInPolygon(const Point<double>& p,
                    const std::vector<Point<double>>& points) {
  int i, j, nvert = points.size();
  bool c = false;

  for (i = 0, j = nvert - 1; i < nvert; j = i++) {
    if (((points[i].y > p.y) != (points[j].y > p.y)) &&
        (p.x < (points[j].x - points[i].x) * (p.y - points[i].y) /
                       (points[j].y - points[i].y) +
                   points[i].x))
      c = !c;
  }

  return c && !pointOnBoundary(p, points);
}

// Computes the perimeter of the given polygon.
//
// The polygon can be arbitrary (convex or concave), with vertices in either
// clockwise or counter-clockwise order.
//
// Runs in O(n) time.
double perimeter(const std::vector<Point<double>>& points) {
  double res = std::sqrt(magSquared(points[0] - points.back()));

  for (int i = 1; i < points.size(); ++i)
    res += std::sqrt(magSquared(points[i] - points[i - 1]));

  return res;
}

// ------------------------------------------------------------
// Polar angle stuff.
// ------------------------------------------------------------

// Does a polar angle comparison with respect to the given origin (and the x-y
// axes being parallel to the normal ones).
//
// If points are colinear, points farther from origin are "smaller," so in a
// sorted order, farther points are put first.
//
// The origin is considered less than all other points (except itself).
template <typename T>
bool polarAngleLess(const Point<T>& origin, Point<T> a, Point<T> b) {
  if (a == origin)
    return b != origin;
  if (b == origin)
    return false;

  a -= origin;
  b -= origin;

  // If points are on two different half-planes, the one in the y > 0 half-plane
  // has a smaller polar angle.
  if (a.y * b.y < 0)
    return a.y > 0;
  // If at least one of the points is on the x-axis, we have to deal with many
  // annoying edge cases.
  if (a.y * b.y == 0) {
    if (a.y == 0) {
      if (b.y == 0) {
        if (a.x * b.x < 0)
          return a.x > 0;
        return magSquared(a) > magSquared(b);
      }
      return (a.x >= 0) || (b.y < 0);
    }
    return (b.x < 0) && (a.y > 0);
  }
  // Finally, we have the normal case: points are on the same half-plane, and
  // neither is on the x-axis, so we can simply use the cross product to check
  // the turn direction from a to b.
  const auto prod = crossProduct(a, b);
  if (prod == 0)
    return magSquared(a) > magSquared(b);
  return prod > 0;
}

// ------------------------------------------------------------
// Polar sort stuff.
// ------------------------------------------------------------

// If polar angle is the same, breaks ties by putting farthest points first
// (except for the origin, which is smaller than any other point).
//
// For an example use, see
//  - Google Kickstart 2021F: Star Trappers
//  https://codingcompetitions.withgoogle.com/kickstart/round/0000000000435bae/0000000000888d45#problem
template <typename T>
void polarSort(std::vector<Point<T>>& vec, Point<T> orig = Point<T>{0, 0}) {
  std::sort(vec.begin(), vec.end(), [&orig](const auto& p1, const auto& p2) {
    return polarAngleLess(orig, p1, p2);
  });
}

// If polar angle is the same, breaks ties by putting farthest points first
// (except for the origin, which is smaller than any other point).
//
// WARNING: technically speaking, this one is untested (although it's so similar
// to the basic polar sort above that I have to imagine it works).
template <typename T>
std::vector<std::pair<int, Point<T>>> polarSortTracedCopy(
    const std::vector<Point<T>>& vec, Point<T> orig = Point<T>{0, 0}) {
  auto cpy = std::vector<std::pair<int, Point<T>>>();
  cpy.reserve(vec.size());
  for (int i = 0; i < vec.size(); ++i)
    cpy.emplace_back(i, vec[i]);

  std::sort(cpy.begin(), cpy.end(), [&orig](const auto& p1, const auto& p2) {
    return polarAngleLess(orig, p1.second, p2.second);
  });
  return cpy;
}

// ------------------------------------------------------------
// Convex hull stuff.
// ------------------------------------------------------------

// Ensures that at each polar angle there is only one point (the furthest one,
// if used with polarSort()).
// (With possible exception of the first and the last ray, which will contain
// all the points on those, __if allOnBoundary is set to true__.)
//
// Note that it assumes that the points vector does not contain duplicate
// points.
template <typename T>
void polarUnique(std::vector<Point<T>>& points, bool allOnBoundary) {
  auto beg = points.begin() + 1;
  if (allOnBoundary) {
    ++beg;
    while ((beg != points.end()) && (turnDirection(points.front(), *(beg - 1),
                                                   *beg) == TurnDir::colinear))
      ++beg;
    // By default, polar sort puts the farthest points first (so that
    // std::unique works well with it). However, for convex hull we need the
    // first set of points to be in order of ascending distance from the origin
    // point.
    std::reverse(points.begin() + 1, beg);
    if (beg == points.end())
      return;
  }
  auto end = points.end();
  if (allOnBoundary) {
    // We know that there are at least two different polar angles (else we'd
    // have had beg == points.end() and would have returned).
    // => No need for bound checks.
    --end;
    while (turnDirection(points.front(), *(end - 1), *end) == TurnDir::colinear)
      --end;
  }
  points.erase(std::unique(beg, end,
                           [&points](const auto& p1, const auto& p2) {
                             return turnDirection(points.front(), p1, p2) ==
                                    TurnDir::colinear;
                           }),
               end);
}

// WARNING: Not tested on a problem where I needed to call convexHull with
// allOnBoundary == false.
//
// Implements Graham's scan, possibly extended to also return all points on the
// boundary of the convex hull.
//
// Runs in O(nlgn).
//
// Note: the convex hull returned does not contain any duplicate points.
// If allOnBoundary is set to true, the convex hull includes all the points on
// the boundary of the convex hull (that is, including points that lie on the
// line segment between a pair of other points on the hull).
//
// Note that even if allOnBoundary is set to true, duplicates will be removed.
template <typename T>
std::vector<Point<T>> convexHull(std::vector<Point<T>> points,
                                 bool allOnBoundary) {
  if (points.size() <= 2)
    return points;
  const auto lowestPoint = *std::min_element(
      points.cbegin(), points.cend(), [](const auto& p1, const auto& p2) {
        return std::tie(p1.y, p1.x) < std::tie(p2.y, p2.x);
      });
  polarSort(points, lowestPoint);
  points.erase(std::unique(points.begin(), points.end()), points.end());
  polarUnique(points, allOnBoundary);
  if (points.size() <= 3)
    return points;
  std::vector<Point<T>> stack{points[0], points[1]};
  for (int i = 2; i < points.size(); ++i) {
    while (!((turnDirection(stack[stack.size() - 2], stack[stack.size() - 1],
                            points[i]) == TurnDir::left) ||
             (allOnBoundary &&
              (turnDirection(stack[stack.size() - 2], stack[stack.size() - 1],
                             points[i]) == TurnDir::colinear))))
      stack.pop_back();
    stack.push_back(points[i]);
  }

  return stack;
}