
from time import sleep

from configuration import Configuration
from clients import MQTTClient, BandManager

if __name__ == '__main__':

    config: Configuration = Configuration.read('resources/config.yaml')
    band_manager: BandManager = BandManager('resources/bands.yaml')
    mqtt_c: MQTTClient = MQTTClient(host = config.MQTT_PARAMS.host,
                                    port = config.MQTT_PARAMS.port,
                                    callback = band_manager.callback)
    
    mqtt_c.start() 
    mqtt_c.subscribe(config.MQTT_PARAMS.subscribe_topic)
    
    while True:

        sleep(config.SERVICE_PARAMS.delay)