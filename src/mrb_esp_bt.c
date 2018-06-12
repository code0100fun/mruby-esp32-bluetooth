#include <esp_bt.h>
#include <freertos/FreeRTOS.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/data.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby-esp32-ext_esp_err.h>

static mrb_state *mrb_esp32_bluetooth_mrb;
static mrb_value notify_host_send_available_block;
static mrb_value notify_host_recv_block;

static const struct mrb_data_type mrb_esp32_bt_controller_config_type = {
  "esp_bt_controller_config_t", mrb_free
};

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

void mrb_esp_bt_init(mrb_state *mrb, struct RClass *mrb_esp32, struct RClass *mrb_esp32_bt, struct RClass *mrb_esp32_ble, struct RClass *mrb_esp32_bredr, struct RClass *mrb_esp32_bt_controller, struct RClass *mrb_esp32_vhci_host, struct RClass *mrb_esp32_bt_controller_config) {
  mrb_esp32_bluetooth_mrb = mrb;
  notify_host_send_available_block = mrb_nil_value();
  notify_host_recv_block = mrb_nil_value();
  esp_vhci_host_register_callback(&mrb_esp32_vhci_host_callback);

  // ESP32::BLE
  mrb_define_class_method(mrb, mrb_esp32_ble, "tx_power_set", mrb_esp32_ble_s_tx_power_set, MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, mrb_esp32_ble, "tx_power_get", mrb_esp32_ble_s_tx_power_get, MRB_ARGS_REQ(1));

  // ESP32::BREDR
  mrb_define_class_method(mrb, mrb_esp32_bredr, "tx_power_set", mrb_esp32_bredr_s_tx_power_set, MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, mrb_esp32_bredr, "tx_power_get", mrb_esp32_bredr_s_tx_power_get, MRB_ARGS_NONE());

  // ESP32::BT::Controller
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "init", mrb_esp32_bt_controller_s_init, MRB_ARGS_OPT(1));
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "deinit", mrb_esp32_bt_controller_s_deinit, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "enable", mrb_esp32_bt_controller_s_enable, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "disable", mrb_esp32_bt_controller_s_disable, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "get_status", mrb_esp32_bt_controller_s_get_status, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_bt_controller, "mem_release", mrb_esp32_bt_controller_s_mem_release, MRB_ARGS_REQ(1));

  // ESP32::VHCIHost
  mrb_define_class_method(mrb, mrb_esp32_vhci_host, "check_send_available", mrb_esp32_vhci_host_s_check_send_available, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, mrb_esp32_vhci_host, "send_packet", mrb_esp32_vhci_host_s_send_packet, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, mrb_esp32_vhci_host, "notify_host_send_available", mrb_esp32_vhci_host_s_notify_host_send_available, MRB_ARGS_BLOCK());
  mrb_define_class_method(mrb, mrb_esp32_vhci_host, "notify_host_recv_block", mrb_esp32_vhci_host_s_notify_host_recv_block, MRB_ARGS_BLOCK());

  // ESP32::BT::ControllerConfig
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
}
