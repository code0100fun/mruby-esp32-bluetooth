#include <esp_gattc_api.h>
#include <mruby.h>
#include <mruby-esp32-ext_esp_err.h>

static mrb_state *mrb_esp32_bluetooth_mrb;
static mrb_value gattc_callback_block;


void mrb_esp32_ble_gattc_callback(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
  if (mrb_nil_p(gattc_callback_block)) { return; }

  mrb_state *mrb = mrb_esp32_bluetooth_mrb;
  struct RClass *mrb_esp32_ble_gattc = mrb_class_get_under(mrb, mrb_class_get_under(mrb, mrb_module_get(mrb, "ESP32"), "BLE"), "GATTC");
}


static mrb_value mrb_esp32_ble_gattc_s_register_callback(mrb_state *mrb, mrb_value self) {
  mrb_value block;
  mrb_get_args(mrb, "&", &block);

  gattc_callback_block = block;
  mrb_gc_register(mrb, block);

  return mrb_nil_value();
}

static mrb_value mrb_esp32_ble_gattc_s_app_register(mrb_state *mrb, mrb_value self) {
  mrb_int app_id;
  mrb_get_args(mrb, "i", &app_id);

  esp_err_t err = esp_ble_gattc_app_register((uint16_t)app_id);
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }

  return mrb_nil_value();
}

static mrb_value mrb_esp32_ble_gattc_s_app_unregister(mrb_state *mrb, mrb_value self) {
  mrb_int gattc_if;
  mrb_get_args(mrb, "i", &gattc_if);

  esp_err_t err = esp_ble_gattc_app_unregister((esp_gatt_if_t)gattc_if);
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }

  return mrb_nil_value();
}

void mrb_esp_gap_gattc_init(mrb_state *mrb, struct RClass *mrb_esp32, struct RClass *mrb_esp32_ble_gattc) {
  mrb_esp32_bluetooth_mrb = mrb;

  esp_ble_gattc_register_callback(mrb_esp32_ble_gattc_callback);

  mrb_define_class_method(mrb, mrb_esp32_ble_gattc, "register_callback", mrb_esp32_ble_gattc_s_register_callback, MRB_ARGS_BLOCK());
  mrb_define_class_method(mrb, mrb_esp32_ble_gattc, "app_register", mrb_esp32_ble_gattc_s_app_register, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, mrb_esp32_ble_gattc, "app_unregister", mrb_esp32_ble_gattc_s_app_unregister, MRB_ARGS_REQ(1));

  /* esp_gattc_cb_event_t */
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_REG_EVT"                 , mrb_fixnum_value(ESP_GATTC_REG_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_UNREG_EVT"               , mrb_fixnum_value(ESP_GATTC_UNREG_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_OPEN_EVT"                , mrb_fixnum_value(ESP_GATTC_OPEN_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_READ_CHAR_EVT"           , mrb_fixnum_value(ESP_GATTC_READ_CHAR_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_WRITE_CHAR_EVT"          , mrb_fixnum_value(ESP_GATTC_WRITE_CHAR_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_CLOSE_EVT"               , mrb_fixnum_value(ESP_GATTC_CLOSE_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_SEARCH_CMPL_EVT"         , mrb_fixnum_value(ESP_GATTC_SEARCH_CMPL_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_SEARCH_RES_EVT"          , mrb_fixnum_value(ESP_GATTC_SEARCH_RES_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_READ_DESCR_EVT"          , mrb_fixnum_value(ESP_GATTC_READ_DESCR_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_WRITE_DESCR_EVT"         , mrb_fixnum_value(ESP_GATTC_WRITE_DESCR_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_NOTIFY_EVT"              , mrb_fixnum_value(ESP_GATTC_NOTIFY_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_PREP_WRITE_EVT"          , mrb_fixnum_value(ESP_GATTC_PREP_WRITE_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_EXEC_EVT"                , mrb_fixnum_value(ESP_GATTC_EXEC_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_ACL_EVT"                 , mrb_fixnum_value(ESP_GATTC_ACL_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_CANCEL_OPEN_EVT"         , mrb_fixnum_value(ESP_GATTC_CANCEL_OPEN_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_SRVC_CHG_EVT"            , mrb_fixnum_value(ESP_GATTC_SRVC_CHG_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_ENC_CMPL_CB_EVT"         , mrb_fixnum_value(ESP_GATTC_ENC_CMPL_CB_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_CFG_MTU_EVT"             , mrb_fixnum_value(ESP_GATTC_CFG_MTU_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_ADV_DATA_EVT"            , mrb_fixnum_value(ESP_GATTC_ADV_DATA_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_MULT_ADV_ENB_EVT"        , mrb_fixnum_value(ESP_GATTC_MULT_ADV_ENB_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_MULT_ADV_UPD_EVT"        , mrb_fixnum_value(ESP_GATTC_MULT_ADV_UPD_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_MULT_ADV_DATA_EVT"       , mrb_fixnum_value(ESP_GATTC_MULT_ADV_DATA_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_MULT_ADV_DIS_EVT"        , mrb_fixnum_value(ESP_GATTC_MULT_ADV_DIS_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_CONGEST_EVT"             , mrb_fixnum_value(ESP_GATTC_CONGEST_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_BTH_SCAN_ENB_EVT"        , mrb_fixnum_value(ESP_GATTC_BTH_SCAN_ENB_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_BTH_SCAN_CFG_EVT"        , mrb_fixnum_value(ESP_GATTC_BTH_SCAN_CFG_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_BTH_SCAN_RD_EVT"         , mrb_fixnum_value(ESP_GATTC_BTH_SCAN_RD_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_BTH_SCAN_THR_EVT"        , mrb_fixnum_value(ESP_GATTC_BTH_SCAN_THR_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_BTH_SCAN_PARAM_EVT"      , mrb_fixnum_value(ESP_GATTC_BTH_SCAN_PARAM_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_BTH_SCAN_DIS_EVT"        , mrb_fixnum_value(ESP_GATTC_BTH_SCAN_DIS_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_SCAN_FLT_CFG_EVT"        , mrb_fixnum_value(ESP_GATTC_SCAN_FLT_CFG_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_SCAN_FLT_PARAM_EVT"      , mrb_fixnum_value(ESP_GATTC_SCAN_FLT_PARAM_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_SCAN_FLT_STATUS_EVT"     , mrb_fixnum_value(ESP_GATTC_SCAN_FLT_STATUS_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_ADV_VSC_EVT"             , mrb_fixnum_value(ESP_GATTC_ADV_VSC_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_REG_FOR_NOTIFY_EVT"      , mrb_fixnum_value(ESP_GATTC_REG_FOR_NOTIFY_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_UNREG_FOR_NOTIFY_EVT"    , mrb_fixnum_value(ESP_GATTC_UNREG_FOR_NOTIFY_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_CONNECT_EVT"             , mrb_fixnum_value(ESP_GATTC_CONNECT_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_DISCONNECT_EVT"          , mrb_fixnum_value(ESP_GATTC_DISCONNECT_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_READ_MULTIPLE_EVT"       , mrb_fixnum_value(ESP_GATTC_READ_MULTIPLE_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_QUEUE_FULL_EVT"          , mrb_fixnum_value(ESP_GATTC_QUEUE_FULL_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_SET_ASSOC_EVT"           , mrb_fixnum_value(ESP_GATTC_SET_ASSOC_EVT));
  mrb_define_const(mrb, mrb_esp32, "ESP_GATTC_GET_ADDR_LIST_EVT"       , mrb_fixnum_value(ESP_GATTC_GET_ADDR_LIST_EVT));
}
