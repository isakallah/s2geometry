#include "s2cell_index_utils.h"

#include "s2/s2latlng.h"

static const S2LatLng kToronto = S2LatLng::FromDegrees(43.687431, -79.394071);
static const S2LatLng kAcroTelAviv = S2LatLng::FromDegrees(32.065246, 34.785711);

void ParseIndexAndQuery(const std::string& file_path) {
  auto index = LoadS2CellIndex(file_path);
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
