/*
 *  Copyright (c) 2022 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/video_coding/svc/scalability_mode_util.h"

#include <array>
#include <utility>

#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include "api/video_codecs/scalability_mode.h"
#include "api/video_codecs/video_codec.h"
#include "rtc_base/checks.h"

namespace webrtc {

namespace {
struct ScalabilityModeParameters {
  int num_spatial_layers;
  int num_temporal_layers;
  InterLayerPredMode inter_layer_pred;
  absl::optional<ScalabilityModeResolutionRatio> ratio;
  bool shift;

  constexpr bool operator==(const ScalabilityModeParameters& other) const {
    // For all L1Tx modes, ignore inter_layer_pred, ratio and shift.
    if (this->num_spatial_layers == 1) {
      return this->num_spatial_layers == other.num_spatial_layers &&
             this->num_temporal_layers == other.num_temporal_layers;
    }
    return this->num_spatial_layers == other.num_spatial_layers &&
           this->num_temporal_layers == other.num_temporal_layers &&
           this->inter_layer_pred == other.inter_layer_pred &&
           this->ratio == other.ratio && this->shift == other.shift;
  }
};

struct ScalabilityModeConfiguration {
  explicit ScalabilityModeConfiguration(ScalabilityMode scalability_mode)
      : scalability_mode(scalability_mode),
        params{
            .num_spatial_layers =
                (ScalabilityModeToNumSpatialLayers(scalability_mode)),
            .num_temporal_layers =
                (ScalabilityModeToNumTemporalLayers(scalability_mode)),
            .inter_layer_pred =
                (ScalabilityModeToInterLayerPredMode(scalability_mode)),
            .ratio = (ScalabilityModeToResolutionRatio(scalability_mode)),
            .shift = (ScalabilityModeIsShiftMode(scalability_mode)),
        } {}

  const ScalabilityMode scalability_mode;
  const ScalabilityModeParameters params;
};

constexpr size_t kNumScalabilityModes =
    static_cast<size_t>(ScalabilityMode::kS3T3h) + 1;
}  // namespace

absl::optional<ScalabilityMode> MakeScalabilityMode(
    int num_spatial_layers,
    int num_temporal_layers,
    InterLayerPredMode inter_layer_pred,
    absl::optional<ScalabilityModeResolutionRatio> ratio,
    bool shift) {
  ScalabilityModeParameters params{num_spatial_layers, num_temporal_layers,
                                   inter_layer_pred, std::move(ratio), shift};

  static const ScalabilityModeConfiguration kScalabilityModeConfigs[] = {
      ScalabilityModeConfiguration{ScalabilityMode::kL1T1},
      ScalabilityModeConfiguration{ScalabilityMode::kL1T2},
      ScalabilityModeConfiguration{ScalabilityMode::kL1T3},
      ScalabilityModeConfiguration{ScalabilityMode::kL2T1},
      ScalabilityModeConfiguration{ScalabilityMode::kL2T1h},
      ScalabilityModeConfiguration{ScalabilityMode::kL2T1_KEY},
      ScalabilityModeConfiguration{ScalabilityMode::kL2T2},
      ScalabilityModeConfiguration{ScalabilityMode::kL2T2h},
      ScalabilityModeConfiguration{ScalabilityMode::kL2T2_KEY},
      ScalabilityModeConfiguration{ScalabilityMode::kL2T2_KEY_SHIFT},
      ScalabilityModeConfiguration{ScalabilityMode::kL2T3},
      ScalabilityModeConfiguration{ScalabilityMode::kL2T3h},
      ScalabilityModeConfiguration{ScalabilityMode::kL2T3_KEY},
      ScalabilityModeConfiguration{ScalabilityMode::kL3T1},
      ScalabilityModeConfiguration{ScalabilityMode::kL3T1h},
      ScalabilityModeConfiguration{ScalabilityMode::kL3T1_KEY},
      ScalabilityModeConfiguration{ScalabilityMode::kL3T2},
      ScalabilityModeConfiguration{ScalabilityMode::kL3T2h},
      ScalabilityModeConfiguration{ScalabilityMode::kL3T2_KEY},
      ScalabilityModeConfiguration{ScalabilityMode::kL3T3},
      ScalabilityModeConfiguration{ScalabilityMode::kL3T3h},
      ScalabilityModeConfiguration{ScalabilityMode::kL3T3_KEY},
      ScalabilityModeConfiguration{ScalabilityMode::kS2T1},
      ScalabilityModeConfiguration{ScalabilityMode::kS2T1h},
      ScalabilityModeConfiguration{ScalabilityMode::kS2T2},
      ScalabilityModeConfiguration{ScalabilityMode::kS2T2h},
      ScalabilityModeConfiguration{ScalabilityMode::kS2T3},
      ScalabilityModeConfiguration{ScalabilityMode::kS2T3h},
      ScalabilityModeConfiguration{ScalabilityMode::kS3T1},
      ScalabilityModeConfiguration{ScalabilityMode::kS3T1h},
      ScalabilityModeConfiguration{ScalabilityMode::kS3T2},
      ScalabilityModeConfiguration{ScalabilityMode::kS3T2h},
      ScalabilityModeConfiguration{ScalabilityMode::kS3T3},
      ScalabilityModeConfiguration{ScalabilityMode::kS3T3h},
  };
  static_assert(std::size(kScalabilityModeConfigs) == kNumScalabilityModes);

  for (const auto& candidate_mode : kScalabilityModeConfigs) {
    if (candidate_mode.params == params) {
      return candidate_mode.scalability_mode;
    }
  }
  return absl::nullopt;
}

absl::optional<ScalabilityMode> ScalabilityModeFromString(
    absl::string_view mode_string) {
  if (mode_string == "L1T1")
    return ScalabilityMode::kL1T1;
  if (mode_string == "L1T2")
    return ScalabilityMode::kL1T2;
  if (mode_string == "L1T3")
    return ScalabilityMode::kL1T3;

  if (mode_string == "L2T1")
    return ScalabilityMode::kL2T1;
  if (mode_string == "L2T1h")
    return ScalabilityMode::kL2T1h;
  if (mode_string == "L2T1_KEY")
    return ScalabilityMode::kL2T1_KEY;

  if (mode_string == "L2T2")
    return ScalabilityMode::kL2T2;
  if (mode_string == "L2T2h")
    return ScalabilityMode::kL2T2h;
  if (mode_string == "L2T2_KEY")
    return ScalabilityMode::kL2T2_KEY;
  if (mode_string == "L2T2_KEY_SHIFT")
    return ScalabilityMode::kL2T2_KEY_SHIFT;
  if (mode_string == "L2T3")
    return ScalabilityMode::kL2T3;
  if (mode_string == "L2T3h")
    return ScalabilityMode::kL2T3h;
  if (mode_string == "L2T3_KEY")
    return ScalabilityMode::kL2T3_KEY;

  if (mode_string == "L3T1")
    return ScalabilityMode::kL3T1;
  if (mode_string == "L3T1h")
    return ScalabilityMode::kL3T1h;
  if (mode_string == "L3T1_KEY")
    return ScalabilityMode::kL3T1_KEY;

  if (mode_string == "L3T2")
    return ScalabilityMode::kL3T2;
  if (mode_string == "L3T2h")
    return ScalabilityMode::kL3T2h;
  if (mode_string == "L3T2_KEY")
    return ScalabilityMode::kL3T2_KEY;

  if (mode_string == "L3T3")
    return ScalabilityMode::kL3T3;
  if (mode_string == "L3T3h")
    return ScalabilityMode::kL3T3h;
  if (mode_string == "L3T3_KEY")
    return ScalabilityMode::kL3T3_KEY;

  if (mode_string == "S2T1")
    return ScalabilityMode::kS2T1;
  if (mode_string == "S2T1h")
    return ScalabilityMode::kS2T1h;
  if (mode_string == "S2T2")
    return ScalabilityMode::kS2T2;
  if (mode_string == "S2T2h")
    return ScalabilityMode::kS2T2h;
  if (mode_string == "S2T3")
    return ScalabilityMode::kS2T3;
  if (mode_string == "S2T3h")
    return ScalabilityMode::kS2T3h;
  if (mode_string == "S3T1")
    return ScalabilityMode::kS3T1;
  if (mode_string == "S3T1h")
    return ScalabilityMode::kS3T1h;
  if (mode_string == "S3T2")
    return ScalabilityMode::kS3T2;
  if (mode_string == "S3T2h")
    return ScalabilityMode::kS3T2h;
  if (mode_string == "S3T3")
    return ScalabilityMode::kS3T3;
  if (mode_string == "S3T3h")
    return ScalabilityMode::kS3T3h;

  return absl::nullopt;
}

InterLayerPredMode ScalabilityModeToInterLayerPredMode(
    ScalabilityMode scalability_mode) {
  switch (scalability_mode) {
    case ScalabilityMode::kL1T1:
    case ScalabilityMode::kL1T2:
    case ScalabilityMode::kL1T3:
    case ScalabilityMode::kL2T1:
    case ScalabilityMode::kL2T1h:
      return InterLayerPredMode::kOn;
    case ScalabilityMode::kL2T1_KEY:
      return InterLayerPredMode::kOnKeyPic;
    case ScalabilityMode::kL2T2:
    case ScalabilityMode::kL2T2h:
      return InterLayerPredMode::kOn;
    case ScalabilityMode::kL2T2_KEY:
    case ScalabilityMode::kL2T2_KEY_SHIFT:
      return InterLayerPredMode::kOnKeyPic;
    case ScalabilityMode::kL2T3:
    case ScalabilityMode::kL2T3h:
      return InterLayerPredMode::kOn;
    case ScalabilityMode::kL2T3_KEY:
      return InterLayerPredMode::kOnKeyPic;
    case ScalabilityMode::kL3T1:
    case ScalabilityMode::kL3T1h:
      return InterLayerPredMode::kOn;
    case ScalabilityMode::kL3T1_KEY:
      return InterLayerPredMode::kOnKeyPic;
    case ScalabilityMode::kL3T2:
    case ScalabilityMode::kL3T2h:
      return InterLayerPredMode::kOn;
    case ScalabilityMode::kL3T2_KEY:
      return InterLayerPredMode::kOnKeyPic;
    case ScalabilityMode::kL3T3:
    case ScalabilityMode::kL3T3h:
      return InterLayerPredMode::kOn;
    case ScalabilityMode::kL3T3_KEY:
      return InterLayerPredMode::kOnKeyPic;
    case ScalabilityMode::kS2T1:
    case ScalabilityMode::kS2T1h:
    case ScalabilityMode::kS2T2:
    case ScalabilityMode::kS2T2h:
    case ScalabilityMode::kS2T3:
    case ScalabilityMode::kS2T3h:
    case ScalabilityMode::kS3T1:
    case ScalabilityMode::kS3T1h:
    case ScalabilityMode::kS3T2:
    case ScalabilityMode::kS3T2h:
    case ScalabilityMode::kS3T3:
    case ScalabilityMode::kS3T3h:
      return InterLayerPredMode::kOff;
  }
  RTC_CHECK_NOTREACHED();
}

int ScalabilityModeToNumSpatialLayers(ScalabilityMode scalability_mode) {
  switch (scalability_mode) {
    case ScalabilityMode::kL1T1:
    case ScalabilityMode::kL1T2:
    case ScalabilityMode::kL1T3:
      return 1;
    case ScalabilityMode::kL2T1:
    case ScalabilityMode::kL2T1h:
    case ScalabilityMode::kL2T1_KEY:
    case ScalabilityMode::kL2T2:
    case ScalabilityMode::kL2T2h:
    case ScalabilityMode::kL2T2_KEY:
    case ScalabilityMode::kL2T2_KEY_SHIFT:
    case ScalabilityMode::kL2T3:
    case ScalabilityMode::kL2T3h:
    case ScalabilityMode::kL2T3_KEY:
      return 2;
    case ScalabilityMode::kL3T1:
    case ScalabilityMode::kL3T1h:
    case ScalabilityMode::kL3T1_KEY:
    case ScalabilityMode::kL3T2:
    case ScalabilityMode::kL3T2h:
    case ScalabilityMode::kL3T2_KEY:
    case ScalabilityMode::kL3T3:
    case ScalabilityMode::kL3T3h:
    case ScalabilityMode::kL3T3_KEY:
      return 3;
    case ScalabilityMode::kS2T1:
    case ScalabilityMode::kS2T1h:
    case ScalabilityMode::kS2T2:
    case ScalabilityMode::kS2T2h:
    case ScalabilityMode::kS2T3:
    case ScalabilityMode::kS2T3h:
      return 2;
    case ScalabilityMode::kS3T1:
    case ScalabilityMode::kS3T1h:
    case ScalabilityMode::kS3T2:
    case ScalabilityMode::kS3T2h:
    case ScalabilityMode::kS3T3:
    case ScalabilityMode::kS3T3h:
      return 3;
  }
  RTC_CHECK_NOTREACHED();
}

int ScalabilityModeToNumTemporalLayers(ScalabilityMode scalability_mode) {
  switch (scalability_mode) {
    case ScalabilityMode::kL1T1:
      return 1;
    case ScalabilityMode::kL1T2:
      return 2;
    case ScalabilityMode::kL1T3:
      return 3;
    case ScalabilityMode::kL2T1:
    case ScalabilityMode::kL2T1h:
    case ScalabilityMode::kL2T1_KEY:
      return 1;
    case ScalabilityMode::kL2T2:
    case ScalabilityMode::kL2T2h:
    case ScalabilityMode::kL2T2_KEY:
    case ScalabilityMode::kL2T2_KEY_SHIFT:
      return 2;
    case ScalabilityMode::kL2T3:
    case ScalabilityMode::kL2T3h:
    case ScalabilityMode::kL2T3_KEY:
      return 3;
    case ScalabilityMode::kL3T1:
    case ScalabilityMode::kL3T1h:
    case ScalabilityMode::kL3T1_KEY:
      return 1;
    case ScalabilityMode::kL3T2:
    case ScalabilityMode::kL3T2h:
    case ScalabilityMode::kL3T2_KEY:
      return 2;
    case ScalabilityMode::kL3T3:
    case ScalabilityMode::kL3T3h:
    case ScalabilityMode::kL3T3_KEY:
      return 3;
    case ScalabilityMode::kS2T1:
    case ScalabilityMode::kS2T1h:
    case ScalabilityMode::kS3T1:
    case ScalabilityMode::kS3T1h:
      return 1;
    case ScalabilityMode::kS2T2:
    case ScalabilityMode::kS2T2h:
    case ScalabilityMode::kS3T2:
    case ScalabilityMode::kS3T2h:
      return 2;
    case ScalabilityMode::kS2T3:
    case ScalabilityMode::kS2T3h:
    case ScalabilityMode::kS3T3:
    case ScalabilityMode::kS3T3h:
      return 3;
  }
  RTC_CHECK_NOTREACHED();
}

absl::optional<ScalabilityModeResolutionRatio> ScalabilityModeToResolutionRatio(
    ScalabilityMode scalability_mode) {
  switch (scalability_mode) {
    case ScalabilityMode::kL1T1:
    case ScalabilityMode::kL1T2:
    case ScalabilityMode::kL1T3:
      return absl::nullopt;
    case ScalabilityMode::kL2T1:
    case ScalabilityMode::kL2T1_KEY:
    case ScalabilityMode::kL2T2:
    case ScalabilityMode::kL2T2_KEY:
    case ScalabilityMode::kL2T2_KEY_SHIFT:
    case ScalabilityMode::kL2T3:
    case ScalabilityMode::kL2T3_KEY:
    case ScalabilityMode::kL3T1:
    case ScalabilityMode::kL3T1_KEY:
    case ScalabilityMode::kL3T2:
    case ScalabilityMode::kL3T2_KEY:
    case ScalabilityMode::kL3T3:
    case ScalabilityMode::kL3T3_KEY:
    case ScalabilityMode::kS2T1:
    case ScalabilityMode::kS2T2:
    case ScalabilityMode::kS2T3:
    case ScalabilityMode::kS3T1:
    case ScalabilityMode::kS3T2:
    case ScalabilityMode::kS3T3:
      return ScalabilityModeResolutionRatio::kTwoToOne;
    case ScalabilityMode::kL2T1h:
    case ScalabilityMode::kL2T2h:
    case ScalabilityMode::kL2T3h:
    case ScalabilityMode::kL3T1h:
    case ScalabilityMode::kL3T2h:
    case ScalabilityMode::kL3T3h:
    case ScalabilityMode::kS2T1h:
    case ScalabilityMode::kS2T2h:
    case ScalabilityMode::kS2T3h:
    case ScalabilityMode::kS3T1h:
    case ScalabilityMode::kS3T2h:
    case ScalabilityMode::kS3T3h:
      return ScalabilityModeResolutionRatio::kThreeToTwo;
  }
  RTC_CHECK_NOTREACHED();
}

ScalabilityMode LimitNumSpatialLayers(ScalabilityMode scalability_mode,
                                      int max_spatial_layers) {
  int num_spatial_layers = ScalabilityModeToNumSpatialLayers(scalability_mode);
  if (max_spatial_layers >= num_spatial_layers) {
    return scalability_mode;
  }

  switch (scalability_mode) {
    case ScalabilityMode::kL1T1:
      return ScalabilityMode::kL1T1;
    case ScalabilityMode::kL1T2:
      return ScalabilityMode::kL1T2;
    case ScalabilityMode::kL1T3:
      return ScalabilityMode::kL1T3;
    case ScalabilityMode::kL2T1:
      return ScalabilityMode::kL1T1;
    case ScalabilityMode::kL2T1h:
      return ScalabilityMode::kL1T1;
    case ScalabilityMode::kL2T1_KEY:
      return ScalabilityMode::kL1T1;
    case ScalabilityMode::kL2T2:
      return ScalabilityMode::kL1T2;
    case ScalabilityMode::kL2T2h:
      return ScalabilityMode::kL1T2;
    case ScalabilityMode::kL2T2_KEY:
      return ScalabilityMode::kL1T2;
    case ScalabilityMode::kL2T2_KEY_SHIFT:
      return ScalabilityMode::kL1T2;
    case ScalabilityMode::kL2T3:
      return ScalabilityMode::kL1T3;
    case ScalabilityMode::kL2T3h:
      return ScalabilityMode::kL1T3;
    case ScalabilityMode::kL2T3_KEY:
      return ScalabilityMode::kL1T3;
    case ScalabilityMode::kL3T1:
      return max_spatial_layers == 2 ? ScalabilityMode::kL2T1
                                     : ScalabilityMode::kL1T1;
    case ScalabilityMode::kL3T1h:
      return max_spatial_layers == 2 ? ScalabilityMode::kL2T1h
                                     : ScalabilityMode::kL1T1;
    case ScalabilityMode::kL3T1_KEY:
      return max_spatial_layers == 2 ? ScalabilityMode::kL2T1_KEY
                                     : ScalabilityMode::kL1T1;
    case ScalabilityMode::kL3T2:
      return max_spatial_layers == 2 ? ScalabilityMode::kL2T2
                                     : ScalabilityMode::kL1T2;
    case ScalabilityMode::kL3T2h:
      return max_spatial_layers == 2 ? ScalabilityMode::kL2T2h
                                     : ScalabilityMode::kL1T2;
    case ScalabilityMode::kL3T2_KEY:
      return max_spatial_layers == 2 ? ScalabilityMode::kL2T2_KEY
                                     : ScalabilityMode::kL1T2;
    case ScalabilityMode::kL3T3:
      return max_spatial_layers == 2 ? ScalabilityMode::kL2T3
                                     : ScalabilityMode::kL1T3;
    case ScalabilityMode::kL3T3h:
      return max_spatial_layers == 2 ? ScalabilityMode::kL2T3h
                                     : ScalabilityMode::kL1T3;
    case ScalabilityMode::kL3T3_KEY:
      return max_spatial_layers == 2 ? ScalabilityMode::kL2T3_KEY
                                     : ScalabilityMode::kL1T3;
    case ScalabilityMode::kS2T1:
      return ScalabilityMode::kL1T1;
    case ScalabilityMode::kS2T1h:
      return ScalabilityMode::kL1T1;
    case ScalabilityMode::kS2T2:
      return ScalabilityMode::kL1T2;
    case ScalabilityMode::kS2T2h:
      return ScalabilityMode::kL1T2;
    case ScalabilityMode::kS2T3:
      return ScalabilityMode::kL1T3;
    case ScalabilityMode::kS2T3h:
      return ScalabilityMode::kL1T3;
    case ScalabilityMode::kS3T1:
      return max_spatial_layers == 2 ? ScalabilityMode::kS2T1
                                     : ScalabilityMode::kL1T1;
    case ScalabilityMode::kS3T1h:
      return max_spatial_layers == 2 ? ScalabilityMode::kS2T1h
                                     : ScalabilityMode::kL1T1;
    case ScalabilityMode::kS3T2:
      return max_spatial_layers == 2 ? ScalabilityMode::kS2T2
                                     : ScalabilityMode::kL1T2;
    case ScalabilityMode::kS3T2h:
      return max_spatial_layers == 2 ? ScalabilityMode::kS2T2h
                                     : ScalabilityMode::kL1T2;
    case ScalabilityMode::kS3T3:
      return max_spatial_layers == 2 ? ScalabilityMode::kS2T3
                                     : ScalabilityMode::kL1T3;
    case ScalabilityMode::kS3T3h:
      return max_spatial_layers == 2 ? ScalabilityMode::kS2T3h
                                     : ScalabilityMode::kL1T3;
  }
  RTC_CHECK_NOTREACHED();
}

bool ScalabilityModeIsShiftMode(ScalabilityMode scalability_mode) {
  return scalability_mode == ScalabilityMode::kL2T2_KEY_SHIFT;
}

}  // namespace webrtc
