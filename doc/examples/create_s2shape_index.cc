#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "s2/mutable_s2shape_index.h"
#include "s2/s2shapeutil_coding.h"
#include "s2/s2cell.h"
#include "s2/s2polygon.h"
#include "s2/s2contains_point_query.h"
#include "s2/s2text_format.h"


std::unique_ptr<MutableS2ShapeIndex> CreateMutableIndex(
    const std::vector<S2LatLng>& lat_lng_array) {

  auto mutable_index = std::make_unique<MutableS2ShapeIndex>();
  
  for (const auto& lat_lng : lat_lng_array) {
    uint64 ubimo_id = 17;
    S2Cell cell(lat_lng);

    auto owning_shape = std::make_unique<S2Polygon::CanadaCensusPolygon>();
    owning_shape->Init(ubimo_id, std::make_unique<S2Polygon>(cell));

    // how is this working? there should be no template convariance in C++...
    mutable_index->Add(std::move(owning_shape));
  }

  return std::move(mutable_index);
}

void QueryMutableS2ShapeIndex(
    MutableS2ShapeIndex* mutable_index,
    const S2LatLng& lat_lng,
    const std::string& index_name) {

  auto query = MakeS2ContainsPointQuery(mutable_index);
  auto point = S2Point(lat_lng);
  auto contains = query.Contains(point);
  auto containing_shapes = query.GetContainingShapes(point);
  
  auto num_containing_shapes = containing_shapes.size();
  bool error = (contains && num_containing_shapes == 0) || (!contains && num_containing_shapes > 0);
  S2_LOG_IF(ERROR, error) << "contains && num_containing_shapes don't match";

  std::cout << "Does " << index_name << " contain lat,lng(" << lat_lng.ToStringInDegrees() << ") ? : ";
  if (!contains) {
    std::cout << "NO you stupid BITCH! hahaha..." << std::endl << std::endl;
    return;
  }

  std::cout << "YES! num_containing_shapes: " << num_containing_shapes;
  std::cout << ", containing_shapes ubimo ids: ";
  std::string delim = "";
  for (const auto& shape : containing_shapes) {
    auto ubimo_id = static_cast<const uint64*>(shape->user_data());
    std::cout << delim << *ubimo_id;
    delim = ",";
  }
  std::cout << std::endl << std::endl;
}

void EncodeMutableIndex(const MutableS2ShapeIndex& mutable_index, Encoder* encoder) {
  std::cout << "EncodeMutableIndex" << std::endl << std::endl;
  s2shapeutil::FastEncodeTaggedShapes(mutable_index, encoder);
  mutable_index.Encode(encoder);
}

std::unique_ptr<MutableS2ShapeIndex> DecodeMutableIndex(const char* base, size_t length) {
  std::cout << "DecodeMutableIndex" << std::endl << std::endl;
  Decoder decoder(base, length);
  auto shape_factory = s2shapeutil::FullDecodeShapeFactory(&decoder);

  auto mutable_index = std::make_unique<MutableS2ShapeIndex>();
  mutable_index->Init(&decoder, shape_factory);

  return std::move(mutable_index);
}

int main(int argc, char **argv) {
  std::vector<S2LatLng> lat_lng_array = {
    S2LatLng::FromDegrees(43.687431, -79.394071),
    S2LatLng::FromDegrees(43.687431, -79.394071),
    S2LatLng::FromDegrees(43.687431, -79.394071),
    S2LatLng::FromDegrees(43.687431, -79.394071),
    S2LatLng::FromDegrees(43.687431, -79.394071),
    S2LatLng::FromDegrees(43.687431, -79.394071),
    S2LatLng::FromDegrees(43.687431, -79.394071)
  };
  auto mutable_index = CreateMutableIndex(lat_lng_array);

  QueryMutableS2ShapeIndex(mutable_index.get(), S2LatLng::FromDegrees(43.687431, -79.394071), "mutable_index");
  QueryMutableS2ShapeIndex(mutable_index.get(), S2LatLng::FromDegrees(32.065246, 34.785711), "mutable_index");

  Encoder encoder;
  EncodeMutableIndex(*mutable_index, &encoder);

  // delete the current index to make sure it's not used in the following actions
  mutable_index.reset();

  auto decoded_mutable_index = DecodeMutableIndex(encoder.base(), encoder.length());
  QueryMutableS2ShapeIndex(decoded_mutable_index.get(), S2LatLng::FromDegrees(43.687431, -79.394071), "decoded_mutable_index");
  QueryMutableS2ShapeIndex(decoded_mutable_index.get(), S2LatLng::FromDegrees(32.065246, 34.785711), "decoded_mutable_index");
  return 0;
}
