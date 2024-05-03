import aiohttp
import asyncio
import msvcrt

ESP32_DRIVE_API = "http://192.168.86.22:80/drive"

print()
print("###############################################")
print("########## RC ROBOT CONTROL TERMINAL ##########")
print("###############################################")
print("\nUse the WASD keys to control motor direction:")

async def sendAsyncRequest(url):
    async with aiohttp.ClientSession() as session:
        try:
            # Fire-and-forget, no handling of response
            await session.get(url)
        except Exception as e:
            # Optionally log the error
            # print(f"Failed to send request: {str(e)}")
            pass

async def sendRobotCommand(c):
    if c in ['w', 'a', 's', 'd', 'q']:
        url = f"{ESP32_DRIVE_API}/{c}"
        # Schedule the task but do not await it
        asyncio.create_task(sendAsyncRequest(url))
    else:
        print("Invalid command", flush=True)

async def main():
    while True:
        await asyncio.sleep(0.05) # Small delay to allow other coroutines to run
        if msvcrt.kbhit():
            char = msvcrt.getch().decode('utf-8')
            if char == '\x03':  # Handle Ctrl+C in asyncio
                break
            print(char, end='', flush=True)  # Echo the character to console
            asyncio.create_task(sendRobotCommand(char))

try:
    asyncio.run(main())
except KeyboardInterrupt:
    print("\nExiting.")