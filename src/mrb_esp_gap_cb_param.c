#include <esp_gap_ble_api.h>
#include <mruby.h>
#include <mruby/data.h>
#include <mruby-esp32-ext_esp_err.h>

extern const struct mrb_data_type mrb_esp32_ble_gap_cb_param_type;

static mrb_value mrb_esp32_scan_start_cmpl_success_p(mrb_state *mrb, mrb_value self) {
  esp_ble_gap_cb_param_t *param = (esp_ble_gap_cb_param_t *)mrb_data_get_ptr(mrb, self, &mrb_esp32_ble_gap_cb_param_type);
  if (param->scan_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
    return mrb_true_value();
  } else {
    return mrb_false_value();
  }
}

static mrb_value mrb_esp32_scan_start_cmpl_status(mrb_state *mrb, mrb_value self) {
  esp_ble_gap_cb_param_t *param = (esp_ble_gap_cb_param_t *)mrb_data_get_ptr(mrb, self, &mrb_esp32_ble_gap_cb_param_type);
  return mrb_fixnum_value(param->scan_start_cmpl.status);
}

static void scan_start_cmpl_init(mrb_state *mrb, struct RClass *scan_start_cmpl) {
  mrb_define_method(mrb, scan_start_cmpl, "success?", mrb_esp32_scan_start_cmpl_success_p, MRB_ARGS_NONE());
  mrb_define_method(mrb, scan_start_cmpl, "status", mrb_esp32_scan_start_cmpl_status, MRB_ARGS_NONE());
}

void mrb_esp_gap_cb_param_init(mrb_state *mrb, struct RClass *mrb_esp32_ble_gap) {
  scan_start_cmpl_init(mrb, mrb_class_get_under(mrb, mrb_esp32_ble_gap, "ScanStartCmpl"));
}
