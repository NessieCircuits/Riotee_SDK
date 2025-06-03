#include "models/hello_world_float_model_data.h"
#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/recording_micro_allocator.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"

#include "printf.h"
#include "riotee.h"
#include "riotee_gpio.h"
#include "riotee_timing.h"
#include "riotee_uart.h"

namespace {
using HelloWorldOpResolver = tflite::MicroMutableOpResolver<1>;

TfLiteStatus RegisterOps(HelloWorldOpResolver &op_resolver) {
  TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
  return kTfLiteOk;
}
}  // namespace

void lateinit(void) {
  riotee_gpio_cfg_output(PIN_LED_CTRL);
}

// Dummy implementation for _sbrk
void *_sbrk(intptr_t increment) {
  errno = ENOSYS;  // Not implemented
  return (void *)-1;
}

int main() {
  const tflite::Model *model = ::tflite::GetModel(g_hello_world_float_model_data);

  HelloWorldOpResolver op_resolver;
  RegisterOps(op_resolver);

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 3000;
  uint8_t tensor_arena[kTensorArenaSize];

  tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena, kTensorArenaSize);
  interpreter.AllocateTensors();

  while (1) {
    for (int i = 0; i < 100; ++i) {
      float x = 2 * 3.14159 * i / 100;
      interpreter.input(0)->data.f[0] = x;
      interpreter.Invoke();
      float y_pred = interpreter.output(0)->data.f[0];
      float err = fabs(sin(x) - y_pred);
      printf("x: %.2f -> y: %.2f (e=%.2f)\r\n", x, y_pred, err);
    }
  }
}
