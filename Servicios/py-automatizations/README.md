# py-automatizations
Servicio que se encarga de las automatizaciones.

## Ficheros

La estructura de ficheros es la siguiente:

```
.
├── devops
├── resources
└── src
    ├── automation
    ├── clients
    ├── configuration
    └── schemas
```

- **[devops](./devops/)**: directorio donde se encuentran los ficheros para el despliegue del servicio.
- **[resources](./resources/)**: directorio donde se encuentran los ficheros de configuración asi como los requisitos para el despliegue.
- **[src](./src/)**: directorio con el codigo fuente del servicio.

## Configuracion

Para la configuracion del servicio, existe un [fichero](./resources/config.yaml) donde se pueden definir los parametros.

Se puede definir en el apartado de **mqtt**:
- **host**: ip del broker mqtt.
- **port**: puerto del broker mqtt.

## Funcionamiento

Para la definicion de las automatizaciones, existe un [fichero](./resources/automations.yaml) donde se definen las condiciones y las acciones de las automatizaciones.

Por ejemplo:

```yaml
automations:
  - name: "Encender calefacción inteligentemente"
    trigger_topic: "casa/sensor/temperatura"
    conditions:
      - value: 18
        operator: "<"
      - topic: "casa/estado/presencia"
        value: "ON"
        operator: "=="
    action:
      topic: "casa/calefaccion/set"
      payload: "ON"
      retain: False
```

- **name**: nombre de la automatización.
- **trigger_topic**: tópico a escuchar para activar la automatización.
- **conditions**: condiciones a cumplir para que se realice la acción.
    - **value**: valor a usar para comparar con el valor recibido por MQTT.
    - **operator**: operador que se usa para comparar los valores.
    - **topic** (opcional): se puede definir un topic para que se utilice el ultimo mensaje recibido en ese topico como condición.
- **action**: acción a realizar si se cumplen todas las condiciones.
    - **topic**: tópico en el que se publica el mensaje.
    - **payload**: mensaje a enviar.
    - **retain**: flag para definir si se retiene el mensaje en el broker MQTT.

Para poder usar los valores recibidos en otros tópicos, el servicio se suscribe a todos los tópicos y va almacenando los valores de los tópicos recibidos. Esto para que si se utiliza el valor de un tópico en alguna de las condiciones, se pueda saber cuál fue el último valor recibido.

## Despliegue
Existe un fichero [dockerfile](./devops/dockerfile) para la creacion de la imagen del contenedor docker para el despliegue del servicio.

Existe un [Makefile](./Makefile) para la construccion de la imagen y la ejecucion del contenedor.

- ```make build```: construye la imagen del contenedor.
-  ```make run```: inicia el contenedor.
-  ```make deploy```: inicia el contenedor en segundo plano.