#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "s2/mutable_s2shape_index.h"
#include "s2/s2shapeutil_coding.h"
#include "s2/s2cell.h"
#include "s2/s2polygon.h"
#include "s2/s2contains_point_query.h"

MutableS2ShapeIndex CreateMutableIndex() {
  MutableS2ShapeIndex mutable_index;

  S2LatLng lat_lng = S2LatLng::FromE6(43687431, -79394071);
  std::cout << "Adding lat,lng(" << lat_lng.ToStringInDegrees() << ") to index" << std::endl;

  S2Cell cell(lat_lng);
  std::unique_ptr<S2Polygon> polygon = std::make_unique<S2Polygon>(cell);
  mutable_index.Add(std::make_unique<S2Polygon::Shape>(polygon.get()));
}

void EncodeMutableIndex(MutableS2ShapeIndex* mutable_index, Encoder* encoder) {
  s2shapeutil::FastEncodeTaggedShapes(*mutable_index, encoder);
  mutable_index->Encode(encoder);
}

std::unique_ptr<MutableS2ShapeIndex> DecodeMutableIndex(const char* base, size_t length) {
  Decoder decoder(base, length);
  auto mutable_index = std::make_unique<MutableS2ShapeIndex>();

  return std::move(mutable_index);
}

void QueryMutableS2ShapeIndex(MutableS2ShapeIndex* mutable_index, S2LatLng lat_lng) {
  auto query = MakeS2ContainsPointQuery(mutable_index);
  std::cout << "Does index contain lat,lng(" << lat_lng.ToStringInDegrees() << ") ? : "
            << (query.Contains(S2Point(lat_lng)) ? "YES" : "NO you stupid BITCH!") << std::endl;
}

int main(int argc, char **argv) {
  MutableS2ShapeIndex mutable_index = CreateMutableIndex();

  Encoder encoder;
  EncodeMutableIndex(&mutable_index, &encoder);

  auto decoded_mutable_index = DecodeMutableIndex(encoder.base(), encoder.length());
  // decoded_mutable_index.
  return 0;
}
