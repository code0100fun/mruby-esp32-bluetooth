MRuby::Gem::Specification.new('esp32-bluetooth') do |spec|
  spec.license = 'MIT'
  spec.author  = 'Seiei Miyagi'
  spec.summary = 'ESP-IDF Bluetooth API wrapper'
  spec.add_dependency('esp32-ext_esp_err', :github => 'code0100fun/mruby-esp32-ext_esp_err')
end
