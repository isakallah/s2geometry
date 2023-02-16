#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "s2/base/commandlineflags.h"
#include "s2/mutable_s2shape_index.h"
#include "s2/s2shapeutil_coding.h"
#include "s2/s2contains_point_query.h"
#include "s2/s2text_format.h"

S2_DEFINE_string(java_shape_file_path, "", "Path to the decoded shapes created using the Java version of this library");
S2_DEFINE_string(java_index_file_path, "", "Path to the decoded index created using the Java version of this library");

void QueryIndex() {

}

int main(int argc, char **argv) {
  std::string shape_file_path = absl::GetFlag(FLAGS_java_shape_file_path);
  std::string index_file_path = absl::GetFlag(FLAGS_java_index_file_path);
  shape_file_path = "/home/root/data/s2shapeindex_created_in_java/test_s2shape";
  index_file_path = "/home/root/data/s2shapeindex_created_in_java/test_s2shapeIndex";

  if (shape_file_path.empty() || index_file_path.empty()) {
    std::cout << "WTF DUDE?! Empty file path! shape_file_path: "
              << shape_file_path << ", index_file_path: " << index_file_path
              << std::endl;
    return 0;
  }

  std::ifstream shape_file(shape_file_path, std::ios::binary);
  std::vector<char> shape_buffer((std::istreambuf_iterator<char>(shape_file)), std::istreambuf_iterator<char>());
  std::ifstream index_file(index_file_path, std::ios::binary);
  std::vector<char> index_buffer((std::istreambuf_iterator<char>(index_file)), std::istreambuf_iterator<char>());

  Decoder shape_decoder(shape_buffer.data(), shape_buffer.size());
  Decoder index_decoder(index_buffer.data(), index_buffer.size());

  MutableS2ShapeIndex mutable_index;
  auto shape_factory = s2shapeutil::FullDecodeShapeFactory(&shape_decoder);
  bool index_decoding_succeeded = mutable_index.Init(&index_decoder, shape_factory);

  std::cout << "index_decoding_succeeded: " << (index_decoding_succeeded ? "YES HONEY!" : "NO YOU STUPID HOE!") << std::endl;
  std::cout << "is the index fresh?! " << (mutable_index.is_fresh() ? "YES" : "NO") << std::endl;

  S2LatLng lat_lng = S2LatLng::FromDegrees(66.351480, -150.026771);
  auto query = MakeS2ContainsPointQuery(&mutable_index);
  std::cout << "Does index contain lat,lng(" << lat_lng.ToStringInDegrees() << ") ? : "
          << (query.Contains(S2Point(lat_lng)) ? "YES HONEY!" : "NO HONEY...") << std::endl;

  S2LatLng lat_lng_tlv_office = S2LatLng::FromDegrees(32.065246, 34.785711);
  std::cout << "Does index contain lat,lng(" << lat_lng_tlv_office.ToStringInDegrees() << ") ? : "
          << (query.Contains(S2Point(lat_lng_tlv_office)) ? "YES HONEY!" : "NO HONEY...") << std::endl;

  // std::cout << "mutable_index string:" << std::endl;
  // std::cout << s2textformat::ToString(mutable_index) << std::endl;
  // std::cout << "mutable_index.num_shape_ids(): " << mutable_index.num_shape_ids() << std::endl;
  return 0;
}
