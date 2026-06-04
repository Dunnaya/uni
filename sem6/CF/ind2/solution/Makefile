IMAGE ?= gitlab-registry.lre.epita.fr/tiger/buildenv/buildenv:latest
CONTAINER ?= tc-private
TC_DIR ?= $(abspath ../tc-base.plain)
TC_MOUNT ?= /tc
SHELL_CMD ?= sh

.PHONY: start ensure-container shell inspect stop destroy

ensure-container:
	@test -d "$(TC_DIR)" || (echo "Tiger source directory not found: $(TC_DIR)" && exit 1)
	@docker image inspect "$(IMAGE)" >/dev/null 2>&1 || docker pull "$(IMAGE)"
	@if ! docker ps -a --format '{{.Names}}' | grep -qx "$(CONTAINER)"; then \
		echo "Creating container $(CONTAINER) with bind mount $(TC_DIR):$(TC_MOUNT)"; \
		docker create -it --name "$(CONTAINER)" -v "$(TC_DIR):$(TC_MOUNT)" -w / "$(IMAGE)" "$(SHELL_CMD)" >/dev/null; \
	else \
		echo "Container $(CONTAINER) already exists"; \
	fi

start: ensure-container
	docker start "$(CONTAINER)" >/dev/null
	docker attach "$(CONTAINER)"

shell: ensure-container
	docker start "$(CONTAINER)" >/dev/null
	docker exec -it "$(CONTAINER)" "$(SHELL_CMD)"

inspect:
	docker inspect "$(CONTAINER)" --format '{{range .Mounts}}{{.Source}} -> {{.Destination}}{{println}}{{end}}'

stop:
	-docker stop "$(CONTAINER)"

destroy:
	-docker rm -f "$(CONTAINER)"