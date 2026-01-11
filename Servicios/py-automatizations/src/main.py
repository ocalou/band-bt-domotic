from time import sleep

from automation import AutomationManager
from clients import MQTTClient
from configuration import Configuration

if __name__ == '__main__':

    config: Configuration = Configuration.read('./resources/config.yaml')
    a: AutomationManager = AutomationManager(file = './resources/automations.yaml')

    def callback(topic: str,
                 payload: bytes,
                 client: MQTTClient):
        
        a.update_state(topic = topic,
                       value = payload.decode('utf-8'))
        
        a.evaluate(trigger_topic = topic,
                   payload = payload.decode('utf-8'),
                   mqtt_c = client)

    mqtt_c: MQTTClient = MQTTClient(host = config.MQTT_PARAMS.host,
                                    port = config.MQTT_PARAMS.port,
                                    callback = callback)
    
    mqtt_c.start()
    mqtt_c.subscribe('#')
    
    while(1):
        sleep(1)
