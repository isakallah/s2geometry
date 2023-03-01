#include <fstream>

#include "s2cell_index_utils.h"

int main(int argc, char **argv) {
  std::string index_file_path = "/home/root/data/s2cellindex/1677665361281_s2cells.csv";
  auto index = LoadS2CellIndex(index_file_path);

  std::string test_dataset_file_path = "/home/root/data/test_dataset/test_data_all.csv";
  auto lat_lng_label_pairs = GetLatLngLabelPairs(test_dataset_file_path);

  std::vector<std::chrono::microseconds> query_times;
  for (const auto& lat_lng_label_pair : lat_lng_label_pairs) {
    S2LatLng lat_lng;
    LabelString label_str;
    std::tie(lat_lng, label_str) = lat_lng_label_pair;

    S2CellUnion target({S2CellId(lat_lng)});

    auto start = std::chrono::high_resolution_clock::now();
    absl::flat_hash_set<S2CellIndex::Label> intersecting_labels;
    index.s2cell_index->GetIntersectingLabels(target, &intersecting_labels);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        elapsed);
    query_times.push_back(microseconds);
  }

  std::ofstream results;
  std::string output_file_name = "/home/root/results/s2cell_index_performance_test_" + std::to_string(std::time(nullptr)) + ".txt";
  results.open(output_file_name);
  for (const auto& microseconds : query_times) {
    results << microseconds.count() << std::endl;
  }
  results.close();
  return 0;
}
