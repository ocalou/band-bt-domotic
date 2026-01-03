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

        self._host: str = host
        self._port: int = port

        self._callback: Callable[str, bytes] = callback

        self._client: mqtt.Client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

        self._client.on_connect = self._on_connect
        self._client.on_subscribe = self._on_subscribe
        self._client.on_message = self._on_message

        self._client.connect(host = self._host,
                             port = self._port)
        
    def subscribe(self, topic: str):
        """
        Function for subscribing to MQTT topics
        
        :param topic: topic to subscribe to
        :type topic: str
        """

        self._client.subscribe(topic)
    
    def start(self):
        """
        Function to start ther MQTT client
        """
        
        self._client.loop_start()

    def stop(self):
        """
        Function to stop the MQTT client
        """

        self._client.disconnect()
            
    def _on_connect(self, client, userdata, flags, reason_code, properties):
        log.success('Connected!')

    def _on_subscribe(self, client, userdata, mid, reason_code_list, properties):
        if reason_code_list[0].is_failure:
            log.error(f"Broker rejected you subscription: {reason_code_list[0]}")
        else:
            log.success(f"Broker granted the following QoS: {reason_code_list[0].value}")

    def publish(self,
                topic: str,
                msg: str,
                retain: bool = False):
        """
        Publishes message to MQTT broker

        :param str topic: Topic to publish message
        :param str msg: Message to send to payload
        """
        
        self._client.publish(topic = topic,
                             payload = msg,
                             retain = retain)
        
    def _on_message(self,
                    client,
                    userdata,
                    msg):
        
        self._callback(topic = msg.topic,
                       payload = msg.payload,
                       client = self)
