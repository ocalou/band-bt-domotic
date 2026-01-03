"""MQTT Client"""
from typing import Callable
from loguru import logger as log

import paho.mqtt.client as mqtt


class MQTTClient:

    def __init__(self,
                 host: str,
                 port: int = 1883,
                 callback: Callable[str, bytes] = None) -> "MQTTClient":
        """
        Constructor for MQTTClient class

        :param str host: Host for MQTT Broker
        :param Callable[str, bytes] callback: Callback for MQTT message received
        """

        self._callback: Callable[str, bytes] = callback

        self._client: mqtt.Client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

        self._client.on_connect = self._on_connect

        self._client.connect(host = host,
                             port = port)

    def _on_connect(self, client, userdata, flags, reason_code, properties):
        log.success('Connected!')

    def publish(self,
                topic: str,
                msg: str):
        """
        Publishes message to MQTT broker

        :param str topic: Topic to publish message
        :param str msg: Message to send to payload
        """
        
        self._client.publish(topic = topic,
                             payload = msg)
        
    def _on_message(self,
                    client,
                    userdata,
                    msg):

        self._callback(msg.topic, msg.payload)

