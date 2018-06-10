#include <esp_bt_main.h>
#include <mruby.h>
#include <mruby-esp32-ext_esp_err.h>

static mrb_value mrb_esp32_bluedroid_get_status(mrb_state *mrb, mrb_value self) {
  return mrb_fixnum_value((mrb_int)esp_bluedroid_get_status());
}

static mrb_value  mrb_esp32_bluedroid_enable(mrb_state *mrb, mrb_value self) {
  esp_err_t err = esp_bluedroid_enable();
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }
  return mrb_nil_value();
}

static mrb_value  mrb_esp32_bluedroid_disable(mrb_state *mrb, mrb_value self) {
  esp_err_t err = esp_bluedroid_disable();
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }
  return mrb_nil_value();
}

static mrb_value  mrb_esp32_bluedroid_init(mrb_state *mrb, mrb_value self) {
  esp_err_t err = esp_bluedroid_init();
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }
  return mrb_nil_value();
}

static mrb_value  mrb_esp32_bluedroid_deinit(mrb_state *mrb, mrb_value self) {
  esp_err_t err = esp_bluedroid_deinit();
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }
  return mrb_nil_value();
}

void mrb_esp_bt_main_init(mrb_state *mrb, struct RClass *mrb_esp32_bluedroid) {
  mrb_define_class_method(mrb, mrb_esp32_bluedroid, "get_status", mrb_esp32_bluedroid_get_status, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_bluedroid, "enable", mrb_esp32_bluedroid_enable, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_bluedroid, "disable", mrb_esp32_bluedroid_disable, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_bluedroid, "init", mrb_esp32_bluedroid_init, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_bluedroid, "deinit", mrb_esp32_bluedroid_deinit, MRB_ARGS_NONE());

  /* esp_bluedroid_status_t */
  mrb_define_const(mrb, mrb_esp32_bluedroid, "STATUS_UNINITIALIZED", mrb_fixnum_value(ESP_BLUEDROID_STATUS_UNINITIALIZED));
  mrb_define_const(mrb, mrb_esp32_bluedroid, "STATUS_INITIALIZED", mrb_fixnum_value(ESP_BLUEDROID_STATUS_INITIALIZED));
  mrb_define_const(mrb, mrb_esp32_bluedroid, "STATUS_ENABLED", mrb_fixnum_value(ESP_BLUEDROID_STATUS_ENABLED));
}
