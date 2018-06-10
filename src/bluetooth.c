#include <esp_bt.h>
#include <esp_gap_ble_api.h>
#include <freertos/FreeRTOS.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/data.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby-esp32-ext_esp_err.h>

void mrb_esp_bt_main_init(mrb_state *mrb, struct RClass *mrb_esp32_bluedroid);

static mrb_state *mrb_esp32_bluetooth_mrb;
static mrb_value notify_host_send_available_block;
static mrb_value notify_host_recv_block;
static mrb_value gap_callback_block;

void mrb_esp32_ble_gap_cb_free(mrb_state *mrb, void *param) {
  // TODO
}

static const struct mrb_data_type mrb_esp32_ble_gap_cb_param_type = {
  "esp_ble_gap_cb_param_t", mrb_esp32_ble_gap_cb_free
};

static const struct mrb_data_type mrb_esp32_bt_controller_config_type = {
  "esp_bt_controller_config_t", mrb_free
};

void mrb_esp32_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
  if (mrb_nil_p(gap_callback_block)) { return; }

  mrb_state *mrb = mrb_esp32_bluetooth_mrb;
  struct RClass *mrb_esp32_ble_gap = mrb_class_get_under(mrb, mrb_class_get_under(mrb, mrb_module_get(mrb, "ESP32"), "BLE"), "GAP");
  struct RClass *callback_param_klass;
  switch (event) {
  case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "AdvDataCmpl");
    break;
  case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "ScanRspDataCmpl");
    break;
  case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "ScanParamCmpl");
    break;
  case ESP_GAP_BLE_SCAN_RESULT_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "ScanRst");
    break;
  case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "AdvDataRawCmpl");
    break;
  case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "ScanRspDataRawCmpl");
    break;
  case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "AdvStartCmpl");
    break;
  case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "ScanStartCmpl");
    break;
  case ESP_GAP_BLE_AUTH_CMPL_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "AuthCmpl");
    break;
  case ESP_GAP_BLE_KEY_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "Key");
    break;
  case ESP_GAP_BLE_SEC_REQ_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "SecReq");
    break;
  case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "SecKeyNotif");
    break;
  case ESP_GAP_BLE_PASSKEY_REQ_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "SecReq");
    break;
  case ESP_GAP_BLE_OOB_REQ_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "SecReq");
    break;
  case ESP_GAP_BLE_LOCAL_IR_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "LocalIdKeys");
    break;
  case ESP_GAP_BLE_LOCAL_ER_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "LocalIdKeys");
    break;
  case ESP_GAP_BLE_NC_REQ_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "SecReq");
    break;
  case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "ScanStopCmpl");
    break;
  case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "AdvStopCmpl");
    break;
  case ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "SetRandAddrCmpl");
    break;
  case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "UpdateConnParams");
    break;
  case ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "PktDataLenthCmpl");
    break;
  case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "LocalPrivacyCmpl");
    break;
  case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "RemoveBondDevCmpl");
    break;
  case ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "ClearBondDevCmpl");
    break;
  case ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "GetBondDevCmpl");
    break;
  case ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "ReadRssiCmpl");
    break;
  case ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT:
    callback_param_klass = mrb_class_get_under(mrb, mrb_esp32_ble_gap, "UpdateWhitelistCmpl");
    break;
  case ESP_GAP_BLE_EVT_MAX:
    mrb_raisef(mrb, E_ARGUMENT_ERROR, "invalid callback event %S", mrb_fixnum_value(event));
    break;
  }

  mrb_value callback_param = mrb_obj_value(mrb_data_object_alloc(mrb, callback_param_klass, (void *)param, &mrb_esp32_ble_gap_cb_param_type));
  mrb_funcall(mrb, gap_callback_block, "call", 2, mrb_fixnum_value(event), callback_param);

  return;
}

void mrb_esp32_notify_host_send_available() {
  if (mrb_nil_p(notify_host_send_available_block)) { return; }

  mrb_funcall(mrb_esp32_bluetooth_mrb, notify_host_send_available_block, "call", 0);
}

int mrb_esp32_notify_host_recv(uint8_t *data, uint16_t len) {
  if (mrb_nil_p(notify_host_recv_block)) { return 0; }

  mrb_value d = mrb_str_new(mrb_esp32_bluetooth_mrb, (char *)data, (size_t)len);
  mrb_funcall(mrb_esp32_bluetooth_mrb, notify_host_recv_block, "call", 1, d);

  return 0;
}

static esp_vhci_host_callback_t mrb_esp32_vhci_host_callback = {
  mrb_esp32_notify_host_send_available, mrb_esp32_notify_host_recv
};

// esp_bt.h

/*
 * @example
 *   ESP32::BLE::tx_power_set(ESP32::BLE::PWR_TYPE_DEFAULT, ESP32::PWR_LVL_N2)
 */
static mrb_value mrb_esp32_ble_s_tx_power_set(mrb_state *mrb, mrb_value self) {
  mrb_int power_type, power_level;
  mrb_get_args(mrb, "ii", &power_type, &power_level);

  esp_err_t err = esp_ble_tx_power_set((esp_ble_power_type_t)power_type, (esp_power_level_t)power_level);
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }

  return mrb_nil_value();
}

/*
 * @example
 *   power_level = ESP32::BLE::tx_power_get(ESP32::BLE::PWR_TYPE_DEFAULT)
 */
static mrb_value mrb_esp32_ble_s_tx_power_get(mrb_state *mrb, mrb_value self) {
  mrb_int power_type;
  mrb_get_args(mrb, "i", &power_type);

  esp_power_level_t power_level = esp_ble_tx_power_get((esp_ble_power_type_t)power_type);
  if (power_level < 0) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "Invalid power level");
  }

  return mrb_fixnum_value((mrb_int)power_level);
}

/*
 * @example
 *   ESP32::BREDR::tx_power_set(ESP32::PWR_LVL_N2, ESP32::PWR_LVL_P1)
 */
static mrb_value mrb_esp32_bredr_s_tx_power_set(mrb_state *mrb, mrb_value self) {
  mrb_int min_power_level, max_power_level;
  mrb_get_args(mrb, "ii", &min_power_level, &max_power_level);

  esp_err_t err = esp_bredr_tx_power_set((esp_power_level_t)min_power_level, (esp_power_level_t)max_power_level);
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }

  return mrb_nil_value();
}


/*
 * @example
 *   min_power_level, max_power_level = ESP32::BREDR::tx_power_get
 */
static mrb_value mrb_esp32_bredr_s_tx_power_get(mrb_state *mrb, mrb_value self) {
  esp_power_level_t min_power_level, max_power_level;
  esp_err_t err = esp_bredr_tx_power_get(&min_power_level, &max_power_level);
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }

  mrb_value ary = mrb_ary_new_capa(mrb, 2);
  mrb_ary_set(mrb, ary, 0, mrb_fixnum_value((mrb_int)min_power_level));
  mrb_ary_set(mrb, ary, 1, mrb_fixnum_value((mrb_int)max_power_level));
  return ary;
}

/*
 * @example
 *   ESP32::BT::Controller.init
 *   # or
 *   ESP32::BT::Controller.init(controller_config)
 */
static mrb_value mrb_esp32_bt_controller_s_init(mrb_state *mrb, mrb_value self) {
  mrb_value controller_config;
  if (!mrb_get_args(mrb, "|o", &controller_config)) {
    controller_config = mrb_const_get(mrb, mrb_obj_value(mrb_class_get_under(mrb, mrb_class_get_under(mrb, mrb_module_get(mrb, "ESP32"), "BT"), "Controller")), mrb_intern_lit(mrb, "INIT_CONFIG_DEFAULT"));
  }

  esp_bt_controller_config_t *config = (esp_bt_controller_config_t *)DATA_PTR(controller_config);
  esp_err_t err = esp_bt_controller_init(config);
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }

  return mrb_nil_value();
}

/*
 * @example
 *   ESP32::BT::Controller.deinit
 */
static mrb_value mrb_esp32_bt_controller_s_deinit(mrb_state *mrb, mrb_value self) {
  esp_err_t err = esp_bt_controller_deinit();
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }

  return mrb_nil_value();
}

/*
 * @example
 *   ESP32::BT::Controller.enable(ESP32::BT::MODE_BLE)
 */
static mrb_value mrb_esp32_bt_controller_s_enable(mrb_state *mrb, mrb_value self) {
  mrb_int mode;
  mrb_get_args(mrb, "i", &mode);

  esp_err_t err = esp_bt_controller_enable((esp_bt_mode_t)mode);
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }

  return mrb_nil_value();
}

/*
 * @example
 *   ESP32::BT::Controller.disable
 */
static mrb_value mrb_esp32_bt_controller_s_disable(mrb_state *mrb, mrb_value self) {
  esp_err_t err = esp_bt_controller_disable();
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }

  return mrb_nil_value();
}

/*
 * @example
 *   ESP32::BT::Controller.get_status
 */
static mrb_value mrb_esp32_bt_controller_s_get_status(mrb_state *mrb, mrb_value self) {
  esp_bt_controller_status_t status = esp_bt_controller_get_status();
  return mrb_fixnum_value((mrb_int)status);
}

/*
 * @example
 *   ESP32::VHCIHost.check_send_available
 */
static mrb_value mrb_esp32_vhci_host_s_check_send_available(mrb_state *mrb, mrb_value self) {
  if (esp_vhci_host_check_send_available()) {
    return mrb_true_value();
  } else {
    return mrb_false_value();
  }
}

/*
 * @example
 *   ESP32::VHCIHost.send_packet(data)
 */
static mrb_value mrb_esp32_vhci_host_s_send_packet(mrb_state *mrb, mrb_value self) {
  char *data;
  mrb_int len;
  mrb_get_args(mrb, "s", &data, &len);

  esp_vhci_host_send_packet((uint8_t *)data, (uint16_t)len);

  return mrb_nil_value();
}

/*
 * @example
 *   ESP32::BT::Controller.mem_release(ESP32::BT::MODE_BLE)
 */
static mrb_value mrb_esp32_bt_controller_s_mem_release(mrb_state *mrb, mrb_value self) {
  mrb_int mode;
  mrb_get_args(mrb, "i", &mode);

  esp_err_t err = esp_bt_controller_mem_release((esp_bt_mode_t)mode);
  if (err != ESP_OK) {
    mrb_raise_esp32_err(mrb, err);
  }

  return mrb_nil_value();
}

/*
 * @example
 *   ESP32::VHCIHost.notify_host_send_available do
 *     # ...
 *   end
 */
static mrb_value mrb_esp32_vhci_host_s_notify_host_send_available(mrb_state *mrb, mrb_value self) {
  mrb_value block;
  mrb_get_args(mrb, "&", &block);

  notify_host_send_available_block = block;
  mrb_gc_register(mrb, block);

  return mrb_nil_value();
}

/*
 * @example
 *   ESP32::VHCIHost.notify_host_recv_block do
 *     # ...
 *   end
 */
static mrb_value mrb_esp32_vhci_host_s_notify_host_recv_block(mrb_state *mrb, mrb_value self) {
  mrb_value block;
  mrb_get_args(mrb, "&", &block);

  notify_host_recv_block = block;
  mrb_gc_register(mrb, block);

  return mrb_nil_value();
}

/*
 * @example
 *   ESP32::BT::ControllerConfig.new(controller_task_stack_size, controller_task_prio, hci_uart_no, hci_uart_baudrate)
 */
static mrb_value mrb_esp32_bt_controller_config_init(mrb_state *mrb, mrb_value self) {
  mrb_int controller_task_stack_size, controller_task_prio, hci_uart_no, hci_uart_baudrate;
  mrb_get_args(mrb, "iiii", &controller_task_stack_size, &controller_task_prio, &hci_uart_no, &hci_uart_baudrate);

  esp_bt_controller_config_t *config = (esp_bt_controller_config_t *)mrb_malloc(mrb, sizeof(esp_bt_controller_config_t));
  config->controller_task_stack_size = (uint16_t)controller_task_stack_size;
  config->controller_task_prio = (uint8_t)controller_task_prio;
  config->hci_uart_no = (uint8_t)hci_uart_no;
  config->hci_uart_baudrate = (uint32_t)hci_uart_baudrate;

  mrb_data_init(self, (void *)config, &mrb_esp32_bt_controller_config_type);

  return self;
}

// esp_gap_ble_api.h

static mrb_value mrb_esp32_gap_s_register_callback(mrb_state *mrb, mrb_value self) {
  mrb_value block;
  mrb_get_args(mrb, "&", &block);

  gap_callback_block = block;
  mrb_gc_register(mrb, block);

  return mrb_nil_value();
}

void mrb_esp32_bluetooth_gem_init(mrb_state* mrb) {
  mrb_esp32_bluetooth_mrb = mrb;
  notify_host_send_available_block = mrb_nil_value();
  notify_host_recv_block = mrb_nil_value();
  esp_vhci_host_register_callback(&mrb_esp32_vhci_host_callback);

  struct RClass *mrb_esp32 = mrb_define_module(mrb, "ESP32");

  // esp_bt_main.h
  struct RClass *mrb_esp32_bluedroid = mrb_define_class_under(mrb, mrb_esp32, "Bluedroid", mrb->object_class);
  mrb_esp_bt_main_init(mrb, mrb_esp32_bluedroid);

  // esp_bt.h

  /*
   * ESP-IDF Bluetooth API wrapper.
   *
   * c.f. {http://esp-idf.readthedocs.io/en/latest/api-reference/bluetooth/index.html Bluetooth API documentation}
   */
  struct RClass *mrb_esp32_bt = mrb_define_class_under(mrb, mrb_esp32, "BT", mrb->object_class);

  struct RClass *mrb_esp32_ble = mrb_define_class_under(mrb, mrb_esp32, "BLE", mrb->object_class);
  mrb_define_class_method(mrb, mrb_esp32_ble, "tx_power_set", mrb_esp32_ble_s_tx_power_set, MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, mrb_esp32_ble, "tx_power_get", mrb_esp32_ble_s_tx_power_get, MRB_ARGS_REQ(1));

  struct RClass *mrb_esp32_bredr = mrb_define_class_under(mrb, mrb_esp32, "BREDR", mrb->object_class);
  mrb_define_class_method(mrb, mrb_esp32_bredr, "tx_power_set", mrb_esp32_bredr_s_tx_power_set, MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, mrb_esp32_bredr, "tx_power_get", mrb_esp32_bredr_s_tx_power_get, MRB_ARGS_NONE());

  struct RClass *mrb_esp32_bt_controller = mrb_define_class_under(mrb, mrb_esp32_bt, "Controller", mrb->object_class);
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "init", mrb_esp32_bt_controller_s_init, MRB_ARGS_OPT(1));
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "deinit", mrb_esp32_bt_controller_s_deinit, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "enable", mrb_esp32_bt_controller_s_enable, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "disable", mrb_esp32_bt_controller_s_disable, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "get_status", mrb_esp32_bt_controller_s_get_status, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "mem_release", mrb_esp32_bt_controller_s_mem_release, MRB_ARGS_REQ(1));

  struct RClass *mrb_esp32_vhci_host = mrb_define_class_under(mrb, mrb_esp32, "VHCIHost", mrb->object_class);
  mrb_define_class_method(mrb, mrb_esp32_vhci_host, "check_send_available", mrb_esp32_vhci_host_s_check_send_available, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_vhci_host, "send_packet", mrb_esp32_vhci_host_s_send_packet, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, mrb_esp32_vhci_host, "notify_host_send_available", mrb_esp32_vhci_host_s_notify_host_send_available, MRB_ARGS_BLOCK());
  mrb_define_class_method(mrb, mrb_esp32_vhci_host, "notify_host_recv_block", mrb_esp32_vhci_host_s_notify_host_recv_block, MRB_ARGS_BLOCK());

  struct RClass *mrb_esp32_bt_controller_config = mrb_define_class_under(mrb, mrb_esp32_bt, "ControllerConfig", mrb->object_class);
  mrb_define_method(mrb, mrb_esp32_bt_controller_config, "initialize", mrb_esp32_bt_controller_config_init, MRB_ARGS_REQ(4));

  /* esp_bt_mode_t */
  mrb_define_const(mrb, mrb_esp32_bt, "MODE_IDLE", mrb_fixnum_value(ESP_BT_MODE_IDLE));
  mrb_define_const(mrb, mrb_esp32_bt, "MODE_BLE", mrb_fixnum_value(ESP_BT_MODE_BLE));
  mrb_define_const(mrb, mrb_esp32_bt, "MODE_CLASSIC_BT", mrb_fixnum_value(ESP_BT_MODE_CLASSIC_BT));
  mrb_define_const(mrb, mrb_esp32_bt, "MODE_BTDM", mrb_fixnum_value(ESP_BT_MODE_BTDM));

  /* esp_bt_controller_status_t */
  mrb_define_const(mrb, mrb_esp32_bt, "CONTROLLER_STATUS_IDLE", mrb_fixnum_value(ESP_BT_CONTROLLER_STATUS_IDLE));
  mrb_define_const(mrb, mrb_esp32_bt, "CONTROLLER_STATUS_INITED", mrb_fixnum_value(ESP_BT_CONTROLLER_STATUS_INITED));
  mrb_define_const(mrb, mrb_esp32_bt, "CONTROLLER_STATUS_ENABLED", mrb_fixnum_value(ESP_BT_CONTROLLER_STATUS_ENABLED));
  mrb_define_const(mrb, mrb_esp32_bt, "CONTROLLER_STATUS_NUM", mrb_fixnum_value(ESP_BT_CONTROLLER_STATUS_NUM));

  /* esp_ble_power_type_t */
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_CONN_HDL0", mrb_fixnum_value(ESP_BLE_PWR_TYPE_CONN_HDL0));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_CONN_HDL1", mrb_fixnum_value(ESP_BLE_PWR_TYPE_CONN_HDL1));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_CONN_HDL2", mrb_fixnum_value(ESP_BLE_PWR_TYPE_CONN_HDL2));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_CONN_HDL3", mrb_fixnum_value(ESP_BLE_PWR_TYPE_CONN_HDL3));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_CONN_HDL4", mrb_fixnum_value(ESP_BLE_PWR_TYPE_CONN_HDL4));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_CONN_HDL5", mrb_fixnum_value(ESP_BLE_PWR_TYPE_CONN_HDL5));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_CONN_HDL6", mrb_fixnum_value(ESP_BLE_PWR_TYPE_CONN_HDL6));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_CONN_HDL7", mrb_fixnum_value(ESP_BLE_PWR_TYPE_CONN_HDL7));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_CONN_HDL8", mrb_fixnum_value(ESP_BLE_PWR_TYPE_CONN_HDL8));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_ADV", mrb_fixnum_value(ESP_BLE_PWR_TYPE_ADV));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_SCAN", mrb_fixnum_value(ESP_BLE_PWR_TYPE_SCAN));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_DEFAULT", mrb_fixnum_value(ESP_BLE_PWR_TYPE_DEFAULT));
  mrb_define_const(mrb, mrb_esp32_ble, "PWR_TYPE_NUM", mrb_fixnum_value(ESP_BLE_PWR_TYPE_NUM));

  /* esp_power_level_t */
  mrb_define_const(mrb, mrb_esp32, "PWR_LVL_N14", mrb_fixnum_value(ESP_PWR_LVL_N14));
  mrb_define_const(mrb, mrb_esp32, "PWR_LVL_N11", mrb_fixnum_value(ESP_PWR_LVL_N11));
  mrb_define_const(mrb, mrb_esp32, "PWR_LVL_N8", mrb_fixnum_value(ESP_PWR_LVL_N8));
  mrb_define_const(mrb, mrb_esp32, "PWR_LVL_N5", mrb_fixnum_value(ESP_PWR_LVL_N5));
  mrb_define_const(mrb, mrb_esp32, "PWR_LVL_N2", mrb_fixnum_value(ESP_PWR_LVL_N2));
  mrb_define_const(mrb, mrb_esp32, "PWR_LVL_P1", mrb_fixnum_value(ESP_PWR_LVL_P1));
  mrb_define_const(mrb, mrb_esp32, "PWR_LVL_P4", mrb_fixnum_value(ESP_PWR_LVL_P4));
  mrb_define_const(mrb, mrb_esp32, "PWR_LVL_P7", mrb_fixnum_value(ESP_PWR_LVL_P7));

  /* macros */
  mrb_value init_config_default = mrb_obj_value(mrb_data_object_alloc(mrb, mrb_esp32_bt_controller_config, NULL, &mrb_esp32_bt_controller_config_type));
  esp_bt_controller_config_t default_config = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_bt_controller_config_t *config = (esp_bt_controller_config_t *)mrb_malloc(mrb, sizeof(esp_bt_controller_config_t));
  *config = default_config;
  mrb_data_init(init_config_default, (void *)config, &mrb_esp32_bt_controller_config_type);
  mrb_define_const(mrb, mrb_esp32_bt_controller, "INIT_CONFIG_DEFAULT", init_config_default);

  // esp_gap_ble_api.h

  struct RClass *mrb_esp32_ble_gap = mrb_define_class_under(mrb, mrb_esp32_ble, "GAP", mrb->object_class);
  mrb_define_class_method(mrb, mrb_esp32_ble_gap, "register_callback", mrb_esp32_gap_s_register_callback, MRB_ARGS_BLOCK());

  /* esp_ble_gap_cb_param_t */
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "AdvDataCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "ScanRspDataCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "ScanParamCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "ScanRst", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "AdvDataRawCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "ScanRspDataRawCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "AdvStartCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "ScanStartCmpl", mrb->object_class);

  // esp_ble_sec_t
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "SecKeyNotif", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "SecReq", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "Key", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "LocalIdKeys", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "AuthCmpl", mrb->object_class);
  // esp_ble_sec_t

  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "ScanStopCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "AdvStopCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "SetRandAddrCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "UpdateConnParams", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "PktDataLenthCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "LocalPrivacyCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "RemoveBondDevCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "ClearBondDevCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "GetBondDevCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "ReadRssiCmpl", mrb->object_class);
  mrb_define_class_under(mrb, mrb_esp32_ble_gap, "UpdateWhitelistCmpl", mrb->object_class);

  /* struct RClass *mrb_esp32_ble_gap_callback_param = mrb_define_class_under(mrb, mrb_esp32_ble_gap, "CallbackParam", mrb->object_class); */

  /* esp_gap_ble_cb_event_t */
  struct RClass *mrb_esp32_ble_gap_callback_event = mrb_define_module_under(mrb, mrb_esp32_ble_gap, "CallbackEvent");
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "ADV_DATA_SET_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "SCAN_RSP_DATA_SET_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "SCAN_PARAM_SET_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "SCAN_RESULT_EVT", mrb_fixnum_value(ESP_GAP_BLE_SCAN_RESULT_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "ADV_DATA_RAW_SET_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "ADV_START_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_ADV_START_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "SCAN_START_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_SCAN_START_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "AUTH_CMPL_EVT", mrb_fixnum_value(ESP_GAP_BLE_AUTH_CMPL_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "KEY_EVT", mrb_fixnum_value(ESP_GAP_BLE_KEY_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "SEC_REQ_EVT", mrb_fixnum_value(ESP_GAP_BLE_SEC_REQ_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "PASSKEY_NOTIF_EVT", mrb_fixnum_value(ESP_GAP_BLE_PASSKEY_NOTIF_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "PASSKEY_REQ_EVT", mrb_fixnum_value(ESP_GAP_BLE_PASSKEY_REQ_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "OOB_REQ_EVT", mrb_fixnum_value(ESP_GAP_BLE_OOB_REQ_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "LOCAL_IR_EVT", mrb_fixnum_value(ESP_GAP_BLE_LOCAL_IR_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "LOCAL_ER_EVT", mrb_fixnum_value(ESP_GAP_BLE_LOCAL_ER_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "NC_REQ_EVT", mrb_fixnum_value(ESP_GAP_BLE_NC_REQ_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "ADV_STOP_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "SCAN_STOP_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "SET_STATIC_RAND_ADDR_EVT", mrb_fixnum_value(ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "UPDATE_CONN_PARAMS_EVT", mrb_fixnum_value(ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "SET_PKT_LENGTH_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "SET_LOCAL_PRIVACY_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "REMOVE_BOND_DEV_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "CLEAR_BOND_DEV_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "GET_BOND_DEV_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "READ_RSSI_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "UPDATE_WHITELIST_COMPLETE_EVT", mrb_fixnum_value(ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_callback_event, "EVT_MAX", mrb_fixnum_value(ESP_GAP_BLE_EVT_MAX));

  /* esp_ble_adv_data_type */
  struct RClass *mrb_esp32_ble_adv_data_type = mrb_define_module_under(mrb, mrb_esp32_ble, "AdvDataType");
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "FLAG", mrb_fixnum_value(ESP_BLE_AD_TYPE_FLAG));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SRV_PART_16", mrb_fixnum_value(ESP_BLE_AD_TYPE_16SRV_PART));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SRV_CMPL_16", mrb_fixnum_value(ESP_BLE_AD_TYPE_16SRV_CMPL));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SRV_PART_32", mrb_fixnum_value(ESP_BLE_AD_TYPE_32SRV_PART));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SRV_CMPL_32", mrb_fixnum_value(ESP_BLE_AD_TYPE_32SRV_CMPL));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SRV_PART_128", mrb_fixnum_value(ESP_BLE_AD_TYPE_128SRV_PART));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SRV_CMPL_128", mrb_fixnum_value(ESP_BLE_AD_TYPE_128SRV_CMPL));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "NAME_SHORT", mrb_fixnum_value(ESP_BLE_AD_TYPE_NAME_SHORT));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "NAME_CMPL", mrb_fixnum_value(ESP_BLE_AD_TYPE_NAME_CMPL));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "TX_PWR", mrb_fixnum_value(ESP_BLE_AD_TYPE_TX_PWR));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "DEV_CLASS", mrb_fixnum_value(ESP_BLE_AD_TYPE_DEV_CLASS));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SM_TK", mrb_fixnum_value(ESP_BLE_AD_TYPE_SM_TK));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SM_OOB_FLAG", mrb_fixnum_value(ESP_BLE_AD_TYPE_SM_OOB_FLAG));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "INT_RANGE", mrb_fixnum_value(ESP_BLE_AD_TYPE_INT_RANGE));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SOL_SRV_UUID", mrb_fixnum_value(ESP_BLE_AD_TYPE_SOL_SRV_UUID));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SOL_SRV_UUID_128", mrb_fixnum_value(ESP_BLE_AD_TYPE_128SOL_SRV_UUID));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SERVICE_DATA", mrb_fixnum_value(ESP_BLE_AD_TYPE_SERVICE_DATA));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "PUBLIC_TARGET", mrb_fixnum_value(ESP_BLE_AD_TYPE_PUBLIC_TARGET));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "RANDOM_TARGET", mrb_fixnum_value(ESP_BLE_AD_TYPE_RANDOM_TARGET));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "APPEARANCE", mrb_fixnum_value(ESP_BLE_AD_TYPE_APPEARANCE));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "ADV_INT", mrb_fixnum_value(ESP_BLE_AD_TYPE_ADV_INT));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "LE_DEV_ADDR", mrb_fixnum_value(ESP_BLE_AD_TYPE_LE_DEV_ADDR));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "LE_ROLE", mrb_fixnum_value(ESP_BLE_AD_TYPE_LE_ROLE));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SPAIR_C256", mrb_fixnum_value(ESP_BLE_AD_TYPE_SPAIR_C256));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SPAIR_R256", mrb_fixnum_value(ESP_BLE_AD_TYPE_SPAIR_R256));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SOL_SRV_UUID_32", mrb_fixnum_value(ESP_BLE_AD_TYPE_32SOL_SRV_UUID));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SERVICE_DATA_32", mrb_fixnum_value(ESP_BLE_AD_TYPE_32SERVICE_DATA));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SERVICE_DATA_128", mrb_fixnum_value(ESP_BLE_AD_TYPE_128SERVICE_DATA));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "LE_SECURE_CONFIRM", mrb_fixnum_value(ESP_BLE_AD_TYPE_LE_SECURE_CONFIRM));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "LE_SECURE_RANDOM", mrb_fixnum_value(ESP_BLE_AD_TYPE_LE_SECURE_RANDOM));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "URI", mrb_fixnum_value(ESP_BLE_AD_TYPE_URI));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "INDOOR_POSITION", mrb_fixnum_value(ESP_BLE_AD_TYPE_INDOOR_POSITION));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "TRANS_DISC_DATA", mrb_fixnum_value(ESP_BLE_AD_TYPE_TRANS_DISC_DATA));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "LE_SUPPORT_FEATURE", mrb_fixnum_value(ESP_BLE_AD_TYPE_LE_SUPPORT_FEATURE));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "CHAN_MAP_UPDATE", mrb_fixnum_value(ESP_BLE_AD_TYPE_CHAN_MAP_UPDATE));
  mrb_define_const(mrb, mrb_esp32_ble_adv_data_type, "SPECIFIC_TYPE", mrb_fixnum_value(ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE));

  /* esp_ble_adv_type_t */
  struct RClass *mrb_esp32_ble_adv_type = mrb_define_module_under(mrb, mrb_esp32_ble, "AdvType");
  mrb_define_const(mrb, mrb_esp32_ble_adv_type, "IND", mrb_fixnum_value(ADV_TYPE_IND));
  mrb_define_const(mrb, mrb_esp32_ble_adv_type, "DIRECT_IND_HIGH", mrb_fixnum_value(ADV_TYPE_DIRECT_IND_HIGH));
  mrb_define_const(mrb, mrb_esp32_ble_adv_type, "SCAN_IND", mrb_fixnum_value(ADV_TYPE_SCAN_IND));
  mrb_define_const(mrb, mrb_esp32_ble_adv_type, "NONCONN_IND", mrb_fixnum_value(ADV_TYPE_NONCONN_IND));
  mrb_define_const(mrb, mrb_esp32_ble_adv_type, "DIRECT_IND_LOW", mrb_fixnum_value(ADV_TYPE_DIRECT_IND_LOW));

  /* esp_ble_adv_channel_t */
  struct RClass *mrb_esp32_ble_adv_channel = mrb_define_module_under(mrb, mrb_esp32_ble, "AdvChannel");
  mrb_define_const(mrb, mrb_esp32_ble_adv_channel, "CH37", mrb_fixnum_value(ADV_CHNL_37));
  mrb_define_const(mrb, mrb_esp32_ble_adv_channel, "CH38", mrb_fixnum_value(ADV_CHNL_38));
  mrb_define_const(mrb, mrb_esp32_ble_adv_channel, "CH39", mrb_fixnum_value(ADV_CHNL_39));
  mrb_define_const(mrb, mrb_esp32_ble_adv_channel, "ALL", mrb_fixnum_value(ADV_CHNL_ALL));

  /* esp_ble_adv_filter_t */
  struct RClass *mrb_esp32_ble_adv_filter = mrb_define_module_under(mrb, mrb_esp32_ble, "AdvFilter");
  mrb_define_const(mrb, mrb_esp32_ble_adv_filter, "ALLOW_SCAN_ANY_CON_ANY", mrb_fixnum_value(ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY));
  mrb_define_const(mrb, mrb_esp32_ble_adv_filter, "ALLOW_SCAN_ANY_SCAN_WLST_CON_ANY", mrb_fixnum_value(ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY));
  mrb_define_const(mrb, mrb_esp32_ble_adv_filter, "ALLOW_SCAN_ANY_CON_WLST", mrb_fixnum_value(ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY));
  mrb_define_const(mrb, mrb_esp32_ble_adv_filter, "ALLOW_SCAN_WLST_CON_WLST", mrb_fixnum_value(ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY));

  /* esp_ble_sec_act_t */
  struct RClass *mrb_esp32_ble_sec = mrb_define_module_under(mrb, mrb_esp32_ble, "Sec");
  mrb_define_const(mrb, mrb_esp32_ble_sec, "ENCRYPT", mrb_fixnum_value(ESP_BLE_SEC_ENCRYPT));
  mrb_define_const(mrb, mrb_esp32_ble_sec, "ENCRYPT_NO_MITM", mrb_fixnum_value(ESP_BLE_SEC_ENCRYPT_NO_MITM));
  mrb_define_const(mrb, mrb_esp32_ble_sec, "ENCRYPT_MITM", mrb_fixnum_value(ESP_BLE_SEC_ENCRYPT_MITM));

  /* esp_ble_sm_param_t */
  struct RClass *mrb_esp32_ble_sm = mrb_define_module_under(mrb, mrb_esp32_ble, "SM");
  mrb_define_const(mrb, mrb_esp32_ble_sm, "PASSKEY", mrb_fixnum_value(ESP_BLE_SM_PASSKEY));
  mrb_define_const(mrb, mrb_esp32_ble_sm, "AUTHEN_REQ_MODE", mrb_fixnum_value(ESP_BLE_SM_AUTHEN_REQ_MODE));
  mrb_define_const(mrb, mrb_esp32_ble_sm, "IOCAP_MODE", mrb_fixnum_value(ESP_BLE_SM_IOCAP_MODE));
  mrb_define_const(mrb, mrb_esp32_ble_sm, "SET_INIT_KEY", mrb_fixnum_value(ESP_BLE_SM_SET_INIT_KEY));
  mrb_define_const(mrb, mrb_esp32_ble_sm, "SET_RSP_KEY", mrb_fixnum_value(ESP_BLE_SM_SET_RSP_KEY));
  mrb_define_const(mrb, mrb_esp32_ble_sm, "MAX_KEY_SIZE", mrb_fixnum_value(ESP_BLE_SM_MAX_KEY_SIZE));

  /* esp_ble_scan_type_t */
  struct RClass *mrb_esp32_ble_scan_type = mrb_define_module_under(mrb, mrb_esp32_ble, "ScanType");
  mrb_define_const(mrb, mrb_esp32_ble_scan_type, "PASSIVE", mrb_fixnum_value(BLE_SCAN_TYPE_PASSIVE));
  mrb_define_const(mrb, mrb_esp32_ble_scan_type, "ACTIVE", mrb_fixnum_value(BLE_SCAN_TYPE_ACTIVE));

  /* esp_ble_scan_filter_t */
  struct RClass *mrb_esp32_ble_scan_filter = mrb_define_module_under(mrb, mrb_esp32_ble, "ScanFilter");
  mrb_define_const(mrb, mrb_esp32_ble_scan_filter, "ALLOW_ALL", mrb_fixnum_value(BLE_SCAN_FILTER_ALLOW_ALL));
  mrb_define_const(mrb, mrb_esp32_ble_scan_filter, "ALLOW_ONLY_WLST", mrb_fixnum_value(BLE_SCAN_FILTER_ALLOW_ONLY_WLST));
  mrb_define_const(mrb, mrb_esp32_ble_scan_filter, "ALLOW_UND_RPA_DIR", mrb_fixnum_value(BLE_SCAN_FILTER_ALLOW_UND_RPA_DIR));
  mrb_define_const(mrb, mrb_esp32_ble_scan_filter, "ALLOW_WLIST_PRA_DIR", mrb_fixnum_value(BLE_SCAN_FILTER_ALLOW_WLIST_PRA_DIR));

  /* esp_gap_search_evt_t */
  struct RClass *mrb_esp32_ble_gap_search_event = mrb_define_module_under(mrb, mrb_esp32_ble_gap, "SearchEvent");
  mrb_define_const(mrb, mrb_esp32_ble_gap_search_event, "INQ_RES_EVT", mrb_fixnum_value(ESP_GAP_SEARCH_INQ_RES_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_search_event, "INQ_CMPL", mrb_fixnum_value(ESP_GAP_SEARCH_INQ_CMPL_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_search_event, "DISC_RES", mrb_fixnum_value(ESP_GAP_SEARCH_DISC_RES_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_search_event, "DISC_BLE_RES", mrb_fixnum_value(ESP_GAP_SEARCH_DISC_BLE_RES_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_search_event, "DISC_CMPL", mrb_fixnum_value(ESP_GAP_SEARCH_DISC_CMPL_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_search_event, "DI_DISC_CMPL", mrb_fixnum_value(ESP_GAP_SEARCH_DI_DISC_CMPL_EVT));
  mrb_define_const(mrb, mrb_esp32_ble_gap_search_event, "SEARCH_CANCEL_CMPL", mrb_fixnum_value(ESP_GAP_SEARCH_SEARCH_CANCEL_CMPL_EVT));

  /* esp_ble_evt_type_t */
  struct RClass *mrb_esp32_ble_event_type = mrb_define_module_under(mrb, mrb_esp32_ble, "EventType");
  mrb_define_const(mrb, mrb_esp32_ble_event_type, "CONN_ADV", mrb_fixnum_value(ESP_BLE_EVT_CONN_ADV));
  mrb_define_const(mrb, mrb_esp32_ble_event_type, "CONN_DIR_ADV", mrb_fixnum_value(ESP_BLE_EVT_CONN_DIR_ADV));
  mrb_define_const(mrb, mrb_esp32_ble_event_type, "DISC_ADV", mrb_fixnum_value(ESP_BLE_EVT_DISC_ADV));
  mrb_define_const(mrb, mrb_esp32_ble_event_type, "NON_CONN_ADV", mrb_fixnum_value(ESP_BLE_EVT_NON_CONN_ADV));
  mrb_define_const(mrb, mrb_esp32_ble_event_type, "SCAN_RSP", mrb_fixnum_value(ESP_BLE_EVT_SCAN_RSP));

  /* esp_ble_wl_opration_t */
  mrb_define_const(mrb, mrb_esp32_ble, "WHITELIST_REMOVE", mrb_fixnum_value(ESP_BLE_WHITELIST_REMOVE));
  mrb_define_const(mrb, mrb_esp32_ble, "WHITELIST_ADD", mrb_fixnum_value(ESP_BLE_WHITELIST_ADD));

  /* TODO macro */
}

void mrb_esp32_bluetooth_gem_final(mrb_state* mrb) {
}
