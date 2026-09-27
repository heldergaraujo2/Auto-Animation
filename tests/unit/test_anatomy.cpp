#include "auto_animation/anatomy/Markers.hpp"

#include <cmath>
#include <iostream>
#include <string_view>

namespace {
int failures = 0;
void expect(bool v, std::string_view m) { if (!v) { std::cerr << "FAIL: " << m << '\n'; ++failures; } }
bool near(float a, float b) { return std::abs(a - b) < 1e-5f; }
}

int main() {
    using namespace auto_animation::anatomy;
    MarkerSet set;
    set.profile = "Humanoid";
    expect(set.add_or_replace({MarkerType::Pelvis,"", {0,1,0},{0,1,0},MarkerSpace::Object,-1,1,true}), "pelvis marker added");
    expect(set.add_or_replace({MarkerType::WingTipLeft,"wing.L",{2,3,4},{1,0,0},MarkerSpace::Surface,4,0.8f,false}), "custom wing marker added");
    expect(set.find(MarkerType::Pelvis) != nullptr, "marker can be found");
    expect(set.find(MarkerType::WingTipLeft,"wing.L")->bone_hint == 4, "bone hint persists");
    expect(set.add_or_replace({MarkerType::Pelvis,"",{5,6,7},{0,1,0},MarkerSpace::Object,-1,0.9f,true}), "duplicate key replaces marker");
    expect(near(set.find(MarkerType::Pelvis)->position.x,5.0f), "replacement works");
    expect(set.validate().valid(), "valid marker set passes");
    const auto text = set.serialize();
    MarkerSet decoded;
    std::string error;
    expect(MarkerSet::deserialize(text, decoded, error), "marker serialization round trips");
    expect(decoded.profile == "Humanoid" && decoded.markers.size() == 2, "decoded profile and marker count persist");
    expect(decoded.find(MarkerType::WingTipLeft,"wing.L")->position.z == 4.0f, "decoded coordinates persist");
    expect(set.remove(MarkerType::WingTipLeft,"wing.L"), "marker removal works");
    expect(!set.remove(MarkerType::WingTipLeft,"wing.L"), "missing marker removal fails cleanly");

    MarkerSet bad;
    bad.profile = "Bad";
    bad.markers.push_back({MarkerType::Head,"",{NAN,0,0},{0,1,0},MarkerSpace::Object,-1,1,true});
    expect(!bad.validate().valid(), "non-finite marker is rejected");
    return failures;
}
