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

using LabelString = std::string;
using S2CellIdLabelPair = std::pair<S2CellId, LabelString>;
using LatLngLabelPair = std::pair<S2LatLng, LabelString>;

struct Index {
  std::unique_ptr<S2CellIndex> s2cell_index;
  std::unique_ptr<ValueLexicon<LabelString>> labels_lexicon;
};

Index LoadS2CellIndex(const std::string& file_path);

std::vector<LatLngLabelPair> GetLatLngLabelPairs(const std::string& file_path);

void QueryS2CellIndex(
    const Index& index,
    const S2LatLng& lat_lng,
    const std::string& index_name,
    const std::string& lat_lng_name);
