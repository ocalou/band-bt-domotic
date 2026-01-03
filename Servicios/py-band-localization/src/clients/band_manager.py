from loguru import logger as log

from schemas import Bands
from .mqtt import MQTTClient

class BandManager:

    def __init__(self,
                 file: str = 'resources/bands.yaml') -> "BandManager":
        
        """
        Constructor
        
        :param file: Path to file with bands defined
        :type file: str
        """
        
        self.bands: Bands = Bands.read(file)

    def callback(self,
                 topic: str,
                 payload: bytes,
                 client: MQTTClient):
        
        log.debug(f'Received: {payload} in topic: {topic}')
        
        _, mac, room = topic.split('/')

        if mac in self.bands.bands_list.keys():

            last_rssi = self.bands.bands_list[mac].rssi
            new_rssi = float(payload)

            if not last_rssi:
                last_rssi = new_rssi

            if new_rssi >= last_rssi:

                self.bands.bands_list[mac].room = room
                self.bands.bands_list[mac].rssi = new_rssi

                publish_topic = f'loc/band/{self.bands.bands_list[mac].user}'

                log.debug(f'Publishing \'{room}\' to topic: {publish_topic}')

                client.publish(topic = publish_topic,
                               msg = room,
                               retain = True)
