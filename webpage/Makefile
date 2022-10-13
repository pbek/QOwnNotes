.PHONY: bash

bash:
	#docker run -it --rm --volume .:/app --user $(id -u) node:12 bash
	#docker-compose run --rm dev bash -c "cd /app && bash"
	docker-compose exec dev bash -c "cd /app && bash"

dev:
	yarn run dev
