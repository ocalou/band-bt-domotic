project_name := py-band-localization

build:
	docker build -t $(project_name) -f ./devops/dockerfile .

run: build
	docker run --name $(project_name) $(project_name)

deploy: build
	docker run -d --name $(project_name) $(project_name)