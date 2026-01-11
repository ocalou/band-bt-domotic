from typing import Dict
import operator
from loguru import logger as log

from schemas import Automations, Automation
from clients import MQTTClient

class AutomationManager:

    operators: Dict = {
        '<': operator.lt,
        '<=': operator.le,
        '>': operator.gt,
        '>=': operator.ge,
        '==': operator.eq,
        '!=': operator.ne
    }

    def __init__(self,
                 file: str):
        """
        Constructor for AutomationManager
        
        :param file: File where the automations are defined
        :type file: str
        """

        self._automatizations: Automations = Automations.read(file)
        self._state_store: Dict = {}

    def update_state(self,
                     topic: str,
                     value: any):
        """
        Saves in memory all the messages received
        
        :param topic: Topic of the message received
        :type topic: str
        :param value: Payload of the message received
        :type value: any
        """
        
        self._state_store[topic] = value

    def evaluate(self,
                 trigger_topic: str,
                 payload: str,
                 mqtt_c: MQTTClient):
        """
        Checks the trigger_topic and executes the corresponding automation
        
        :param trigger_topic: Topic received
        :type trigger_topic: str
        :param payload: Payload received 
        :type payload: str
        :param mqtt_c: MQTT client
        :type mqtt_c: MQTTClient
        """
        
        automation: Automation = self._automatizations.automations.get(trigger_topic, None)

        if automation:
            log.debug(f'Activating "{automation.name}"')
            
            last_c : bool = True
            for cond in automation.conditions:

                if cond.topic:
                    payload = self._state_store.get(cond.topic, None)

                    if not payload:
                        log.debug(f'Not message for topic {cond.topic}')
                        return

                try:
                    val: int = int(payload)
                except (TypeError, ValueError):
                    val: str = str(payload)

                c: bool = self._check_condition(operator = cond.operator,
                                                value_a = val,
                                                value_b = cond.value)
                
                last_c = last_c and c
                
                if not last_c:
                    log.debug('No action realized')
                    return
            
            mqtt_c.publish(topic = automation.action.topic,
                            msg = automation.action.payload,
                            retain = automation.action.retain)
            log.debug('Action realized!')

    def _check_condition(self,
                        operator: str,
                        value_a: int | str,
                        value_b: int | str) -> bool:
        """
        Checks condition with the operatos and values
        
        :param operator: Operator to aply for checking condition
        :type operator: str
        :param value_a: value received by MQTT
        :type value_a: int | str
        :param value_b: value defined in automation
        :type value_b: int | str
        :return: Response from evaluating the condition
        :rtype: bool
        """

        log.debug(f'Checking {value_a} {operator} {value_b}')

        return self.operators[operator](value_a, value_b) if operator else False
