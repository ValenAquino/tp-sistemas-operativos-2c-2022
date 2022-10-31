RM=rm -rf
CC=gcc

H_SRCS=$(shell find ./shared/ -iname "*.h" | tr '\n' ' ')

texto_normal=\033[0m
texto_turquesa=\033[36m

ifneq ($(shell id -un),root)
SUDO=sudo
endif

# Clean and compile .so
all: install memoria kernel cpu consola

memoria: libshared.so
	@echo "\n$(texto_turquesa)Compilando memoria $(texto_normal)"
	@make all -C ./memoria/Debug

kernel: libshared.so
	@echo "\n$(texto_turquesa)Compilando kernel $(texto_normal)"
	@make all -C ./kernel/Debug

cpu: libshared.so
	@echo "\n$(texto_turquesa)Compilando cpu $(texto_normal)"
	@make all -C ./cpu/Debug

consola: libshared.so
	@echo "\n$(texto_turquesa)Compilando consola $(texto_normal)"
	@make all -C ./consola/Debug

libshared.so:
	@echo "\n$(texto_turquesa)Compilando shared $(texto_normal)"
	@make all -C ./shared/Debug

clean:
	@echo "\n$(texto_turquesa)Borrando memoria $(texto_normal)"
	@make clean -C ./memoria/Debug
	@echo "\n$(texto_turquesa)Borrando kernel $(texto_normal)"
	@make clean -C ./kernel/Debug
	@echo "\n$(texto_turquesa)Borrando cpu $(texto_normal)"
	@make clean -C ./cpu/Debug
	@echo "\n$(texto_turquesa)Borrando consola $(texto_normal)"
	@make clean -C ./consola/Debug

install: libshared.so
	@echo "\n$(texto_turquesa)Instalando shared $(texto_normal)"
	@$(SUDO) mkdir -p /usr/include/shared
	@echo "$(texto_turquesa)Moviendo libshared a usr/lib $(texto_normal)"
	@$(SUDO) cp -u shared/Debug/libshared.so /usr/lib
	@echo "$(texto_turquesa)Moviendo los headers $(texto_normal)"
	@$(SUDO) cp -u $(H_SRCS) /usr/include/shared
	@echo "$(texto_turquesa)Asignando permisos de usuario $(texto_normal)"
	@$(SUDO) chmod o+rwx -R /usr/include/shared
	@$(SUDO) chmod o+rwx /usr/lib/libshared.so

uninstall:
	@echo "\n$(texto_turquesa)Borrando shared $(texto_normal)"
	@make clean -C ./shared/Debug
	@echo "\n$(texto_turquesa)Desinstalando shared$(texto_normal)"
	$(SUDO) rm -f /usr/lib/libshared.so
	$(SUDO) rm -rf /usr/include/shared

.PHONY: all debug sources clean install uninstall