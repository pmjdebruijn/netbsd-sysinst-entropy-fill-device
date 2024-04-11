
.venv:
	python3 -m venv .venv

	.venv/bin/pip install -U wheel --no-cache-dir
	.venv/bin/pip install -U platformio --no-cache-dir

build: .venv
	.venv/bin/platformio run

upload: .venv
	.venv/bin/platformio run -t upload
