#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

#include "micro_model_settings.h"
#include "models/micro_speech_quantized_model_data.h"
#include "models/audio_preprocessor_int8_model_data.h"

#include "printf.h"

namespace {
// Arena size is a guesstimate, followed by use of
// MicroInterpreter::arena_used_bytes() on both the AudioPreprocessor and
// MicroSpeech models and using the larger of the two results.
constexpr size_t kArenaSize = 9464;
alignas(16) uint8_t g_arena[kArenaSize];

using Features = int8_t[kFeatureCount][kFeatureSize];
Features g_features;

constexpr int kAudioSampleDurationCount = kFeatureDurationMs * kAudioSampleFrequency / 1000;
constexpr int kAudioSampleStrideCount = kFeatureStrideMs * kAudioSampleFrequency / 1000;

using MicroSpeechOpResolver = tflite::MicroMutableOpResolver<4>;
using AudioPreprocessorOpResolver = tflite::MicroMutableOpResolver<18>;

TfLiteStatus RegisterOps(MicroSpeechOpResolver &op_resolver) {
  TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
  TF_LITE_ENSURE_STATUS(op_resolver.AddDepthwiseConv2D());
  TF_LITE_ENSURE_STATUS(op_resolver.AddSoftmax());
  return kTfLiteOk;
}

TfLiteStatus RegisterOps(AudioPreprocessorOpResolver &op_resolver) {
  TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
  TF_LITE_ENSURE_STATUS(op_resolver.AddCast());
  TF_LITE_ENSURE_STATUS(op_resolver.AddStridedSlice());
  TF_LITE_ENSURE_STATUS(op_resolver.AddConcatenation());
  TF_LITE_ENSURE_STATUS(op_resolver.AddMul());
  TF_LITE_ENSURE_STATUS(op_resolver.AddAdd());
  TF_LITE_ENSURE_STATUS(op_resolver.AddDiv());
  TF_LITE_ENSURE_STATUS(op_resolver.AddMinimum());
  TF_LITE_ENSURE_STATUS(op_resolver.AddMaximum());
  TF_LITE_ENSURE_STATUS(op_resolver.AddWindow());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFftAutoScale());
  TF_LITE_ENSURE_STATUS(op_resolver.AddRfft());
  TF_LITE_ENSURE_STATUS(op_resolver.AddEnergy());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFilterBank());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFilterBankSquareRoot());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFilterBankSpectralSubtraction());
  TF_LITE_ENSURE_STATUS(op_resolver.AddPCAN());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFilterBankLog());
  return kTfLiteOk;
}
}  // namespace

TfLiteStatus LoadMicroSpeechModelAndPerformInference(struct ClassificationResult &result, const Features &features) {
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model *model = tflite::GetModel(g_micro_speech_quantized_model_data);
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

  MicroSpeechOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  tflite::MicroInterpreter interpreter(model, op_resolver, g_arena, kArenaSize);

  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

  TfLiteTensor *input = interpreter.input(0);
  TFLITE_CHECK_NE(input, nullptr);

  TfLiteTensor *output = interpreter.output(0);
  TFLITE_CHECK_NE(output, nullptr);

  float output_scale = output->params.scale;
  int output_zero_point = output->params.zero_point;

  std::copy_n(&features[0][0], kFeatureElementCount, tflite::GetTensorData<int8_t>(input));
  TF_LITE_ENSURE_STATUS(interpreter.Invoke());

  // Dequantize and find category with maximum probability
  float category_predictions[kCategoryCount];

  int cat_idx_max = 0;
  for (int i = 0; i < kCategoryCount; i++) {
    category_predictions[i] = (tflite::GetTensorData<int8_t>(output)[i] - output_zero_point) * output_scale;
    if (category_predictions[i] > category_predictions[cat_idx_max])
      cat_idx_max = i;
  }

  result.category_idx = cat_idx_max;
  result.probability = category_predictions[cat_idx_max];

  return kTfLiteOk;
}

TfLiteStatus GenerateSingleFeature(const int16_t *audio_data, const int audio_data_size, int8_t *feature_output,
                                   tflite::MicroInterpreter *interpreter) {
  TfLiteTensor *input = interpreter->input(0);
  TFLITE_CHECK_NE(input, nullptr);

  TfLiteTensor *output = interpreter->output(0);
  TFLITE_CHECK_NE(output, nullptr);

  std::copy_n(audio_data, audio_data_size, tflite::GetTensorData<int16_t>(input));
  TF_LITE_ENSURE_STATUS(interpreter->Invoke());

  std::copy_n(tflite::GetTensorData<int8_t>(output), kFeatureSize, feature_output);

  return kTfLiteOk;
}

TfLiteStatus GenerateFeatures(const int16_t *audio_data, const size_t audio_data_size, Features *features_output) {
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model *model = tflite::GetModel(g_audio_preprocessor_int8_model_data);
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

  AudioPreprocessorOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  tflite::MicroInterpreter interpreter(model, op_resolver, g_arena, kArenaSize);
  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

  size_t remaining_samples = audio_data_size;
  size_t feature_index = 0;
  while (remaining_samples >= kAudioSampleDurationCount && feature_index < kFeatureCount) {
    TF_LITE_ENSURE_STATUS(
        GenerateSingleFeature(audio_data, kAudioSampleDurationCount, (*features_output)[feature_index], &interpreter));
    feature_index++;
    audio_data += kAudioSampleStrideCount;
    remaining_samples -= kAudioSampleStrideCount;
  }

  return kTfLiteOk;
}

TfLiteStatus Classify(struct ClassificationResult &result, const int16_t *audio_data, const size_t audio_data_size) {
  TF_LITE_ENSURE_STATUS(GenerateFeatures(audio_data, audio_data_size, &g_features));
  TF_LITE_ENSURE_STATUS(LoadMicroSpeechModelAndPerformInference(result, g_features));
  return kTfLiteOk;
}