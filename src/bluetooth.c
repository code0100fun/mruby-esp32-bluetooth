#include <mruby.h>

void mrb_esp_bt_init(mrb_state *mrb, struct RClass *mrb_esp32, struct RClass *mrb_esp32_bt, struct RClass *mrb_esp32_ble, struct RClass *mrb_esp32_bredr, struct RClass *mrb_esp32_bt_controller, struct RClass *mrb_esp32_vhci_host, struct RClass *mrb_esp32_bt_controller_config);
void mrb_esp_bt_main_init(mrb_state *mrb, struct RClass *mrb_esp32, struct RClass *mrb_esp32_bluedroid);
void mrb_esp_gap_ble_api_init(mrb_state *mrb, struct RClass *mrb_esp32, struct RClass *mrb_esp32_ble, struct RClass *mrb_esp32_ble_gap);
void mrb_esp_gap_gattc_init(mrb_state *mrb, struct RClass *mrb_esp32, struct RClass *mrb_esp32_ble_gattc);
void mrb_esp_gatt_common_api_init(mrb_state *mrb, struct RClass *mrb_esp32_ble_gatt);

void mrb_esp32_bluetooth_gem_init(mrb_state* mrb) {
  struct RClass *mrb_esp32 = mrb_define_module(mrb, "ESP32");

  // esp_bt.h
  struct RClass *mrb_esp32_bt = mrb_define_class_under(mrb, mrb_esp32, "BT", mrb->object_class);
  struct RClass *mrb_esp32_ble = mrb_define_class_under(mrb, mrb_esp32, "BLE", mrb->object_class);
  struct RClass *mrb_esp32_bredr = mrb_define_class_under(mrb, mrb_esp32, "BREDR", mrb->object_class);
  struct RClass *mrb_esp32_bt_controller = mrb_define_class_under(mrb, mrb_esp32_bt, "Controller", mrb->object_class);
  struct RClass *mrb_esp32_vhci_host = mrb_define_class_under(mrb, mrb_esp32, "VHCIHost", mrb->object_class);
  struct RClass *mrb_esp32_bt_controller_config = mrb_define_class_under(mrb, mrb_esp32_bt, "ControllerConfig", mrb->object_class);
  mrb_esp_bt_init(mrb, mrb_esp32, mrb_esp32_bt, mrb_esp32_ble, mrb_esp32_bredr, mrb_esp32_bt_controller, mrb_esp32_vhci_host, mrb_esp32_bt_controller_config);

  // esp_bt_main.h
  struct RClass *mrb_esp32_bluedroid = mrb_define_class_under(mrb, mrb_esp32, "Bluedroid", mrb->object_class);
  mrb_esp_bt_main_init(mrb, mrb_esp32, mrb_esp32_bluedroid);

  // esp_gap_ble_api.h
  struct RClass *mrb_esp32_ble_gap = mrb_define_class_under(mrb, mrb_esp32_ble, "GAP", mrb->object_class);
  mrb_esp_gap_ble_api_init(mrb, mrb_esp32, mrb_esp32_ble, mrb_esp32_ble_gap);


  // esp_gap_gattc.h
  struct RClass *mrb_esp32_ble_gattc = mrb_define_class_under(mrb, mrb_esp32_ble, "GATTC", mrb->object_class);
  mrb_esp_gap_gattc_init(mrb, mrb_esp32, mrb_esp32_ble_gattc);

  // esp_gatt_common_api.h
  struct RClass *mrb_esp32_ble_gatt = mrb_define_class_under(mrb, mrb_esp32_ble, "GATT", mrb->object_class);
  mrb_esp_gatt_common_api_init(mrb, mrb_esp32_ble_gatt);
}

void mrb_esp32_bluetooth_gem_final(mrb_state* mrb) {
}
