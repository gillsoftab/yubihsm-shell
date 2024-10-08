#include <assert.h>
#include "debug_lib.h"

extern "C" {
#include "yubihsm.h"

uint8_t *backend_data;
size_t backend_data_len;
}

#include "yubihsm_fuzz.h"

yh_connector *connector;

static bool initialize() {
  yh_rc rc = yh_init_connector("yhfuzz://yubihsm_fuzz", &connector);
  assert(rc == YHR_SUCCESS);
  rc = yh_connect(connector, 0);
  assert(rc == YHR_SUCCESS);
  return true;
}

extern "C" int LLVMFuzzerTestOneInput(uint8_t *data, size_t size) {
  static bool is_initialized = initialize();

  if (size < 2) {
    return 0;
  }
  size_t data_len = data[0];
  size_t response_len = data[1];

  backend_data = data + 2;
  backend_data_len = size - 2;

  uint8_t *hsm_data = new uint8_t[data_len];
  uint8_t *response = new uint8_t[response_len];
  yh_cmd response_cmd;

  yh_send_plain_msg(connector, YHC_ECHO, hsm_data, data_len, &response_cmd,
                    response, &response_len);

  delete[] hsm_data;
  delete[] response;

  return 0;
}