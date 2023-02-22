#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <fstream>

#include "s2/value_lexicon.h"
#include "s2/s2cell_index.h"
#include "s2/s2latlng.h"
#include "s2/s2cell_id.h"
#include "s2/s2cell_union.h"

static const S2LatLng kToronto = S2LatLng::FromDegrees(43.687431, -79.394071);
static const S2LatLng kAcroTelAviv = S2LatLng::FromDegrees(32.065246, 34.785711);

using LabelString = std::string;
using S2CellIdLabelPair = std::pair<S2CellId, LabelString>;

struct Index {
  std::unique_ptr<S2CellIndex> s2cell_index;
  std::unique_ptr<ValueLexicon<LabelString>> labels_lexicon;
};

std::vector<S2CellIdLabelPair> GetS2CellLabelPairs(const std::string& file_path) {
  std::vector<S2CellIdLabelPair> s2cell_label_pairs;

  std::ifstream file_stream(file_path);
  std::string line;
  std::getline(file_stream, line); // skip header
  while(std::getline(file_stream, line)) {
    std::istringstream line_stream(line);
    std::string s2cell_id_str;
    std::getline(line_stream, s2cell_id_str,',');
    auto s2cell_id = S2CellId(std::stoull(s2cell_id_str));

    std::string label_str;
    std::getline(line_stream, label_str,',');

    s2cell_label_pairs.push_back(S2CellIdLabelPair(s2cell_id, label_str));
  }
  return s2cell_label_pairs;
}

Index CreateS2CellIndex(
    const std::vector<S2CellIdLabelPair>& s2cell_label_pairs) {

  std::cout << "CreateS2CellIndex" << std::endl;

  // notice that ValueLexicon::Add is fucking stupid and returns uint32,
  // when S2CellIndex::Label is a fucking int32.
  Index index;
  index.labels_lexicon = std::make_unique<ValueLexicon<LabelString>>();
  index.s2cell_index = std::make_unique<S2CellIndex>();

  for (const auto& s2cell_label_pair : s2cell_label_pairs) {
    S2CellId s2cell_id;
    std::string label_str;
    std::tie(s2cell_id, label_str) = s2cell_label_pair;
    index.s2cell_index->Add(s2cell_id, index.labels_lexicon->Add(label_str));
  }

  index.s2cell_index->Build();

  std::cout << "Finished creating S2CellIndex, s2cell_index->num_cells(): " << index.s2cell_index->num_cells() << std::endl << std::endl;
  return std::move(index);
}

void QueryS2CellIndex(
    const Index& index,
    const S2LatLng& lat_lng,
    const std::string& index_name,
    const std::string& lat_lng_name) {

  // Notice that S2ClosestPointQuery can also be used
  S2CellUnion target({S2CellId(lat_lng)});
  absl::flat_hash_set<S2CellIndex::Label> intersecting_labels = index.s2cell_index->GetIntersectingLabels(target);
  
  auto num_intersecting_labels = intersecting_labels.size();

  std::cout << "Does " << index_name << " contain lat,lng(" << lat_lng.ToStringInDegrees()
            << ") (" << lat_lng_name << ") ? ";
  if (num_intersecting_labels == 0) {
    std::cout << "NO you stupid BITCH! hahaha... BYE!" << std::endl;
    return;
  }

  std::cout << "YES! num_intersecting_labels: " << num_intersecting_labels;
  std::cout << ", intersecting_labels: ";
  std::string delim = "";
  for (const auto& label : intersecting_labels) {
    std::cout << delim << index.labels_lexicon->value(label);
    delim = ",";
  }
  std::cout << std::endl;
}

void ParseIndexAndQuery(const std::string& file_path) {
  auto s2cell_label_pairs = GetS2CellLabelPairs(file_path);
  auto index = CreateS2CellIndex(s2cell_label_pairs);
  QueryS2CellIndex(index, kToronto, file_path, "kToronto");
  QueryS2CellIndex(index, kAcroTelAviv, file_path, "kAcroTelAviv");
  std::cout << std::endl;
}

int main(int argc, char **argv) {
  ParseIndexAndQuery("/home/root/data/s2cellindex/1676894426715_s2cells.csv");
  ParseIndexAndQuery("/home/root/data/s2cellindex/1676909243333_s2cells.csv");
  ParseIndexAndQuery("/home/root/data/s2cellindex/1677012853219_s2cells.csv");
  // ParseIndexAndQuery("/home/root/data/s2cellindex/parent_cell_test.csv");
  return 0;
}
