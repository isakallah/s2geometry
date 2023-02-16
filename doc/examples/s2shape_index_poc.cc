#include <iostream>
#include <memory>

#include "s2/mutable_s2shape_index.h"
#include "s2/s2cell.h"
#include "s2/s2polygon.h"
#include "s2/s2contains_point_query.h"


int main(int argc, char **argv) {
  MutableS2ShapeIndex mutable_index;

  S2LatLng lat_lng = S2LatLng::FromE6(43687431, -79394071);
  std::cout << "Adding lat,lng(" << lat_lng.ToStringInDegrees() << ") to index" << std::endl;

  S2Cell cell(lat_lng);
  std::unique_ptr<S2Polygon> polygon = std::make_unique<S2Polygon>(cell);
  mutable_index.Add(std::make_unique<S2Polygon::Shape>(polygon.get()));

  std::cout << "mutable_index.num_shape_ids(): " << mutable_index.num_shape_ids() << std::endl;
  
  auto query = MakeS2ContainsPointQuery(&mutable_index);
  std::cout << "Does index contain lat,lng(" << lat_lng.ToStringInDegrees() << ") ? : "
            << (query.Contains(S2Point(lat_lng)) ? "YES" : "NO you stupid BITCH!") << std::endl;

  return 0;
}
