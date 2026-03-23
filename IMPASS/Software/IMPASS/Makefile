# Makefile para test unitarios del sistema IMPASS
ENV = esp32-s3-custom-uart

.PHONY: lm75_log lm75_pot all_tests build deploy monitor all clean

# --- Tests de LM75 ---
lm75_log:
	@echo "Ejecutando tests de LM75 en Placa Lógica..."
	pio test -e $(ENV) --filter="test_lm75_logica" 

lm75_pot:
	@echo "Ejecutando tests de LM75 en Placa Potencia..."
	pio test -e $(ENV) --filter="test_lm75_potencia"

all_tests:
	pio test -e $(ENV)

# --- Compilación y Carga ---
build:
	@echo "Compilando proyecto IMPASS..."
	pio run -e $(ENV)

deploy:
	@echo "Cargando firmware en la placa..."
	pio run -e $(ENV) --target upload

monitor:
	@echo "Abriendo monitor serie..."
	pio device monitor -e $(ENV)

all: build deploy monitor

clean:
	@echo "Limpiando archivos temporales..."
	pio run -e $(ENV) --target clean