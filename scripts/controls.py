
import requests
import aiohttp
import asyncio
import msvcrt

ESP32_DRIVE_API = "http://192.168.86.29:80/drive"

print("###############################################")
print("########## RC ROBOT CONTROL TERMINAL ##########")
print("###############################################")
print("\nUse the WASD keys to control motor direction:")

async def sendAsyncRequest(url):
    async with aiohttp.ClientSession() as session:
        async with session.get(url) as response:
            if response.status == 200:
                return
            else:
                print("Request failed with status code:", response.status)

async def sendRobotCommand(c):
    if c in ['w', 'a', 's', 'd', 'q']:
        url = f"{ESP32_DRIVE_API}/{c}"
        try:
            await sendAsyncRequest(url)
        except Exception as e:
            print("An error occurred:", str(e))
    else:
        print("Invalid command")

def main():
    while True:
        """
        Get user input without needing to press enter.
        """
        input_chars = []

        if msvcrt.kbhit():
            char = msvcrt.getch().decode('utf-8')
            if char == '\x03':  # Break the loop on Ctrl+C (ASCII value 03)
                raise KeyboardInterrupt
            else:
                input_chars.append(char)
                print(char, end='', flush=True)  # Echo the character back to the console
                loop = asyncio.get_event_loop()
                loop.run_until_complete(sendRobotCommand(char))

try:
    main()
except KeyboardInterrupt:
    print("\nExiting.")
