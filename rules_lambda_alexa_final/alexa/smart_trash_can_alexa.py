import logging
import json
import boto3
import ask_sdk_core.utils as ask_utils
from ask_sdk_core.skill_builder import SkillBuilder
from ask_sdk_core.dispatch_components import AbstractRequestHandler, AbstractExceptionHandler
from ask_sdk_core.handler_input import HandlerInput
from ask_sdk_core.utils import is_request_type, is_intent_name
from ask_sdk_model import Response

logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

iot_client = boto3.client('iot-data', region_name='us-east-1', endpoint_url='https://ahxlba0g2uxk8-ats.iot.us-east-1.amazonaws.com')

USER_THING_MAP = {
    "amzn1.ask.account.AMAX6BUWVHT5IWW7PWJV62F2RWJFEMTT7PATDHMYQKZGGH5TIEGLBKKHC463AQJDGXGGCQ6VGYW4HJVGFNU4E6Q67QEAWTI6RBMESTLXXNAIHSENCMJMZR53TPP7LSEMKF52AYEKFCF57O6PTX54AM3EJISAT7RA6F4KMGKH33BLQU2DVRK53OUNZ3RIFLZZVDZ24BLPBEFZHWIO4QSULGMTEJYADE7RXSUZYPEOK3IA": "smart_trash_can_sn0001"
}

def get_thing_name(handler_input):
    user_id = handler_input.request_envelope.session.user.user_id
    return USER_THING_MAP.get(user_id)

def get_shadow_state(thing_name):
    try:
        response = iot_client.get_thing_shadow(thingName=thing_name)
        payload = json.loads(response['payload'].read())
        return payload.get("state", {}).get("reported", {})
    except Exception as e:
        logger.error(f"Error fetching shadow state: {e}")
        return {}

def update_shadow_state(thing_name, desired_state, force_reported=False):
    payload = {"state": {"desired": desired_state}}
    iot_client.update_thing_shadow(thingName=thing_name, payload=json.dumps(payload))
    if force_reported:
        iot_client.update_thing_shadow(thingName=thing_name, payload=json.dumps({"state": {"reported": desired_state}}))

class LaunchRequestHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return ask_utils.is_request_type("LaunchRequest")(handler_input)

    def handle(self, handler_input):
        speak_output = "Welcome to your smart trash can. Do you want to use it in automatic mode or manual mode?"
        return handler_input.response_builder.speak(speak_output).ask(speak_output).response

class GetTrashCanStatusIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return is_intent_name("GetTrashCanStatusIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = get_thing_name(handler_input)
        if not thing_name:
            return handler_input.response_builder.speak("Trash can not found.").ask("Try again.").response

        status = get_shadow_state(thing_name)
        if not status:
            return handler_input.response_builder.speak("I couldn't access the trash can state.").ask("Try again?").response

        speech = (
            f"The lid is {'open' if status.get('lid_open') else 'closed'}. "
            f"Distance is {status.get('depth_cm', 'unknown')} cm. "
            f"The LED is {status.get('led_color', 'off')}. "
            f"The filling state is {status.get('filling_state', 'unknown')}. "
            f"Automatic mode is {'on' if status.get('automatic_mode') else 'off'}."
        )
        return handler_input.response_builder.speak(speech).ask("Anything else?").response

class OpenLidIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return is_intent_name("OpenLidIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = get_thing_name(handler_input)
        shadow = get_shadow_state(thing_name)
        if shadow.get("automatic_mode"):
            return handler_input.response_builder.speak(
                "I can't open the lid manually while in automatic mode. Please switch to manual mode."
            ).ask("Would you like to switch to manual mode?").response

        if shadow.get("lid_open"):
            return handler_input.response_builder.speak("The lid is already open.").ask("Anything else?").response

        update_shadow_state(thing_name, {"lid_open": True})
        return handler_input.response_builder.speak("Opening the lid.").ask("Anything else?").response

class CloseLidIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return is_intent_name("CloseLidIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = get_thing_name(handler_input)
        shadow = get_shadow_state(thing_name)

        if not shadow.get("lid_open"):
            return handler_input.response_builder.speak("The lid is already closed.").ask("Anything else?").response

        update_shadow_state(thing_name, {"lid_open": False})
        return handler_input.response_builder.speak("Closing the lid.").ask("Anything else?").response

class AutomaticModeHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return is_intent_name("AutomaticModeIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = get_thing_name(handler_input)
        shadow = get_shadow_state(thing_name)

        if shadow.get("automatic_mode", False):
            return handler_input.response_builder.speak("It is already in automatic mode.").ask("Anything else?").response

        update_shadow_state(thing_name, {"automatic_mode": True}, force_reported=True)
        return handler_input.response_builder.speak("Switching to automatic mode.").ask("Anything else?").response

class NoAutomaticModeHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return is_intent_name("NoAutomaticModeIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = get_thing_name(handler_input)
        shadow = get_shadow_state(thing_name)

        if not shadow.get("automatic_mode", False):
            return handler_input.response_builder.speak("It is already in manual mode.").ask("Anything else?").response

        update_shadow_state(thing_name, {"automatic_mode": False}, force_reported=True)
        return handler_input.response_builder.speak("Switching to manual mode.").ask("What would you like to do next?").response

class CatchAllExceptionHandler(AbstractExceptionHandler):
    def can_handle(self, handler_input, exception):
        return True

    def handle(self, handler_input, exception):
        logger.error(exception, exc_info=True)
        return handler_input.response_builder.speak("Sorry, something went wrong.").ask("Please try again.").response

sb = SkillBuilder()
sb.add_request_handler(LaunchRequestHandler())
sb.add_request_handler(GetTrashCanStatusIntentHandler())
sb.add_request_handler(OpenLidIntentHandler())
sb.add_request_handler(CloseLidIntentHandler())
sb.add_request_handler(AutomaticModeHandler())
sb.add_request_handler(NoAutomaticModeHandler())
sb.add_exception_handler(CatchAllExceptionHandler())

lambda_handler = sb.lambda_handler()
