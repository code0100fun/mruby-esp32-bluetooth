#include <esp_gatt_common_api.h>
#include <mruby.h>
#include <mruby-esp32-ext_esp_err.h>

static mrb_value mrb_esp_ble_gatt_set_local_mtu(mrb_state *mrb, mrb_value self) {
  mrb_int mtu;
  mrb_get_args(mrb, "i", &mtu);

  esp_err_t err = esp_ble_gatt_set_local_mtu((uint16_t)mtu);
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }
  return mrb_nil_value();
};

void mrb_esp_gatt_common_api_init(mrb_state *mrb, struct RClass *mrb_esp32_ble_gatt) {
  mrb_define_class_method(mrb, mrb_esp32_ble_gatt, "set_local_mtu", mrb_esp_ble_gatt_set_local_mtu, MRB_ARGS_REQ(1));
}
